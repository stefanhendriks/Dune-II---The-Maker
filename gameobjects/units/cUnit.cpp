/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "d2tmh.h"
#include "cUnit.h"

#include "utils/cSoundPlayer.h"

#include <fmt/core.h>

#include <cmath>

// Path creation definitions / var
#define CLOSED        -1
#define OPEN          0

struct ASTAR {
    int cost;
    int parent;
    int state;
};

// Temp map
ASTAR temp_map[16384]; // 4096 = 64x64 map, 16384 = 128x128 map

// Class specific on top
// Globals on bottom

void cUnit::init(int i) {
    mission = -1;
    boundParticleId = -1;
    bSelected = false;
    bHovered  = false;

    unitsEaten = 0;

    isReinforcement = false; // set to true by REINFORCE when a carry-all is spawned to bring a 'real' reinforcement. So we can
                             // emit the proper CREATED game event later. :/

    fExperience = 0;

    iID = i;

    iType = 0;          // type of unit

    iHitPoints = -1;     // hitpoints of unit
    iTempHitPoints = -1; // temp hold back for 'reinforced' / 'dumping' and 'repairing' units

    iPlayer = -1;        // belongs to player

    bRemoveMe = false;

    iGroup = -1;

    // Movement
    iNextCell = -1;      // where to move to (next cell)
    iGoalCell = -1;      // the goal cell (goal of path)
    iCell = -1;          // cell of unit
    iCellX = -1;
    iCellY = -1;
    posX = -1;
    posY = -1;
    memset(iPath, -1, sizeof(iPath));    // path of unit
    iPathIndex = -1;     // where are we?
    iPathFails = 0;
    bCalculateNewPath = false;

    lastDroppedOffCell = -1;

    bCarryMe = false;        // carry this unit when moving it around?
    iCarryAll = -1;        // any carry-all that will pickup this unit... so this unit knows
    // it should wait when moving, etc, etc


    m_action = eActionType::GUARD;
    intent = INTENT_NONE;

    iAttackUnit = -1;      // attacking unit id
    iAttackStructure = -1; // attack structure id
    iAttackCell = -1;

    // selected
    bSelected = false;

    // Action given code
    iUnitID = -1;        // Unit ID to attack/pickup, etc
    iStructureID = -1;   // structure ID to attack/bring to (refinery)

    // Carry-All specific
    m_transferType = eTransferType::NONE;
    // iUnitIDWithinStructure = unit we CARRY (when TransferType == 1)
    // iTempHitPoints = hp of unit when transfertype = 1

    iCarryTarget = -1;    // Unit ID to carry, but is not carried yet
    iBringTarget = -1;    // Where to bring the carried unit (when iUnitIDWithinStructure > -1)
    iNewUnitType = -1;    // new unit that will be brought, will be this type
    bPickedUp = false;    // carry-all/frigate: picked up the unit? (TODO: change this into states!)
    willBePickedUpBy = -1;// ID of the carry-all that will pick up this unit

    // harv
    iCredits = 0;

    // Drawing
    iBodyFacing = 0;    // Body of tanks facing
    iHeadFacing = 0;    // Head of tanks facing
    iBodyShouldFace = iBodyFacing;    // where should the unit body look at?
    iHeadShouldFace = iHeadFacing;    // where should th eunit look at?

    iFrame = 0;

    // TIMERS
    TIMER_blink = 0;
    TIMER_move = 0;
    TIMER_movewait = 0;
    TIMER_thinkwait = 0;    // wait with normal thinking..
    TIMER_turn = 0;
    TIMER_frame = 0;
    TIMER_harvest = 0;
    TIMER_guard = 0;    // guard scanning timer
    TIMER_bored = 0;    // how long are we bored?
    TIMER_attack = 0;
    TIMER_wormtrail = 0;
    TIMER_movedelay = 0;
}

void cUnit::recreateDimensions() {
    // set up dimensions
    dimensions = cRectangle(draw_x(), draw_y(), getBmpWidth(), getBmpHeight());
}

void cUnit::setBoundParticleId(int particleId) {
    this->boundParticleId = particleId;
}

void cUnit::die(bool bBlowUp, bool bSquish) {
    // DO NOTE: We do *not* set the HP to -1 here for a reason. Being: that the isValid() function checks for
    // health and that will give us a unit ID that is the *same* as this unit ID. (see UNIT_NEW() implementation).
    // hence we don't want to fiddle with that (for now), but instead we set a "removeMe" flag.
    //
    // Usually the Hitpoints will be < 0; but the die() function could be called without a unit having < 0 HP for now
    // via debug (and other) possible means. Hence, we don't want to rely on this and introduced this flag.
    //
    // TODO: this should be revisited and fixed in a later version properly!
    bRemoveMe = true;

    // any damage particle dies with the unit?
    if (boundParticleId > -1) {
        cParticle &pParticle = particle[boundParticleId];
        if (pParticle.isValid()) {
            pParticle.die();
        }
        boundParticleId = -1;
    }

    // Animation / Sound

    // Anyone who was attacking this unit is on actionGuard
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue; // skip invalid
        if (pUnit.iAttackUnit != iID) continue; // skip those who did not want to attack me

        pUnit.actionGuard();
    }

    if (iStructureID > -1) {
        unitWillNoLongerBeInteractingWithStructure();
    }

    if (isAirbornUnit()) {
        if (iUnitID > -1) {
            // we intended to pick up this unit
            cUnit &pUnit = unit[iUnitID];
            if (pUnit.isValid()) {
                pUnit.willBePickedUpBy = -1; // no longer being picked up by this one
            }
        }
    } else {
        tellCarryAllThatWouldPickMeUpToForgetAboutMe();
    }

    if (bBlowUp) {
        createExplosionParticle();
    }

    if (bSquish) {
        createSquishedParticle();
    }

    if (iStructureID > -1) {
        cAbstractStructure *pStructure = structure[iStructureID];
        if (pStructure && pStructure->isValid()) {
            // TODO: Use events, and let structure deal with this themselves!
        }
    }

    // before re-initing, send out event, so in case we need to handle the event and fetch the data from that
    // entity then we can atleast pry it for data...
    s_GameEvent event {
            .eventType = eGameEventType::GAME_EVENT_DESTROYED,
            .entityType = eBuildType::UNIT,
            .entityID = iID,
            .player = getPlayer(),
            .entitySpecificType = iType
    };

    game.onNotifyGameEvent(event);

    init(iID);    // re-init

    map.remove_id(iID, MAPID_UNITS);
    map.remove_id(iID, MAPID_AIR);
    map.remove_id(iID, MAPID_WORMS);
}

void cUnit::createSquishedParticle() {
    int iDieX = pos_x_centered();
    int iDieY = pos_y_centered();
    // when we do not 'blow up', we died by something else. Only infantry will be 'squished' here now.
    if (iType == SOLDIER || iType == TROOPER || iType == UNIT_FREMEN_ONE) {
        int iType1 = D2TM_PARTICLE_SQUISH01 + rnd(2);
        cParticle::create(iDieX, iDieY, iType1, iPlayer, iFrame);
        play_sound_id_with_distance(SOUND_SQUISH, distanceBetweenCellAndCenterOfScreen(iCell));
    } else if (iType == TROOPERS || iType == INFANTRY || iType == UNIT_FREMEN_THREE) {
        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_SQUISH03, iPlayer, iFrame);
        play_sound_id_with_distance(SOUND_SQUISH, distanceBetweenCellAndCenterOfScreen(iCell));
    }
}

void cUnit::createExplosionParticle() {
    int iDieX = pos_x_centered();
    int iDieY = pos_y_centered();

    if (iType == TRIKE || iType == RAIDER || iType == QUAD) {
        // play quick 'boom' sound and show animation
        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_TRIKE, -1, -1);
        play_sound_id_with_distance(SOUND_TRIKEDIE, distanceBetweenCellAndCenterOfScreen(iCell));

        if (rnd(100) < 30) {
            cParticle::create(iDieX, iDieY - 24, D2TM_PARTICLE_SMOKE, -1, -1);
        }
    }

    if ((iType == SIEGETANK || iType == DEVASTATOR) && rnd(100) < 25) {
        if (iBodyFacing == FACE_UPLEFT ||
            iBodyFacing == FACE_DOWNRIGHT) {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_SIEGEDIE, iPlayer, -1);
        }
    }

    if (iType == TANK || iType == SIEGETANK || iType == SONICTANK || iType == LAUNCHER || iType == DEVIATOR ||
        iType == HARVESTER || iType == ORNITHOPTER || iType == MCV || iType == FRIGATE) {
        // play quick 'boom' sound and show animation
        if (rnd(100) < 50) {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_TANK_ONE, -1, -1);
            play_sound_id_with_distance(SOUND_TANKDIE2, distanceBetweenCellAndCenterOfScreen(iCell));
        } else {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_TANK_TWO, -1, -1);
            play_sound_id_with_distance(SOUND_TANKDIE, distanceBetweenCellAndCenterOfScreen(iCell));
        }

        if (rnd(100) < 30) {
            cParticle::create(iDieX, iDieY - 24, D2TM_PARTICLE_SMOKE, -1, -1);
        }

        if (iType == HARVESTER) {
            game.shakeScreen(25);
            mapEditor.createRandomField(iCell, TERRAIN_SPICE, ((iCredits + 1) / 7));
        }

        // For now carry-all and ornithopter share same death particle
        if (iType == ORNITHOPTER || iType == CARRYALL) {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_ORNI, -1, -1);
        }

        // Frigate death particle? (doesnt exist in Dune 2, but would be cool to have)
    }

    // when Saboteur dies let it explode like Devastator!
    if (iType == DEVASTATOR || iType == SABOTEUR) {
        int iOrgDieX = iDieX;
        int iOrgDieY = iDieY;


        // create a cirlce of explosions (big ones)
        iDieX -= 32;
        iDieY -= 32;

        for (int cx = 0; cx < 3; cx++)
            for (int cy = 0; cy < 3; cy++) {


                for (int i = 0; i < 2; i++) {
                    int iType1 = D2TM_PARTICLE_EXPLOSION_STRUCTURE01 + rnd(2);
                    cParticle::create(iDieX + (cx * 32), iDieY + (cy * 32), iType1, -1, -1);
                }

                if (rnd(100) < 35)
                    play_sound_id_with_distance(SOUND_TANKDIE + rnd(2),
                                                distanceBetweenCellAndCenterOfScreen(iCell));

                // calculate cell and damage stuff around this
                int cll = map.getCellWithMapBorders((iCellX - 1) + cx, (iCellY - 1) + cy);

                if (cll < 0 || cll == iCell)
                    continue; // do not do own cell

                if (map.getCellType(cll) == TERRAIN_WALL) {
                    // damage this type of wall...
                    map.cellTakeDamage(cll, 150);

                    if (map.getCellHealth(cll) < 0) {
                        // remove wall, turn into smudge:
                        mapEditor.createCell(cll, TERRAIN_ROCK, 0);

                        mapEditor.smoothAroundCell(cll);

                        map.smudge_increase(SMUDGE_WALL, cll);
                    }
                }

                // damage surrounding units
                int idOfUnitAtCell = map.getCellIdUnitLayer(cll);
                if (idOfUnitAtCell > -1) {
                    int id = idOfUnitAtCell;

                    if (unit[id].iHitPoints > 0) {
                        unit[id].iHitPoints -= 150;

                        // NO HP LEFT, DIE
                        if (unit[id].iHitPoints <= 1)
                            unit[id].die(true, false);
                    } // only die when the unit is going to die
                }

                int idOfStructureAtCell = map.getCellIdStructuresLayer(cll);
                if (idOfStructureAtCell > -1) {
                    // structure hit!
                    int id = idOfStructureAtCell;

                    cAbstractStructure *pStructure = structure[id];
                    if (pStructure->getHitPoints() > 0) {

                        int iDamage = 150 + rnd(100);
                        pStructure->damage(iDamage);

                        int iChance = 10;

                        if (pStructure &&
                            pStructure->getHitPoints() < (sStructureInfo[pStructure->getType()].hp / 2)) {
                            iChance = 30;
                        }

                        if (rnd(100) < iChance) {
                            long x = pos_x() + (mapCamera->getViewportStartX()) + 16 + (-8 + rnd(16));
                            long y = pos_y() + (mapCamera->getViewportStartY()) + 16 + (-8 + rnd(16));
                            cParticle::create(x, y, D2TM_PARTICLE_SMOKE, -1, -1);
                        }
                    }
                }


                int cellType = map.getCellType(cll);
                if (cellType == TERRAIN_ROCK) {
                    if (cellType != TERRAIN_WALL)
                        map.cellTakeDamage(cll, 30);

                    if (map.getCellHealth(cll) < -25) {
                        map.smudge_increase(SMUDGE_ROCK, cll);
                        map.cellGiveHealth(cll, rnd(25));
                    }
                } else if (cellType == TERRAIN_SAND ||
                           cellType == TERRAIN_HILL ||
                           cellType == TERRAIN_SPICE ||
                           cellType == TERRAIN_SPICEHILL) {
                    if (cellType != TERRAIN_WALL)
                        map.cellTakeDamage(cll, 30);

                    if (map.getCellHealth(cll) < -25) {
                        map.smudge_increase(SMUDGE_SAND, cll);
                        map.cellGiveHealth(cll, rnd(25));
                    }
                }
            }


        cParticle::create(iOrgDieX, iOrgDieY, D2TM_PARTICLE_OBJECT_BOOM02, -1, -1);

        cParticle::create(iOrgDieX - 32, iOrgDieY, D2TM_PARTICLE_OBJECT_BOOM02, -1, -1);
        cParticle::create(iOrgDieX + 32, iOrgDieY, D2TM_PARTICLE_OBJECT_BOOM02, -1, -1);
        cParticle::create(iOrgDieX, iOrgDieY - 32, D2TM_PARTICLE_OBJECT_BOOM02, -1, -1);
        cParticle::create(iOrgDieX, iOrgDieY + 32, D2TM_PARTICLE_OBJECT_BOOM02, -1, -1);

    }

    if (iType == TROOPER || iType == SOLDIER || iType == UNIT_FREMEN_ONE) {
        // create particle of dead body

        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_DEADINF02, iPlayer, -1);

        play_sound_id_with_distance(SOUND_DIE01 + rnd(5), distanceBetweenCellAndCenterOfScreen(iCell));
    }

    if (iType == TROOPERS || iType == INFANTRY || iType == UNIT_FREMEN_THREE) {
        // create particle of dead body

        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_DEADINF01, iPlayer, -1);

        play_sound_id_with_distance(SOUND_DIE01 + rnd(5), distanceBetweenCellAndCenterOfScreen(iCell));
    }
}

/**
 * Any unit this structure was "interacting with" (ie, wanted to enter), make sure
 * to remove that connection.
 */
void cUnit::unitWillNoLongerBeInteractingWithStructure() const {
    cAbstractStructure *pStructure = getStructureUnitWantsToEnter();
    if (pStructure == nullptr) return; // nothing to do here

    if (pStructure->isValid()) {
        pStructure->unitIsNoLongerInteractingWithStructure(iID);
        if (iUnitID > -1) {
            // in case I am a Carry-All and Carry-ing a unit
            pStructure->unitIsNoLongerInteractingWithStructure(iUnitID);
        }
    }
}


/**
 * Returns true when it belongs to a player. If removeMe flag is set, it does not care if it is dead yet; and will return true.
 * Meaning, a dead unit still is valid when removeMe flag is set. This is usually done when a unit is in the process of
 * dying but the HP is > 0.
 * @return
 */
bool cUnit::isValid() const {
    if (iPlayer < 0)
        return false;

    // invalid cell, not good
    if (iCell < 0 || iCell >= map.getMaxCells())
        return false;

    // not marked (not dying) so do a health check. Else, don't care about health check.
    if (bRemoveMe == false) {
        // no hitpoints (dead) and not in a structure
        if (iHitPoints < 0 && iTempHitPoints < 0)
            return false;
    }

    // when eligible for removal, it is still 'valid'. We do this so other code does not pick this unit
    // while it is in the process of dying or something. The whole resource cleaning and creating should
    // be done differently, this is a mess.
    // see also UNIT_NEW() which returns an id of a 'unused' unit based on the isValid() function.
    return true;
}

int cUnit::pos_x() {
    return posX;
}

int cUnit::pos_y() {
    return posY;
}

int cUnit::draw_x() {
    return draw_x(getBmpWidth());
}

int cUnit::draw_x(int bmpWidth) {
    // Example:
    // unit with 48 width (harvester, carry-all)
    // (32 - 48) = -16 / 2 = -8
    int bmpOffset = (TILESIZE_WIDTH_PIXELS - bmpWidth) / 2;
    // x is -8 pixels (ie, 8 pixels more to the left than top-left corner (pos_x) of a cell
    return mapCamera->getWindowXPositionWithOffset(pos_x(), bmpOffset);
}

int cUnit::draw_y() {
    return draw_y(getBmpHeight());
}

int cUnit::draw_y(int bmpHeight) {
    // same as draw_x
    int bmpOffset = (TILESIZE_HEIGHT_PIXELS - bmpHeight) / 2;
    return mapCamera->getWindowYPositionWithOffset(pos_y(), bmpOffset);
}

int cUnit::center_draw_x() {
    return mapCamera->getWindowXPosition(pos_x_centered());
}

int cUnit::center_draw_y() {
    return mapCamera->getWindowYPosition(pos_y_centered());
}

int cUnit::getBmpHeight() const {
    return sUnitInfo[iType].bmp_height;
}

void cUnit::draw_spice() {
    float width_x = mapCamera->factorZoomLevel(getBmpWidth());
    int height_y = mapCamera->factorZoomLevel(4);
    int drawx = draw_x();
    int drawy = draw_y() - ((height_y * 2) + 2);

    int max = getUnitInfo().credit_capacity;
    int w = health_bar(width_x, iCredits, max);

    // bar itself
    rectfill(bmp_screen, drawx, drawy, drawx + width_x, drawy + height_y, makecol(0, 0, 0));
    rectfill(bmp_screen, drawx, drawy, drawx + (w), drawy + height_y, makecol(255, 91, 1));

    // bar around it (only when it makes sense due zooming)
    if (height_y > 2) {
        rect(bmp_screen, drawx, drawy, drawx + width_x, drawy + height_y, makecol(255, 255, 255));
    }
}

int cUnit::getBmpWidth() const {
    return sUnitInfo[iType].bmp_width;
}

float cUnit::getHealthNormalized() {
    s_UnitInfo &unitType = getUnitInfo();
    float flMAX = unitType.hp;
    return (iHitPoints / flMAX);
}

float cUnit::getTempHealthNormalized() {
    s_UnitInfo &unitType = getUnitInfo();
    float flMAX = unitType.hp;
    return (iTempHitPoints / flMAX);
}

void cUnit::draw_health() {
    if (iHitPoints < 0) return;

    // draw units health
    float width_x = mapCamera->factorZoomLevel(getBmpWidth());
    int height_y = mapCamera->factorZoomLevel(4);
    int drawx = draw_x();
    int drawy = draw_y() - (height_y + 2);

    float healthNormalized = getHealthNormalized();

    int w = healthNormalized * width_x;
    int r = (1.1 - healthNormalized) * 255;
    int g = healthNormalized * 255;

    if (r > 255) r = 255;

    // bar itself
    rectfill(bmp_screen, drawx, drawy, drawx + width_x, drawy + height_y, makecol(0, 0, 0));
    rectfill(bmp_screen, drawx, drawy, drawx + (w - 1), drawy + height_y, makecol(r, g, 32));

    // bar around it (only when it makes sense due zooming)
    if (height_y > 2) {
        rect(bmp_screen, drawx, drawy, drawx + width_x, drawy + height_y, makecol(255, 255, 255));
    }

    // draw group
    if (iGroup > 0 &&
        iPlayer == HUMAN) {
        alfont_textprintf(bmp_screen, bene_font, drawx + 26, drawy - 11, makecol(0, 0, 0), "%d", iGroup);
        alfont_textprintf(bmp_screen, bene_font, drawx + 26, drawy - 12, makecol(255, 255, 255), "%d", iGroup);
    }

}

// this method returns the amount of percent extra damage may be done
float cUnit::fExpDamage() {
    if (fExperience < 1) return 0; // no stars

    // MAX EXPERIENCE = 10 (9 stars)

    // A unit can do 2x damage on full experience; being very powerfull.
    // it does take a long time to get there though.
    float fResult = (fExperience / 10);

    // never make a unit weaker.
    if (fResult < 1.0) fResult = 1.0F;
    return fResult;
}

void cUnit::draw_experience() {
    // draws experience above health

    int iStars = (int) fExperience;

    if (iStars < 1)
        return; // no stars to draw!

    int iStarType = 0;

    // twice correct and upgrade star type
    if (iStars > 2) {
        iStarType++;
        iStars -= 3;
    }

    // red stars now, very much experience!
    if (iStars > 2) {
        iStarType++;
        iStars -= 3;
    }

    // still enough experience! wow
    if (iStars > 2) {
        iStars = 3;
    }


    int drawx = draw_x() + 3;
    int drawy = draw_y() - 19;

    // 1 star = 1 experience
    for (int i = 0; i < iStars; i++) {
        draw_sprite(bmp_screen, (BITMAP *) gfxdata[OBJECT_STAR_01 + iStarType].dat, drawx + i * 9, drawy);
    }

}

void cUnit::draw_path() const {
    // for debugging purposes
    if (iCell == iGoalCell)
        return;

    if (iPath[0] < 0)
        return;

    int halfTile = 16;
    int iPrevX = mapCamera->getWindowXPositionFromCellWithOffset(iPath[0], halfTile);
    int iPrevY = mapCamera->getWindowYPositionFromCellWithOffset(iPath[0], halfTile);

    for (int i = 1; i < MAX_PATH_SIZE; i++) {
        if (iPath[i] < 0) break;
        int iDx = mapCamera->getWindowXPositionFromCellWithOffset(iPath[i], halfTile);
        int iDy = mapCamera->getWindowYPositionFromCellWithOffset(iPath[i], halfTile);

        if (i == iPathIndex) { // current node we navigate to
            line(bmp_screen, iPrevX, iPrevY, iDx, iDy, makecol(255, 255, 255));
        } else if (iPath[i] == iGoalCell) {
            // end of path (goal)
            line(bmp_screen, iPrevX, iPrevY, iDx, iDy, makecol(255, 0, 0));
        } else {
            // everything else
            line(bmp_screen, iPrevX, iPrevY, iDx, iDy, makecol(255, 255, 64));
        }

        // draw a line from previous to current
        iPrevX = iDx;
        iPrevY = iDy;
    }
}

void cUnit::draw() {
    if (iTempHitPoints > -1) {
        // temp hitpoints filled, meaning it is not visible (but not dead). Ie, it is being repaired, or transfered
        // by carry-all
        return;
    }

    // Selection box x, y position. Depends on unit size
    const int ux = draw_x();
    const int uy = draw_y();

    if (isSandworm()) {
        // randomize drawing shimmer effect, as it is expensive
        if (rnd(100) < 15) {
            Shimmer(TILESIZE_HEIGHT_PIXELS, center_draw_x(), center_draw_y());
        }
        return;
    }

    s_UnitInfo &unitType = getUnitInfo();
    const int bmp_width = unitType.bmp_width;
    const int bmp_height = unitType.bmp_height;

    // the multiplier we will use to draw the unit
    const int bmp_head = convert_angle(iHeadFacing);
    const int bmp_body = convert_angle(iBodyFacing);

    // draw body first
    int start_x = bmp_body * bmp_width;
    int start_y = bmp_height * iFrame;

    cPlayer &cPlayer = players[this->iPlayer];

    // Draw SHADOW
    BITMAP *shadow = cPlayer.getUnitShadowBitmap(iType, bmp_body, iFrame);
    const float scaledWidth = mapCamera->factorZoomLevel(bmp_width);
    const float scaledHeight = mapCamera->factorZoomLevel(bmp_height);

    if (shadow) {
        int destY = uy;

        if (iType == CARRYALL) {
            // adjust X and Y so it looks like a carry-all is 'higher up in the air'
            destY = uy + 24; // TODO; do something with height here? the closer to target, the less distant the shadow?
        }

        int colorDepth = bitmap_color_depth(bmp_screen);
        BITMAP *stretchedShadow = create_bitmap_ex(colorDepth, scaledWidth, scaledHeight);
        clear_to_color(stretchedShadow, makecol(255, 0, 255));
        allegroDrawer->maskedStretchBlit(shadow, stretchedShadow, 0, 0, bmp_width, bmp_height,
                                         0, 0,
                                         scaledWidth, scaledHeight);

        allegroDrawer->drawTransSprite(stretchedShadow, bmp_screen, ux, destY);
        destroy_bitmap(shadow);
        destroy_bitmap(stretchedShadow);
    }

    // Draw BODY
    BITMAP *bitmap = cPlayer.getUnitBitmap(iType);
    if (bitmap) {
        allegroDrawer->maskedStretchBlit(bitmap, bmp_screen, start_x, start_y, bmp_width, bmp_height,
                                         ux, uy,
                                         scaledWidth,
                                         scaledHeight);
    } else {
        log(fmt::format("unit of iType [{}] did not have a bitmap!?", iType));
    }

    // Draw TOP
    BITMAP *top = cPlayer.getUnitTopBitmap(iType);
    if (top && iHitPoints > -1) {
        // recalculate start_x using head instead of body
        start_x = bmp_head * bmp_width;
        start_y = bmp_height * iFrame;

        allegroDrawer->maskedStretchBlit(top, bmp_screen, start_x, start_y, bmp_width, bmp_height, ux, uy,
                                         mapCamera->factorZoomLevel(bmp_width), mapCamera->factorZoomLevel(bmp_height));
    }

    // TODO: Fix this / Draw BLINKING (ie, when targeted unit)
//	if (TIMER_blink > 0)
//		if (rnd(100) < 15)
//			mask_to_color(temp, makecol(255,255,255));

    // when we want to be picked up..
    if (bCarryMe) {
        draw_sprite(bmp_screen, (BITMAP *) gfxdata[SYMB_PICKMEUP].dat, ux, uy - 7);
    }

    if (bSelected) {
        BITMAP *focusBitmap = (BITMAP *) gfxdata[FOCUS].dat;
        int bmp_width = focusBitmap->w;
        int bmp_height = focusBitmap->h;

        int x = draw_x(bmp_width);
        int y = draw_y(bmp_height);

        allegroDrawer->maskedStretchBlit(focusBitmap,
                                         bmp_screen,
                                         0, 0, bmp_width, bmp_height, x, y,
                                         mapCamera->factorZoomLevel(bmp_width),
                                         mapCamera->factorZoomLevel(bmp_height)
                                         );
    }

    if (game.m_drawUnitDebug) {
        // render pixel at the very center
        putpixel(bmp_screen, center_draw_x(), center_draw_y(), makecol(255, 255, 0));

        // render from the units top-left to center pixel
        line(bmp_screen, draw_x(), draw_y(), center_draw_x(), center_draw_y(), makecol(255, 255, 0));
    }
}

// TODO: only do this when iCell is updated
void cUnit::updateCellXAndY() {
    iCellX = map.getCellX(iCell);
    iCellY = map.getCellY(iCell);
}

void cUnit::attackUnit(int targetUnit) {
    log(fmt::format("attackUnit() : target is [{}]", targetUnit));
    attack(unit[targetUnit].iCell, targetUnit, -1, -1);
}

void cUnit::attackStructure(int targetStructure) {
    log(fmt::format("attackStructure() : target is [{}]", targetStructure));
    attack(structure[targetStructure]->getCell(), -1, targetStructure, -1);
}

void cUnit::attackCell(int cell) {
    log(fmt::format("attackCell() : cell target is [{}]", cell));
    attack(cell, -1, -1, cell);
}

void cUnit::attack(int goalCell, int unitId, int structureId, int attackCell) {
    // basically the same as move, but since we use m_action as ATTACK
    // it will think first in attack mode, determining if it will be CHASE now or not.
    // if not, it will just fire.

    log(fmt::format("Attacking UNIT ID [{}], STRUCTURE ID [{}], ATTACKCLL [{}], GoalCell [{}]",
                    unitId, structureId, attackCell, goalCell));

    if (unitId < 0 && structureId < 0 && attackCell < 0) {
        log("What is this? Ordered to attack but no target?");
        return;
    }

    // TODO: We have somewhere else something with "intents", so this whole if statement should be removed / replaced?
    if (isSaboteur()) {
        // saboteur does not attack, but only captures
        move_to(goalCell, structureId, -1, eUnitActionIntent::INTENT_CAPTURE);
        return;
    }

    m_action = eActionType::GUARD;
    this->iGoalCell = goalCell;
    iAttackStructure = structureId;
    iAttackUnit = unitId;
    this->iAttackCell = attackCell;
    forgetAboutCurrentPathAndPrepareToCreateNewOne(rnd(5));
}

void cUnit::attackAt(int cell) {
    log(fmt::format("attackAt() : cell target is [{}]", cell));

    if (!map.isWithinBoundaries(cell)) {
        log("attackAt() : Invalid cell, aborting");
        return;
    }

    int unitId = map.getCellIdUnitLayer(cell);
    int structureId = map.getCellIdStructuresLayer(cell);
    log(fmt::format("attackAt() : cell target is [{}], structureId [{}], unitId [{}]", cell, structureId, unitId));

    if (structureId > -1) {
        attackStructure(structureId);
        return;
    }

    if (unitId > -1) {
        attackUnit(unitId);
        return;
    }

    attackCell(cell);
}

void cUnit::move_to(int iCll, int iStructureIdToEnter, int iUnitIdToPickup) {
    move_to(iCll, iStructureIdToEnter, iUnitIdToPickup, eUnitActionIntent::INTENT_MOVE);
}

void cUnit::move_to(int iCll, int iStructureIdToEnter, int iUnitIdToPickup, eUnitActionIntent intent) {
    log(fmt::format("(move_to - START) : to cell [{}], iStructureIdToEnter[{}], iUnitIdToPickup[{}] (to attack, if > -1), intent[{}]",
                    iCll, iStructureIdToEnter, iUnitIdToPickup, eUnitActionIntentString(intent)));
    iGoalCell = iCll;
    if (iStructureID > -1) {
        unitWillNoLongerBeInteractingWithStructure();
    }

    tellCarryAllThatWouldPickMeUpToForgetAboutMe();

    iStructureID = iStructureIdToEnter;

    if (iStructureIdToEnter > -1) {
        cAbstractStructure *pStructure = structure[iStructureIdToEnter];
        if (!pStructure->hasUnitHeadingTowards() && !pStructure->hasUnitWithin()) {
            pStructure->unitHeadsTowardsStructure(iID);
        }
    }

    iUnitID = iUnitIdToPickup;

    iAttackStructure = -1;
    iAttackCell = -1;

    // only when not moving (half on tile) reset nextcell
    if (!isMovingBetweenCells()) {
        iNextCell = -1;
    }

    m_action = eActionType::MOVE;
    this->intent = intent;

    forgetAboutCurrentPathAndPrepareToCreateNewOne();

    log("(move_to - FINISHED)");
}

/**
 * If this unit would be picked up by a carry-all (identifiedby willBePickedUpBy ID), then let it know we no
 * longer want to be picked up. Ie, we died, or we wanted to move away ourselves and the carry-all should not
 * interfere.
 */
void cUnit::tellCarryAllThatWouldPickMeUpToForgetAboutMe() const {
    if (willBePickedUpBy > -1) {
        cUnit &pUnit = unit[willBePickedUpBy];
        if (pUnit.isValid()) {
            pUnit.forgetAboutUnitToPickUp();
        }
    }
}


/**
 * Called every 5 ms, when action = ACTION_GUARD
 */
void cUnit::thinkFast_guard() {
    if (!isValid()) {
        return;
    }

    // unit is not able to 'guard', so guarding does nothing.
    if (!isAbleToGuard()) {
        return;
    }

    TIMER_bored++;
    if (TIMER_bored > 3500) {
        TIMER_bored = 0;
        iBodyShouldFace = rnd(8);
        iHeadShouldFace = rnd(8);
    }

    if (TIMER_guard > 0) {
        TIMER_guard--; // scan time
    }

    if (TIMER_movewait > 0) {
        TIMER_movewait--;
    }

    if (TIMER_movewait > 0 || TIMER_guard > 0) {
        return;
    }

    // scan area
    TIMER_guard = 20 + rnd(35); // do not scan all at the same time

    updateCellXAndY();

    if (isSandworm()) {
        thinkFast_guard_sandworm();
        return;
    }

    // not sandworm
    // TODO: make 'guard range' configurable for ground units
    int unitIdToAttack = findNearbyGroundUnitToAttack(getSight());

    // can attack air units, and no ground threats found. (prioritize ground over air units!)
    if (canAttackAirUnits() && unitIdToAttack < 0) {
        // TODO: make 'guard range' configurable for air units
        int range = getSight() + 3; // do react earlier than already in range for air units
        int airUnitToAttack = findNearbyAirUnitToAttack(range);

        if (airUnitToAttack > -1) {
            // air units override ground units for units that can attack air units ?
            // TODO: Make this configurable?
            unitIdToAttack = airUnitToAttack;
        }
    }

    if (unitIdToAttack > -1) {
        cUnit &unitToAttack = unit[unitIdToAttack];

        if (unitToAttack.isValid()) {
//            s_GameEvent event{
//                    .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
//                    .entityType = eBuildType::UNIT,
//                    .entityID = unitToAttack.iID,
//                    .player = getPlayer(),
//                    .entitySpecificType = unitToAttack.getType(),
//                    .atCell = unitToAttack.iCell
//            };
//            game.onNotifyGameEvent(event);
        }

        // TODO: move this code somewhere else?
        if (!getPlayer()->isHuman()) {
            if (unitToAttack.isInfantryUnit() && canSquishInfantry()) {
                // AI will try to squish infantry units
                move_to(unitToAttack.iCell);
            } else {
                attackUnit(unitIdToAttack);
            }
        } else {
            attackUnit(unitIdToAttack);
        }

        return;
    }

    // no unit found for attacking
    // TODO: think about how to make this generic and deviate from default behavior elsewhere?
    if (!getPlayer()->isHuman()) {
        // TODO: Move this to the AI / brain classes?
        // ai units will auto-attack structures nearby

        int range = getSight() + 3;
        int id = findNearbyStructureToAttack(range);

        if (id > -1) {
            attackStructure(id);
        }
        return;
    } else {
        // human units do auto-attack structures that can attack things
        int id = findNearbyStructureThatCanDamageUnitsToAttack(getRange());
        if (id > -1) {
            attackStructure(id);
        }
    }
}

bool cUnit::isSandworm() const {
    return iType == SANDWORM;
}

/**
 * Called every 100ms
 */
void cUnit::think() {
    thinkActionAgnostic();

    // Think attack style
    if (m_action == eActionType::ATTACK) {
        think_attack();
    }
}

void cUnit::thinkActionAgnostic() {
    if (TIMER_blink > 0) {
        TIMER_blink--;
    }

    if (iType == MCV) {
        think_MVC();
    }

    // HEAD is not facing correctly
    if (!isAirbornUnit()) {
        if (iBodyFacing == iBodyShouldFace) {
            if (iHeadFacing != iHeadShouldFace) {
                TIMER_turn++;
                if (TIMER_turn > getTurnSpeed()) {
                    TIMER_turn = 0;
                    iHeadFacing = determineNewFacing(iHeadFacing, iHeadShouldFace);
                } // turn
            } // head facing

        } else {
            think_turn_to_desired_body_facing();
        }

    } else {
        // air units, have only 'body' facing
        if (iBodyFacing != iBodyShouldFace) {
            think_turn_to_desired_body_facing();
        }
    }

    // when waiting.. wait
    if (TIMER_thinkwait > 0) {
        TIMER_thinkwait--;
        return;
    }

    if (iTempHitPoints > -1)
        return;

    // when any non-airborn, non-sandworm unit is on a spice bloom, it dies
    int cellType = map.getCellType(iCell);
    if (!isAirbornUnit() && !isSandworm() && cellType == TERRAIN_BLOOM) {
        map.detonateSpiceBloom(iCell);
        die(true, false);
        return;
    }

    // --- think
    if (iType == ORNITHOPTER) {
        think_ornithopter();
        return;
    }

    // HARVESTERs logic here
    if (iType == HARVESTER) {
        think_harvester();
    }

    // When this is a carry-all, show proper animation when filled
    if (iType == CARRYALL) {
        think_carryAll();
    }
}

cAbstractStructure * cUnit::findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(int structureType) {
    return map.findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(iCell, structureType, getPlayer());
}

cAbstractStructure * cUnit::findClosestAvailableStructureType(int structureType) {
    return map.findClosestAvailableStructureType(iCell, structureType, getPlayer());
}

cAbstractStructure * cUnit::findClosestStructureType(int structureType) {
    return map.findClosestStructureType(iCell, structureType, getPlayer());
}

void cUnit::think_carryAll() {// A carry-all has something when:
// - it carries a unit (iUnitIDWithinStructure > -1)
// - it has the flag eTransferType::NEW_

    if ((m_transferType == eTransferType::NEW_STAY ||
         m_transferType == eTransferType::NEW_LEAVE ||
         m_transferType == eTransferType::PICKUP) || iUnitID > -1) {

        // when picking up a unit.. only draw when picked up
        if (m_transferType == eTransferType::PICKUP && bPickedUp)
            iFrame = 1;

        // any other transfer, means it is filled from start...
        if (m_transferType != eTransferType::PICKUP)
            iFrame = 1;
    } else {
        iFrame = 0;
    }
}

void cUnit::think_ornithopter() {
    cPlayer *pPlayer = getPlayer();
    iFrame++;

    if (iFrame > 3) {
        iFrame = 0;
    }

    if (iAttackUnit < 0 && iAttackStructure < 0) {
        selectTargetForOrnithopter(pPlayer);
    } else {
        TIMER_attack++;
    }
}

void cUnit::selectTargetForOrnithopter(cPlayer *pPlayer) {
    // find enemy unit or structure to attack
    // units within range are taking priority, else we select a random structure.
    int iDistance = 9999;
    int iTarget = -1;

    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &target = unit[i];
        if (target.isValid() && i != iID) {
            if (pPlayer->isSameTeamAs(target.getPlayer()))
                continue;

            // not ours and its visible
            if (target.iPlayer != iPlayer &&
                map.isVisible(target.iCell, iPlayer) &&
                !target.isAirbornUnit()) // for now, to prevent orni's taking down carry-alls?
            {
                int distance = ABS_length(iCellX, iCellY, target.iCellX, target.iCellY);

                if (distance <= getRange() && distance < iDistance) {
                    // ATTACK
                    iDistance = distance;
                    iTarget = i;
                }
            }

        }
    }

    // target known?
    if (iTarget > -1) {
        attackUnit(iTarget);
        return;
    } else {
        // no unit found, attack structure
        // scan for enemy activities.
        int iDistance = 9999;
        int iTarget = -1;

        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *pStructure = structure[i];
            if (!pStructure) continue;
            if (!pStructure->isValid()) continue;

            // skip same team
            if (pPlayer->isSameTeamAs(pStructure->getPlayer()))
                continue;

            // not ours and its visible
            if (pStructure->getPlayerId() != iPlayer && // enemy
                map.isStructureVisible(pStructure, iPlayer)) {
                int c = pStructure->getCell();
                int distance = map.distance(iCell, c);

                // attack closest structure
                if (distance < iDistance) {
                    iDistance = distance;
                    iTarget = i;
                }
            }
        }

        if (iTarget > -1) {
            attackStructure(iTarget);
        }
    }
}

void cUnit::think_turn_to_desired_body_facing() {
    // BODY is not facing correctly
    TIMER_turn++;

    float turnspeed = sUnitInfo[iType].turnspeed;
    if (isAirbornUnit()) {
        // when closer to goal, turnspeed decreases.
        double distance = map.distance(iCell, iGoalCell);
        int distanceInCells = 8;
        if (distance < distanceInCells) {
            turnspeed = (turnspeed/distanceInCells) * distance;
        } else {
            // when close to a border, then reduce turnspeed so that orni's wont crash over the map borders
            if ((iCellX < 4 || iCellX >= (map.getWidth()-4)) || (iCellY < 4 || iCellY >= (map.getHeight()-4))) {
                turnspeed = 0;
            }
        }
    }

    if (TIMER_turn > turnspeed) {
        TIMER_turn = 0;

        iBodyFacing = determineNewFacing(iBodyFacing, iBodyShouldFace);
    } // turn body
}

int cUnit::determineNewFacing(int currentFacing, int desiredFacing) {
    int newFacing = currentFacing;

    int d = 1;

    int toleft = (newFacing + 8) - desiredFacing;
    if (toleft > 7) toleft -= 8;

    int toright = abs(toleft - 8);

    if (toright == toleft) d = -1 + (rnd(2));
    if (toleft > toright) d = 1;
    if (toright > toleft) d = -1;

    newFacing += d;

    if (newFacing < 0)
        newFacing = 7;

    if (newFacing > 7)
        newFacing = 0;

    return newFacing;
}

// aircraft specific thinking
void cUnit::thinkFast_move_airUnit() {
    if (iTempHitPoints > -1) {
        return;
    }

    if (TIMER_movewait > 0) {
        TIMER_movewait--;
        return;
    }

    iNextCell = getNextCellToMoveTo();

    if (!map.isValidCell(iCell)) {
        die(true, false);

        // KILL UNITS WHO SOMEHOW GET INVALID
        if (DEBUGGING)
            log("ERROR: Unit became invalid somehow, killed it");
        return;
    }

    if (map.isAtMapBoundaries(iCell)) {
        if (!isReinforcement) {
            // let unit face directly to ideal angle, so it won't fly into its doom (out of map)
            iBodyFacing = iBodyShouldFace;
            iHeadFacing = iHeadShouldFace;
        }
    }

    if (!map.isValidCell(iNextCell))
        iNextCell = iCell;

    if (!map.isValidCell(iGoalCell))
        iGoalCell = iCell;

    // same cell (no goal specified or something)
    if (iNextCell == iCell) {
        bool isWithinMapBoundaries = map.isWithinBoundaries(iCellX, iCellY);

        // reinforcement stuff happens here...
        if (m_transferType == eTransferType::DIE) {
            // kill (probably reached border or something)
            die(false, false);
            return;
        }

        // transfer wants to pickup and drop a unit...
        if (m_transferType == eTransferType::PICKUP) {
            if (iUnitID > -1) {
                // Not yet picked up the unit
                cUnit &unitToPickupOrDrop = getUnitToPickupOrDrop();
                if (!bPickedUp) {
                    if (!unitToPickupOrDrop.isValid() || unitToPickupOrDrop.isDead()) {
                        forgetAboutUnitToPickUp();
                        return;
                    }

                    // I believe this statement is always true, as we do not set flag
                    // on unit that is being picked up!? (unit to pick up is not changed, the CARRY-ALL is though)
                    if (unitToPickupOrDrop.bPickedUp == false) {
                        // CARRY-ALL: when we are at the same cell as the unit to pick up
                        if (iCell == unitToPickupOrDrop.iCell) {
                            // when this unit is NOT moving
                            if (!unitToPickupOrDrop.isMovingBetweenCells()) {
                                TIMER_movedelay = 300; // this will make carry-all speed up slowly

                                unitToPickupOrDrop.willBePickedUpBy = -1; // set state in aircraft, that it has picked up a unit
                                bPickedUp = true;

                                // so we set the tempHitpoints so the unit 'disappears' from the map without being
                                // really dead. TODO: use state for this instead
                                unitToPickupOrDrop.iTempHitPoints = unitToPickupOrDrop.iHitPoints;

                                // now remove hitpoints (HACK HACK)
                                unitToPickupOrDrop.iHitPoints = -1;

                                // remove unit from map id (so it wont block other units)
                                map.cellResetIdFromLayer(iCell, MAPID_UNITS);

                                // now move air unit to the 'bring target'
                                iGoalCell = iBringTarget;

                                // smoke puffs when picking up unit
                                int pufX = (pos_x() + getBmpWidth() / 2);
                                int pufY = (pos_y() + getBmpHeight() / 2);

                                cParticle::create(pufX, pufY, D2TM_PARTICLE_CARRYPUFF, -1, -1);

                                log("Pick up unit");
                                return;
                            }
                        } else {
                            // goal/unit not yet reached

                            if (!unitToPickupOrDrop.bPickedUp) {
                                // keep updating goal as long as unit has not been picked up yet.
                                iGoalCell = unitToPickupOrDrop.iCell;
                                iCarryTarget = unitToPickupOrDrop.iCell;
                            } else {
                                forgetAboutUnitToPickUp();
                            }
                            return;
                        }
                    } // is the unit to pick up, not yet picked up yet?
                } // has the aircraft (this) not yet picked up unit?
                else {
                    // picked up unit! yay, we are at the destination where we had to
                    // bring it... w00t
                    if (iCell == iBringTarget) {
                        TIMER_movedelay = 300; // this will make carry-all speed up slowly
                        lastDroppedOffCell = iCell; // remember this cell

                        // check if its valid for this unit...
                        if (!map.occupied(iCell, iUnitID) && isWithinMapBoundaries) {
                            // valid structure
                            cAbstractStructure *structureUnitWantsToEnter = unitToPickupOrDrop.getStructureUnitWantsToEnter();

                            if (structureUnitWantsToEnter) {
                                bool isAttemptingDeployingAtStructure = map.getCellIdStructuresLayer(iCell) == structureUnitWantsToEnter->getStructureId();

                                if (isAttemptingDeployingAtStructure) {
                                    if (structureUnitWantsToEnter->isInProcessOfBeingEnteredOrOccupiedByUnit(unitToPickupOrDrop.iID)) {
                                        // TODO: Do this with events
                                        // already became occupied, so try to find a different kind of structure
                                        int type = structureUnitWantsToEnter->getType();
                                        cAbstractStructure *alternative = findClosestAvailableStructureType(type);

                                        if (alternative) {
                                            iGoalCell = alternative->getRandomStructureCell();
                                            iBringTarget = iGoalCell;
                                            unitToPickupOrDrop.awaitBeingPickedUpToBeTransferedByCarryAllToStructure(alternative);
                                            return;
                                        } else {
                                            // !?
                                            int dropLocation = map.findNearByValidDropLocation(iCell, 3,
                                                                                               unitToPickupOrDrop.iType);
//                                            carryAll_transferUnitTo(iUnitID, dropLocation);
                                            iGoalCell = dropLocation;
                                            iBringTarget = dropLocation;
                                            return;
                                        }
                                    } // structure is being occupied or just about to be occupied
                                } // attempts to unload above structure
                            }

                            // dump it here (unload from carry-all)
                            unitToPickupOrDrop.setCell(iCell);
                            unitToPickupOrDrop.iGoalCell = iCell;
                            unitToPickupOrDrop.updateCellXAndY(); // update cellx and celly
                            map.cellSetIdForLayer(iCell, MAPID_UNITS, iUnitID);

                            unitToPickupOrDrop.iHitPoints = unitToPickupOrDrop.iTempHitPoints;
                            unitToPickupOrDrop.iTempHitPoints = -1;
                            unitToPickupOrDrop.TIMER_movewait = 0;
                            unitToPickupOrDrop.TIMER_thinkwait = 0;
                            unitToPickupOrDrop.iCarryAll = -1;

                            // match facing of carryall
                            unitToPickupOrDrop.iHeadFacing = iHeadFacing;
                            unitToPickupOrDrop.iHeadShouldFace = iHeadShouldFace;
                            unitToPickupOrDrop.iBodyFacing = iBodyFacing;
                            unitToPickupOrDrop.iBodyShouldFace = iBodyShouldFace;

                            // clear spot
                            map.clearShroud(iCell, unitToPickupOrDrop.getUnitInfo().sight, iPlayer);

                            int unitIdOfUnitThatHasBeenPickedUp = iUnitID;

                            iUnitID = -1;         // reset this
                            iTempHitPoints = -1; // reset this
                            m_transferType = eTransferType::NONE; // done

                            // make it enter the structure instantly
                            if (structureUnitWantsToEnter) {
                                if (!structureUnitWantsToEnter->isInProcessOfBeingEnteredOrOccupiedByUnit(unitIdOfUnitThatHasBeenPickedUp)) {
                                    structureUnitWantsToEnter->enterStructure(unitIdOfUnitThatHasBeenPickedUp);
                                }
                            }

                            int pufX = (pos_x() + getBmpWidth() / 2);
                            int pufY = (pos_y() + getBmpHeight() / 2);
                            cParticle::create(pufX, pufY, D2TM_PARTICLE_CARRYPUFF, -1, -1);
                        } else {
                            // find a new spot
                            updateCellXAndY();
                            iGoalCell = findNewDropLocation(unit[iUnitID].iType, iCell);
                            iBringTarget = iGoalCell;
                            return;
                        }
                    }
                }
            } else {
                m_transferType = eTransferType::NONE; // unit is not valid?
                return;
            }
        }

        // transfer is to create a new unit
        if (m_transferType == eTransferType::NEW_LEAVE ||
            m_transferType == eTransferType::NEW_STAY) {
            // bring a new unit

            if (iType == FRIGATE) {
                int iStrucId = map.getCellIdStructuresLayer(iCell);

                if (iStrucId > -1) {
                    iGoalCell = iFindCloseBorderCell(iCell);
                    m_transferType = eTransferType::DIE;

                    structure[iStrucId]->setFrame(4); // show package on this structure
                    structure[iStrucId]->setAnimating(true); // keep animating
                    ((cStarPort *) structure[iStrucId])->setFrigateDroppedPackage(true);
                } else {
                    // we don't expect this to go wrong :/
                    cAbstractStructure *starport = findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(STARPORT);

                    // find closest Starport to deliver next (Starport got mid-way destroyed)
                    if (!starport) {
                        // not found, die
                        die(true, false);
                    } else {
                        iGoalCell = starport->getCell();
                        starport->setUnitIdHeadingTowards(iID);
                    }
                }

                return; // override for frigates
            }

            bool canDeployAtCell = map.occupied(iCell, iID) == false;

            if (iNewUnitType > -1) {
                 canDeployAtCell = map.canDeployUnitAtCell(iCell, iID);
            }

            // first check if this cell is clear
            if (canDeployAtCell && isWithinMapBoundaries) {
                // drop unit
                if (iNewUnitType > -1) {
                    int id = UNIT_CREATE(iCell, iNewUnitType, iPlayer, true, isReinforcement);

                    if (id > -1) {
                        map.cellSetIdForLayer(iCell, MAPID_UNITS, id);
                    }
                }

                // now make sure this carry-all will not be drawn as having a unit:
                iNewUnitType = -1;

                // depending on transfertype...
                if (m_transferType == eTransferType::NEW_LEAVE) {
                    m_transferType = eTransferType::DIE;

                    // find a new border cell close to us... to die
                    iGoalCell = iFindCloseBorderCell(iCell);
                    return;
                } else if (m_transferType == eTransferType::NEW_STAY) {
                    // reset transfertype:
                    m_transferType = eTransferType::NONE;
                    return;
                }

            } else {
                // find a new spot for delivery
                updateCellXAndY();
                iGoalCell = findNewDropLocation(iNewUnitType, iCell);
                return;
            }
        }

        // move randomly
        int cell = lastDroppedOffCell;
        if (cell < 0) {
            cell = getPlayer()->getFocusCell();
        }
        iGoalCell = map.getRandomCellFromWithRandomDistance(cell, 12);
        return;
    }

    // goal cell == next cell (move straight to it)
    TIMER_move++;

    // now move
    int goalCellX = map.getCellX(iGoalCell);
    int goalCellY = map.getCellY(iGoalCell);

    // use this when picking something up
    if (iUnitID > -1 || (m_transferType != eTransferType::DIE && m_transferType != eTransferType::NONE)) {
        int iLength = ABS_length(iCellX, iCellY, goalCellX, goalCellY);

        if (iType != FRIGATE) {
            // 'sand dust' when nearing target and hovering over sandy terrain
            // 6 * 6 = 36
            int dist = 4;
            int maxSlowDown = 36;
            float slowDownStep = maxSlowDown / dist;
            if (iLength < dist) {
                if (rnd(100) < 5) {
                    int cellType = map.getCellType(iCell);
                    if (cellType == TERRAIN_SAND ||
                        cellType == TERRAIN_SPICE ||
                        cellType == TERRAIN_HILL ||
                        cellType == TERRAIN_SPICEHILL) {
                        int pufX = (pos_x() + getBmpWidth() / 2);
                        int pufY = (pos_y() + getBmpHeight() / 2);
                        cParticle::create(pufX, pufY, D2TM_PARTICLE_CARRYPUFF, -1, -1);
                    }
                }
                TIMER_movedelay = (dist - iLength) * (dist * slowDownStep);
            }
        } else {
            int dist = 6;
            if (iLength < dist) {
                TIMER_movedelay = (dist - iLength) * (dist * 6);
            }
        }
    }

    int iSlowDown = 0;
    if (TIMER_movedelay > 0) {
        iSlowDown = (TIMER_movedelay/20);
        TIMER_movedelay--;
    }

    cPlayerDifficultySettings *difficultySettings = getPlayer()->getDifficultySettings();

    if (TIMER_move < (difficultySettings->getMoveSpeed(iType, iSlowDown))) {
        return;
    }

    TIMER_move = 0;

    // air units 'turn around' facing the ideal angle. But they can't turn around swiftly, only when very close.
    int d = fDegrees(iCellX, iCellY, goalCellX, goalCellY);
    int idealAngle = face_angle(d);
    int f = face_angle(d); // get the angle
    float angle = 0;

    iBodyShouldFace = idealAngle;
    if (iBodyFacing != iBodyShouldFace) {
        // not ideal angle, the aircraft flies straight ahead which it is facing.
        // since we don't have a velocity vector, we do it like this for now:
        int nextX = iCellX;
        int nextY = iCellY;

        switch (iBodyFacing) {
            case FACE_UP:
                nextY--;
                break;
            case FACE_UPRIGHT:
                nextY--;
                nextX++;
                break;
            case FACE_RIGHT:
                nextX++;
                break;
            case FACE_DOWNRIGHT:
                nextY++;
                nextX++;
                break;
            case FACE_DOWN:
                nextY++;
                break;
            case FACE_DOWNLEFT:
                nextY++;
                nextX--;
                break;
            case FACE_LEFT:
                nextX--;
                break;
            case FACE_UPLEFT:
                nextY--;
                nextX--;
                break;
        }
        angle = fRadians(iCellX, iCellY, nextX, nextY);
    } else {
        // using ideal angle, just fly straight towards goal
        angle = fRadians(iCellX, iCellY, goalCellX, goalCellY);
    }

    iHeadFacing = f;

    map.cellResetIdFromLayer(iCell, MAPID_AIR);

//    int movespeed = getUnitInfo().speed;
    int movespeed = 2; // 2 pixels, the actual 'speed' is done by the delay above using TIMER_move! :/
    float newPosX = posX + cos(angle) * movespeed;
    float newPosY = posY + sin(angle) * movespeed;
    setPosX(newPosX);
    setPosY(newPosY);

    // Cell of unit is determined by its center
    iCell = mapCamera->getCellFromAbsolutePosition(pos_x_centered(), pos_y_centered());

    updateCellXAndY();
    map.cellSetIdForLayer(iCell, MAPID_AIR, iID);
}

void cUnit::setPosX(float newVal) {
    float diff = newVal - posX;
    posX = newVal;
    if (boundParticleId > -1) {
        cParticle &pParticle = particle[boundParticleId];
        pParticle.addPosX(diff);
    }
}

void cUnit::setPosY(float newVal) {
    float diff = newVal - posY;
    posY = newVal;
    if (boundParticleId > -1) {
        cParticle &pParticle = particle[boundParticleId];
        pParticle.addPosY(diff);
    }
}

void cUnit::forgetAboutUnitToPickUp() {// forget about this
    iGoalCell = iCell;
    m_transferType = eTransferType::NONE;
    iUnitID = -1;
}

cUnit &cUnit::getUnitToPickupOrDrop() const {
    assert(iUnitID > -1 && "cUnit::getUnitToPickupOrDrop() called for invalid iUnitIDWithinStructure!");
    return unit[iUnitID];
}

cAbstractStructure *cUnit::getStructureUnitWantsToEnter() const {
    cAbstractStructure *structureUnitWantsToEnter = nullptr;
    if (iStructureID > -1) {
        structureUnitWantsToEnter = structure[iStructureID];
        if (structureUnitWantsToEnter && !structureUnitWantsToEnter->isValid()) {
            structureUnitWantsToEnter = nullptr;
        }
    }
    return structureUnitWantsToEnter;
}

int cUnit::findNewDropLocation(int unitTypeToDrop, int cell) const {
    int dropLocation = map.findNearByValidDropLocation(cell, 4, unitTypeToDrop);
    if (dropLocation < 0) {
        dropLocation = map.findNearByValidDropLocation(cell, 8, unitTypeToDrop);
    }
    if (dropLocation < 0) {
        dropLocation = map.findNearByValidDropLocation(cell, 16, unitTypeToDrop);
    }
    return dropLocation;
}

// Carryall-order
//
// Purpose:
// Order a carryall to pickup a unit, or send a new unit (depending on transferType)
//
void cUnit::carryall_order(int iuID, eTransferType transferType, int iBring, int iTpe) {
    if (m_transferType != eTransferType::NONE)
        return; // we cannot do multiple things at a time!!

    if (transferType == eTransferType::NEW_STAY || transferType == eTransferType::NEW_LEAVE) {
        // bring a new unit, depending on the transferType the carryall who brings this will be
        // removed after he brought the unit...

        // when iTranfer is 0 or 2, the unit is just created by REINFORCE() and this function
        // sets the target and such.

        m_transferType = transferType;

        // Go to this:
        iGoalCell = iBring;
        iBringTarget = iGoalCell;

        // carry is not valid
        iCarryTarget = -1;

        // unit we carry is none
        iUnitID = -1;

        // the unit type...
        iNewUnitType = iTpe;

        bPickedUp = false;
        // DONE!
    } else if (transferType == eTransferType::PICKUP && iuID > -1) {

        // the carryall must pickup the unit, and then bring it to the iBring stuff
        cUnit &pUnit = unit[iuID];
        if (pUnit.isValid()) {
            m_transferType = transferType;

            iGoalCell = pUnit.iCell; // first go to the target to pick it up
            iCarryTarget = pUnit.iCell; // same here...

            iNewUnitType = -1;

            // where to bring it to
            iBringTarget = iBring; // bring

            bPickedUp = false;

            // which unit do we intent to pick up?
            iUnitID = iuID;
            // let the unit know we intent to pick it up
            pUnit.willBePickedUpBy = iID;
        }
    }
}

void cUnit::shoot(int iTargetCell) {
    // particles are rendered at the center, so do it here as well
    int iShootX = pos_x() + (getBmpWidth() / 2);
    int iShootY = pos_y() + (getBmpHeight() / 2);
    int bmp_head = convert_angle(iHeadFacing);

    // TODO: add this in sUnitInfo
    if (iType == TANK) {
        cParticle::create(iShootX, iShootY, D2TM_PARTICLE_TANKSHOOT, -1, bmp_head);
    } else if (iType == SIEGETANK) {
        cParticle::create(iShootX, iShootY, D2TM_PARTICLE_SIEGESHOOT, -1, bmp_head);
    }

    s_UnitInfo &unitInfo = getUnitInfo();
    int bulletType = unitInfo.bulletType;
    // if secondary fire is configured properly
    if (unitInfo.fireSecondaryWithinRange > -1 && unitInfo.bulletTypeSecondary > -1) {
        if (map.distance(iCell, iTargetCell) <= unitInfo.fireSecondaryWithinRange) {
            bulletType = unitInfo.bulletTypeSecondary;
        }
    }
    
    
    if (bulletType < 0) return; // no bullet type to spawn

    int iBull = create_bullet(bulletType, iCell, iTargetCell, iID, -1);

    cUnit * attackUnit = nullptr;
    if (iAttackUnit > -1) {
        attackUnit = &unit[iAttackUnit];
        if (attackUnit && !attackUnit->isValid()) {
            // allowing homing bullets towards air units from the ground
            if (iBull > -1 && attackUnit->isAirbornUnit()) {
                bullet[iBull].iHoming = iAttackUnit;
                bullet[iBull].TIMER_homing = 200;
            }
        }
    }
}

int cUnit::getNextCellToMoveTo() {
    if (isAirbornUnit()) {
        if (iGoalCell == iCell) {
            return iCell;
        }

        return iGoalCell; // return the goal
    }

    if (iPathIndex < 0) {
        return -1;
    }

    // not valid OR same location
    int nextCell = iPath[iPathIndex];
    if (nextCell < 0) {
        log("No valid iPATH[pathindex], nextCell is < 0");
        return -1;
    }

    // now, we are sure it will be another location
    return nextCell;
}

// ouch, who shot me?
void cUnit::think_hit(int iShotUnit, int iShotStructure) {
    if (isDead()) {
        // I've died , so don't bother
        return;
    }

    if (isSaboteur()) {
        // ignore being shot?
        return;
    }

    if (iShotUnit > -1) {
        cUnit &unitWhoShotMe = unit[iShotUnit];
        if (unitWhoShotMe.getPlayer()->isSameTeamAs(getPlayer())) return; // ignore friendly fire

        bool unitWhoShotMeIsInfantry = unitWhoShotMe.isInfantryUnit();
        bool unitWhoShotMeIsAirborn = unitWhoShotMe.isAirbornUnit();

        // only act when we are doing 'nothing'...
        if (m_action == eActionType::GUARD) {
            // only auto attack back when it is not an airborn unit
            // note: guard state already takes care of scanning for air units and attacking them
            if (!unitWhoShotMeIsAirborn) {
                attackUnit(iShotUnit);
            }
        }

        if (!getPlayer()->isHuman()) {
            if (isHarvester()) {
                if (unit[iShotUnit].isInfantryUnit() && !isMovingBetweenCells()) {
                    // this harvester will try to run over the infantry that attacks it
                    move_to(unit[iShotUnit].iCell);
                } else {
                    // under attack, retreat to base? find nearby units to help out?
                }

            } else {
                if (m_action != eActionType::ATTACK) {
                    if (canSquishInfantry() && unitWhoShotMeIsInfantry) {
                        // AI tries to run over infantry units that attack it
                        move_to(unit[iShotUnit].iCell);
                    } else {
                        if (!unitWhoShotMeIsAirborn) {
                            if (isSandworm()) {
                                if (map.isCellPassableForWorm(unitWhoShotMe.iCell)) {
                                    attackUnit(iShotUnit);
                                } else {
                                    // tough luck :/
                                }
                            } else {
                                attackUnit(iShotUnit);
                            }
                        }
                    }
                } else {
                    // we are attacking, but when target is very far away (out of range?) then we should not attack that but defend
                    // ourselves
                    int iDestCell = iAttackCell;

                    if (iDestCell < 0) {
                        if (iAttackUnit > -1)
                            iDestCell = unit[iAttackUnit].iCell;

                        if (iAttackStructure > -1) {
                            cAbstractStructure *pStructure = structure[iAttackStructure];
                            // it can become null, so check!
                            if (pStructure && pStructure->isValid()) {
                                iDestCell = pStructure->getCell();
                            }
                        }

                        if (ABS_length(iCellX, iCellY, map.getCellX(iDestCell), map.getCellY(iDestCell)) <
                            sUnitInfo[iType].range) {
                            // within range, don't move (just prepare retaliation fire)
                        } else {
                            // out of range unit, attack it
                            if (canSquishInfantry() && unitWhoShotMeIsInfantry) {
                                // AI tries to run over infantry units that attack it
                                move_to(unit[iShotUnit].iCell);
                            } else {
                                // else simply shoot it
                                if (!unitWhoShotMeIsAirborn) {
                                    attackUnit(iShotUnit);
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (iShotStructure > -1) {
      // TODO?
    }

    // for infantry , infantry turns into soldier and troopers into trooper when on 50% damage.
    if (isInfantryUnit()) {
        if (iType == INFANTRY || iType == TROOPERS) {
            // turn into soldier or trooper when on 50% health
            if (iHitPoints <= (sUnitInfo[iType].hp / 3)) {
                // leave 2 dead bodies (of 3 ;))

                // turn into single one
                if (iType == INFANTRY)
                    iType = SOLDIER;

                if (iType == TROOPERS)
                    iType = TROOPER;

                iHitPoints = sUnitInfo[iType].hp;

                int half = 16;
                int iDieX = pos_x() + half;
                int iDieY = pos_y() + half;

                cParticle::create(iDieX, iDieY, D2TM_PARTICLE_DEADINF01, iPlayer, -1);

                play_sound_id_with_distance(SOUND_DIE01 + rnd(5),
                                            distanceBetweenCellAndCenterOfScreen(iCell));

            }
        }
    }
}

void cUnit::log(const std::string& txt) const {
    // logs unit stuff, but gives unit information
    players[iPlayer].log(fmt::format("[UNIT[{}]: type = {}(={}), iCell = {}, iGoalCell = {}] '{}'",
                                     iID, iType, sUnitInfo[iType].name, iCell, iGoalCell, txt));
}

/**
 * Thinking, called every 100ms
 */
void cUnit::think_attack() {
    updateCellXAndY();

    if (isSandworm()) {
        think_attack_sandworm();
        return;
    }

    cUnit * attackUnit = nullptr;
    if (iAttackUnit > -1) {
        attackUnit = &unit[iAttackUnit];

        // should be impossible
        if (!attackUnit) {
            actionGuard();
            return;
        }

        // no longer valid, or dead
        if (!attackUnit->isValid() || attackUnit->isDead()) {
            actionGuard();
            return;
        }

        iGoalCell = attackUnit->iCell;
    }

    cAbstractStructure *pStructure = nullptr;
    if (iAttackStructure > -1) {
        pStructure = structure[iAttackStructure];
        if (pStructure && pStructure->isValid()) {
            iGoalCell = pStructure->getCell();
        } else {
            iAttackUnit = -1;
            iAttackStructure = -1;
            iGoalCell = iCell;
            m_action = eActionType::GUARD;
            return;
        }

        if (pStructure->isDead()) {
            actionGuard();
            log("Destroyed structure target");
            return;
        }
    }

    if (iAttackCell > -1) {
        iGoalCell = iAttackCell;

        bool isBloomOrWallTerrain =
                map.getCellType(iAttackCell) == TERRAIN_BLOOM || map.getCellType(iAttackCell) == TERRAIN_WALL;

        if (isBloomOrWallTerrain) {
            // stop attacking a spice bloom or a wall when it got destroyed
            if (map.getCellHealth(iAttackCell) < 0) {
                actionGuard();
                return;
            }
        } else {
            if (!getPlayer()->isHuman()) {
                // non human units should stop attacking the ground
                actionGuard();
                return;
            }
        }
    }

    // Distance check
    int distance = map.distance(iCell, iGoalCell);

    if (!isAirbornUnit()) {
        if (!isMovingBetweenCells()) {
            if (distance <= getRange()) {
                setAngleTowardsTargetAndFireBullets(distance);
            } else { // not within distance
                startChasingTarget();
            }
        } else {
            if (distance > getRange()) {
                // atleast don't try to attack it
                startChasingTarget();
            }
        }
    } else {
        // AIRBORN UNITS ATTACK THINKING
        int minDistance = 2;

        if (distance > minDistance && distance <= getRange()) {
            // when this function returns true, it is done firing bullets
            if (setAngleTowardsTargetAndFireBullets(distance)) {
                int randomCellFrom = map.getRandomCellFrom(iGoalCell, 16);
                int rx = map.getCellX(randomCellFrom);
                int ry = map.getCellY(randomCellFrom);

                iAttackUnit = -1;
                iAttackStructure = -1;
                m_action = eActionType::MOVE;
                iGoalCell = map.getCellWithMapDimensions(rx, ry);
            }
        } else {
            // stop attacking, move instead?
            m_action = eActionType::MOVE;
            iAttackUnit = -1;
            iAttackStructure = -1;
        }
    }
}

void cUnit::think_attack_sandworm() {
    if (iAttackUnit < 0) {
        // no attack unit
        actionGuard();
        return;
    }

    cUnit * attackUnit = &unit[iAttackUnit];

    // should be impossible
    if (!attackUnit) {
        actionGuard();
        return;
    }

    // no longer valid, or dead
    if (!attackUnit->isValid() || attackUnit->isDead()) {
        actionGuard();
        return;
    }

    // update iGoalCell with where the attacking unit is (chase)
    iGoalCell = attackUnit->iCell;
    if (iGoalCell == iCell) {
        attackUnit->die(false, false);
        unitsEaten++;
        long x = pos_x_centered();
        long y = pos_y_centered();
        cParticle::create(x, y, D2TM_PARTICLE_WORMEAT, -1, -1);
        play_sound_id_with_distance(SOUND_WORM, distanceBetweenCellAndCenterOfScreen(iCell));
        actionGuard();
        TIMER_movewait = (1000/5) * 4; // wait for 4 seconds before moving again
        TIMER_guard = (1000/5) * 4; // timer guard works other way around..

        if (unitsEaten >= getUnitInfo().appetite) {
            // let worm die (and respawn later)
            iHitPoints = getUnitInfo().dieWhenLowerThanHP;
            takeDamage(1); // get below the thresh-hold to die/vanish
        } else {
            TIMER_guard = (1000/5) * ((5*unitsEaten) + rnd((20*unitsEaten)));
        }

        if (DEBUGGING) {
            logbook(fmt::format("think_attack_sandworm() -> eaten unit. Units eaten {}, TIMER_guard {}",
                                unitsEaten, TIMER_guard));
        }
        return;
    }

    if (attackUnit->isIdle() && !map.isCellPassableForWorm(attackUnit->iCell)) {
        // forget about unit that is not reachable
        actionGuard();
        return;
    }

    startChasingTarget();
    return;
}

void cUnit::actionGuard() {
    m_action = eActionType::GUARD;
    iAttackUnit = -1;
    iAttackCell = -1;
    iAttackStructure =-1;
}

int cUnit::getFaceAngleToCell(int cell) const {
    int d = fDegrees(iCellX, iCellY, map.getCellX(cell), map.getCellY(cell));
    return face_angle(d); // get the angle
}

void cUnit::startChasingTarget() {
    if (iAttackStructure > -1) {
        m_action = eActionType::CHASE;
        // a structure does not move, so don't need to re-calculate path?
//        forgetAboutCurrentPathAndPrepareToCreateNewOne();
    } else if (iAttackUnit > -1) {
        cUnit * attackUnit = &unit[iAttackUnit];
        // chase unit, but only when ground unit
        if (!attackUnit->isSandworm() && !attackUnit->isAirbornUnit()) {
            m_action = eActionType::CHASE;
            // only think of new path when our target moved
            if (attackUnit->getCell() != iGoalCell) {
                forgetAboutCurrentPathAndPrepareToCreateNewOne();
            }
        } else {
            // do not chase sandworms or other air units, very ... inconvenient
            m_action = eActionType::GUARD;
            iGoalCell = iCell;
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }
    } else if (iAttackCell > -1) {
        m_action = eActionType::CHASE;
    }
}

bool cUnit::setAngleTowardsTargetAndFireBullets(int distance) {
    s_UnitInfo &unitType = getUnitInfo();
    // in range , fire and such

    // Facing
    int angle = getFaceAngleToCell(iGoalCell);

    iBodyShouldFace = angle;
    iHeadShouldFace = angle;

    if (iBodyShouldFace == iBodyFacing && iHeadShouldFace == iHeadFacing) {

        TIMER_attack++;
        if (TIMER_attack >= unitType.attack_frequency) {
            int shootCell = iGoalCell;

            if (iType == LAUNCHER || iType == DEVIATOR) {
                if (distance < unitType.range) {
                    int dx = map.getCellX(shootCell);
                    int dy = map.getCellY(shootCell);

                    int dif = (unitType.range - distance) / 2;

                    dx -= (dif);
                    dx += rnd(dif * 2);

                    dy -= (dif);
                    dy += rnd(dif * 2);

                    shootCell = map.getCellWithMapDimensions(dx, dy);
                }
            }

            // first bullet
            if (TIMER_attack == unitType.attack_frequency) {
                shoot(shootCell);
            }

            if (unitType.fireTwice == false) {
                TIMER_attack = 0;
                return true;
            } else {
                int secondShotTimeLimit = unitType.attack_frequency + (unitType.attack_frequency / 4);
                if (TIMER_attack > secondShotTimeLimit) {
                    shoot(shootCell);
                    TIMER_attack = 0;
                    return true;
                }
            }
        }
    }
    return false;
}

int cUnit::getRange() const {
    return getUnitInfo().range;
}

int cUnit::getSight() const {
    return getUnitInfo().sight;
}

s_UnitInfo &cUnit::getUnitInfo() const {
    return sUnitInfo[iType];
}

// thinking about movement (called every 5 ms)
// called when ACTION_CHASE or ACTION_MOVE
void cUnit::thinkFast_move() {
    if (!isValid()) {
        assert(false && "Expected to have a valid unit calling thinkFast_move()");
    }

    // this is about non-aircraft only, so bail for aircraft units
    if (isAirbornUnit()) {
        return;
    }

    // it is a unit that is temporarily not available on the map (picked up by carry-all for instance, or
    // within a structure)
    if (iTempHitPoints > -1) {
        return;
    }

    if (TIMER_movewait > 0) {
        TIMER_movewait--;
        return;
    }

    // when there is a valid goal cell (differs), then we go further
    if (iGoalCell == iCell) {
        m_action = eActionType::GUARD; // do nothing
        forgetAboutCurrentPathAndPrepareToCreateNewOne();
        return;
    }

    // not moving between cells, check if there is a new cell to move to
    if (!isMovingBetweenCells()) {
        iNextCell = getNextCellToMoveTo();

        // no next cell determined
        if (iNextCell < 0) {
            forgetAboutCurrentPathAndPrepareToCreateNewOne(0);

            // when we do have a different goal, we should get a path:
            if (iGoalCell != iCell) {
                int iResult = CREATE_PATH(iID, 0); // do not take units into account yet
                log(fmt::format("Create path ... result = {}", iResult));

                // On fail:
                if (iResult < 0) {
                    // simply failed
                    if (iResult == -1) {
                        // Check why, is our goal cell occupied?
                        int uID = map.getCellIdUnitLayer(iGoalCell);
                        int sID = map.getCellIdStructuresLayer(iGoalCell);

                        // Other unit is on goal cell, do something about it.

                        // uh oh
                        if (uID > -1 && uID != iID) {
                            // occupied, not by self
                            // find a goal cell near to it
                            int iNewGoal = RETURN_CLOSE_GOAL(iGoalCell, iCell, iID);

                            if (iNewGoal == iGoalCell) {
                                // same goal, cant find new, stop
                                iGoalCell = iCell;
                                log("Could not find alternative goal");
                                iPathIndex = -1;
                                iPathFails = 0;
                                return;

                            } else {
                                iGoalCell = iNewGoal;
                                TIMER_movewait = rnd(20);
                                log("Found alternative goal");
                                return;
                            }
                        } else if (sID > -1 && iStructureID > -1 && iStructureID != sID) {
                            log("Want to enter structure, yet ID's do not match");
                            log("Resetting structure id and such to redo what i was doing?");
                            iStructureID = -1;
                            iGoalCell = iCell;
                            iPathIndex = -1;
                            iPathFails = 0;
                        } else {
                            log("Something else blocks path, but goal itself is not occupied.");
                            iGoalCell = iCell;
                            iPathIndex = -1;
                            iPathFails = 0;
                            iStructureID = -1;

                            // random move around
                            int iF = UNIT_FREE_AROUND_MOVE(iID);

                            if (iF > -1) {
                                move_to(iF);
                            }
                        }
                    } else if (iResult == -2) {
                        // we have a problem here, units/stuff around us blocks the unit
                        log("Units surround me, what to do?");
                    } else if (iResult == -3) {
                        log("I just failed, ugh");
                    }

                    // We failed
                    iPathFails++;

                    if (iPathFails > 2) {
                        // notify that we can't create path, we should do something about this?
                        // at this point we can still ready unit state about path goal, etc.
                        s_GameEvent event {
                                .eventType = eGameEventType::GAME_EVENT_CANNOT_CREATE_PATH,
                                .entityType = eBuildType::UNIT,
                                .entityID = iID,
                                .player = getPlayer(),
                                .entitySpecificType = iType,
                                .atCell = iCell,
                                .isReinforce = isReinforcement
                        };

                        game.onNotifyGameEvent(event);

                        // stop trying - forget about path stuff
                        iGoalCell = iCell;
                        iPathFails = 0;
                        iPathIndex = -1;
                        if (TIMER_movewait <= 0) {
                            TIMER_movewait = 100;
                        }
                    }
                } else {
                    //log("SUCCES");
                }
            } // must go somewhere (has a goal)
            return;
        } // didn't get valid nextCell
    }

    // Update the 'should' facing (ideal facing) of body and head.
    iBodyShouldFace = getFaceAngleToCell(iNextCell);
    iHeadShouldFace = getFaceAngleToCell(iGoalCell);

    // check
    bool bOccupied = false;

    int idOfUnitAtNextCell = map.getCellIdUnitLayer(iNextCell);

    // Cell is occupied, not by self, so it is occupied...
    if (idOfUnitAtNextCell > -1 &&
        idOfUnitAtNextCell != iID) {
        // get it
        int iUID = idOfUnitAtNextCell;

        bOccupied = true;
        // unless... it is an enemy infantry unit and I can squish stuff
        if (canSquishInfantry()) {
            cUnit &unitAtCell = unit[iUID];
            if (unitAtCell.isValid() &&
                unitAtCell.iPlayer != iPlayer && // enemy player?
                unitAtCell.isInfantryUnit() // squishable?
                    ) {
                bOccupied = false;
            }
        }
    }

    // structure is NOT matching our structure ID, then its blocking us
    int idOfStructureAtNextCell = map.getCellIdStructuresLayer(iNextCell);

    if (idOfStructureAtNextCell > -1) {
        if (iStructureID < 0) {
            // not intended to go into a structure, so it blocks
            bOccupied = true;
        } else if (iStructureID > -1) {
            if (iStructureID == idOfStructureAtNextCell) {
                // it is the structure this unit intents to go to...
                cAbstractStructure *pStructure = structure[iStructureID];
                // we may enter, only if its empty
                if (pStructure && pStructure->isValid()) {
                    // repair/spice unloading structures can only 'contain' ONE unit. So if it is occupied, find another.
                    if (intent == eUnitActionIntent::INTENT_UNLOAD_SPICE ||
                        intent == eUnitActionIntent::INTENT_REPAIR) {
                        bool findAlternativeStructure = false;

                        // another unit is entering this structure (or it is occupied)
                        if (pStructure->isInProcessOfBeingEnteredOrOccupiedByUnit(iID)) {
                            findAlternativeStructure = true;
                            bOccupied = true;
                        } else {
                            // start entering the structure at next cell
                            pStructure->startEnteringStructure(iID);
                            bSelected = false; // deselect unit
                        }

                        // TODO: this should be done via events (EVENT_CAPTURED? and then make unit choose different structure)
                        if (!pStructure->getPlayer()->isSameTeamAs(getPlayer())) { // no longer of my team
                            findAlternativeStructure = true;
                            bOccupied = true;
                        } else {  // structure is occupied by unit
                            // not occupied, continue!
                        }

                        if (findAlternativeStructure) {
                            // find alternative structure type nearby
                            cAbstractStructure *candidate = findBestStructureCandidateToHeadTo(pStructure->getType());

                            if (candidate && candidate->getStructureId() != iStructureID) {
                                // founder an alternative
                                move_to_enter_structure(candidate, intent);
                                return; // bail, else the logic below will kick of (ugh, bad code)
                            } else {
                                // wait, maybe we can enter later!
                                TIMER_movewait = 100; // we wait
                                return; // bail, else the logic below will kick of (ugh, bad code)
                            }
                        }
                    } else if (intent == eUnitActionIntent::INTENT_CAPTURE) {
                        if (pStructure->getPlayer()->isSameTeamAs(getPlayer())) {
                            iStructureID = -1;
                            iGoalCell = iCell;
                            iNextCell = iCell;
                            TIMER_movewait = 100; // we wait
                            bOccupied = true; // obviously - but will do nothing :S
                            return; // bail, else the logic below will kick of (ugh, bad code)
                        }
                    } else {
                        bOccupied = true;
                    }
                } else {
                    // structure is no longer valid, what now?
                    // .. not occupied?
                    bOccupied = true;
                }
            } else {
                // wrong structure, so blocks
                bOccupied = true;
            }
        }
    }

    int cellTypeAtNextCell = map.getCellType(iNextCell);
    if (!isInfantryUnit()) {
        if (cellTypeAtNextCell == TERRAIN_MOUNTAIN) {
            bOccupied = true;
        }
    }

    if (cellTypeAtNextCell == TERRAIN_WALL) {
        bOccupied = true;
    }

    // TODO: this "thinkFast_move" thing should be abstracted somewhere, so that "occupied" becomes an abstraction
    // as well and thus this kind of hack can be prevented.
    if (isSandworm()) {
        bOccupied = false;
    }

    if (bOccupied) {
        if (iNextCell == iGoalCell) {
            // it is our goal cell, close enough
            iGoalCell = iCell;
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
            return;
        } else if (idOfStructureAtNextCell > -1) {
            // blocked by structure
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        } else if (idOfUnitAtNextCell > -1) {
            // From here, assume a unit is standing in our way. First check if this unit will
            // move. If so, we wait until it has moved.
            int uID = idOfUnitAtNextCell;

            // Wait when the obstacle is moving, perhaps it will clear our way
            cUnit &unitOccupyingNextCell = unit[uID];
            if (unitOccupyingNextCell.isValid() &&
                unitOccupyingNextCell.TIMER_movewait <= 0 &&
                unitOccupyingNextCell.iGoalCell != unitOccupyingNextCell.iCell) {
                // this unit is also moving, so wait for it to move
                TIMER_movewait = 50;
                return;
            } else {
                // this unit has no intention to move away, so create path around it.
                forgetAboutCurrentPathAndPrepareToCreateNewOne();
            }
        } else {
            // all other cases (ie terrain blocking, etc)
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }
        return;
    }

    updateCellXAndY();

    if (iBodyShouldFace == iBodyFacing) {
        eUnitMoveToCellResult result = moveToNextCellLogic();

        if (result == eUnitMoveToCellResult::MOVERESULT_AT_GOALCELL ||
            result == eUnitMoveToCellResult::MOVERESULT_AT_CELL) {
            // not occupied cell;
            int idOfStructureAtCurrentCell = map.getCellIdStructuresLayer(iCell);

            // we wanted to enter this structure
            if (iStructureID > -1 &&
                idOfStructureAtCurrentCell > -1 && idOfStructureAtCurrentCell == iStructureID) {
                cAbstractStructure *pStructure = structure[iStructureID];
                if (pStructure && pStructure->isValid()) {
                    if (intent == eUnitActionIntent::INTENT_REPAIR ||
                        intent == eUnitActionIntent::INTENT_UNLOAD_SPICE) {
                        // when this structure is not occupied
                        if (!pStructure->isInProcessOfBeingEnteredOrOccupiedByUnit(iID)) {
                            pStructure->enterStructure(iID);
                            return;
                        } // enter..
                        else {
                            // looks like it is occupied, find alternative
                            if (intent == eUnitActionIntent::INTENT_REPAIR) {
                                // find alternative repair pad
                            } else if (intent == eUnitActionIntent::INTENT_UNLOAD_SPICE) {
                                // find alternative
                            }
                        }
                    } else if (intent == eUnitActionIntent::INTENT_CAPTURE || intent == eUnitActionIntent::INTENT_MOVE) {
                        if (isSaboteur()) {
                            // the unit will die and inflict damage
                            pStructure->damage(getUnitInfo().damageOnEnterStructure);
                            die(true, false);
                        } else {
                            if (pStructure->getHitPoints() < pStructure->getCaptureHP()) {
                                // make structure switch sides
                                pStructure->getsCapturedBy(getPlayer());
                                die(false, false);
                            } else {
                                // the unit will die and inflict damage
                                die(true, false);
                                pStructure->damage(getUnitInfo().damageOnEnterStructure);
                            }
                        }
                        return; // unit is dead, no need to go further
                    }
                }
            }
        }
    }
}

/**
 * This moves a unit to a new cell, (pixel by pixel).
 */
eUnitMoveToCellResult cUnit::moveToNextCellLogic() {
    // When we should move:
    int bToLeft = -1;         // 0 = go left, 1 = go right
    int bToDown = -1;         // 0 = go down, 1 = go up

    int iNextX = map.getCellX(iNextCell);
    int iNextY = map.getCellY(iNextCell);

    // Compare X, Y coordinates
    if (iNextX < iCellX)
        bToLeft = 0; // we head to the left

    if (iNextX > iCellX)
        bToLeft = 1; // we head to the right

    // we go up
    if (iNextY < iCellY)
        bToDown = 1;

    // we go down
    if (iNextY > iCellY)
        bToDown = 0;

    // done, since we already have the other stuff set
    TIMER_move++;


    // Influenced by the terrain type
    int cellType = map.getCellType(iCell);
    int iSlowDown = map.getCellSlowDown(iCell);

    cPlayerDifficultySettings *difficultySettings = players[iPlayer].getDifficultySettings();
    if (TIMER_move < ((difficultySettings->getMoveSpeed(iType, iSlowDown)))) {
        return eUnitMoveToCellResult::MOVERESULT_SLOWDOWN; // get out
    }

    TIMER_move = 0; // reset to 0

    // from here on, set the map id, so no other unit can take its place
    if (!isSandworm()) {
        // note, no AIRBORN here (27/03/2021 - I guess this is because this method is not called by thinkFast_move_airUnit())
        map.cellSetIdForLayer(iNextCell, MAPID_UNITS, iID);
    } else {
        map.cellSetIdForLayer(iNextCell, MAPID_WORMS, iID);

        // add a worm trail behind worm randomly for now (just not every frame, or else this spams a great
        // deal of particles overlapping eachother.
        TIMER_wormtrail++;
        if (TIMER_wormtrail > 4) {
            long x = pos_x_centered();
            long y = pos_y_centered();
            cParticle::create(x, y, D2TM_PARTICLE_WORMTRAIL, -1, -1);
            TIMER_wormtrail = 0;
        }
    }

    // 100% on cell, no offset
    if (!isMovingBetweenCells()) {
        int half = 16;
        int iParX = pos_x() + half;
        int iParY = pos_y() + half;

        // add tracks in the sand
        if (cellType != TERRAIN_ROCK &&
            cellType != TERRAIN_MOUNTAIN &&
            cellType != TERRAIN_WALL &&
            cellType != TERRAIN_SLAB &&
            !isInfantryUnit() &&
            !isSandworm()) {

            // horizontal when only going horizontal
            if (bToLeft > -1 && bToDown < 0) {
                cParticle::create(iParX, iParY, D2TM_PARTICLE_TRACK_HOR, -1, -1);
            }

            // vertical, when only going vertical
            if (bToDown > -1 && bToLeft < 0) {
                cParticle::create(iParX, iParY, D2TM_PARTICLE_TRACK_VER, -1, -1);
            }

            // diagonal when going both ways
            if (bToDown > -1 && bToLeft > -1) {
                if (bToDown == 0) {
                    // going up
                    if (bToLeft == 1) {
                        cParticle::create(iParX, iParY, D2TM_PARTICLE_TRACK_DIA, -1, -1);
                    }
                    else {
                        cParticle::create(iParX, iParY, D2TM_PARTICLE_TRACK_DIA2, -1, -1);
                    }

                } else {
                    if (bToLeft == 0) {
                        cParticle::create(iParX, iParY, D2TM_PARTICLE_TRACK_DIA, -1, -1);
                    }
                    else {
                        cParticle::create(iParX, iParY, D2TM_PARTICLE_TRACK_DIA2, -1, -1);
                    }
                }

            }
        }
    }

    // movement in pixels
    if (bToLeft == 0)
        setPosX(posX-1);
    else if (bToLeft == 1)
        setPosX(posX+1);

    if (bToDown == 0)
        setPosY(posY+1);
    else if (bToDown == 1)
        setPosY(posY-1);

    // When moving, infantry has some animation
    if (isInfantryUnit()) {
        TIMER_frame++;

        if (TIMER_frame > 3) {

            iFrame++;
            if (iFrame > 3)
                iFrame = 0;

            TIMER_frame = 0;
        }
    }

    // movement is done, determine what to do with new state.
    if (!isMovingBetweenCells()) {
        // when we are chasing, we now set on attack...
        if (m_action == eActionType::CHASE) {
            // next time we think, will be checking for distance, etc
            m_action = eActionType::ATTACK;
            forgetAboutCurrentPathAndPrepareToCreateNewOne(0);
        }

        // movement to cell complete
        if (isSandworm()) {
            map.cellResetIdFromLayer(iCell, MAPID_WORMS);
        } else {
            map.cellResetIdFromLayer(iCell, MAPID_UNITS);
        }

        setCell(iNextCell);
        iPathIndex++;
        iPathFails = 0; // we change this to 0... every cell

        updateCellXAndY();

        // check any units that are on our cell as well.
        // perhaps this could be sped up if we had more than 1 unit on a cell possible (for now we don't have
        // this). An alternative is to have a function return per 'segment' the units within it (so we don't need to
        // iterate over all units)
        if (canSquishInfantry()) {
            for (int iq = 0; iq < MAX_UNITS; iq++) {
                cUnit &potentialDeadUnit = unit[iq];
                if (iq == iID) continue; // skip self
                if (!potentialDeadUnit.isValid()) continue;
                if (potentialDeadUnit.iCell != iCell) continue; // not on my cell
                if (!potentialDeadUnit.canBeSquished()) continue;

                if (potentialDeadUnit.isSaboteur()) {
                    // this unit takes damage, catches the explosion so to speak
                    takeDamage(potentialDeadUnit.getUnitInfo().damageOnEnterStructure);
                }

                // die
                potentialDeadUnit.die(false, true);
            }
        }

        if (iPlayer == AI_CPU5 && players[HUMAN].isHouse(ATREIDES)) {
            // TODO: make this work for all allied forces
            // hackish way to get Fog of war clearance by allied fremen units (super weapon).
            map.clearShroud(iCell, sUnitInfo[iType].sight, HUMAN);
        }

        map.clearShroud(iCell, sUnitInfo[iType].sight, iPlayer);

        // The goal did change probably, or something else forces us to reconsider
        if (bCalculateNewPath) {
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }

        if (iCarryAll > -1) {
            log("A carry all is after me...");
            // wait longer for the carry-all to arive before thinking of a new path
            forgetAboutCurrentPathAndPrepareToCreateNewOne(10000);
            return eUnitMoveToCellResult::MOVERESULT_WAIT_FOR_CARRYALL; // wait a second will ya!
        }

        // Just arrived at goal cell
        if (iCell == iGoalCell) {
            return eUnitMoveToCellResult::MOVERESULT_AT_GOALCELL;
        }

        TIMER_movewait = 2 + ((getUnitInfo().speed + iSlowDown) * 3);
        return eUnitMoveToCellResult::MOVERESULT_AT_CELL;
    }
    return eUnitMoveToCellResult::MOVERESULT_BUSY_MOVING;
}

int cUnit::pos_y_centered() {
    return pos_y() + 16;
}

int cUnit::pos_x_centered() {
    return pos_x() + 16;
}

/**
 * Clears the created path, resets next-cell to current cell. Sets timer to wait to 100. So that
 * after that the unit will think of a new path to create.
 */
void cUnit::forgetAboutCurrentPathAndPrepareToCreateNewOne() {
    forgetAboutCurrentPathAndPrepareToCreateNewOne(35);
}

/**
 * Clears the created path, resets next-cell to current cell. Sets timer to wait to <timeToWait>. So that
 * after that the unit will think of a new path to create.
 */
void cUnit::forgetAboutCurrentPathAndPrepareToCreateNewOne(int timeToWait) {
    memset(iPath, -1, sizeof(iPath));
    iPathIndex = -1;
    TIMER_movewait = timeToWait;
}

bool cUnit::isInfantryUnit() const {
    return sUnitInfo[iType].infantry;
}

cUnit::cUnit() {
    mission = -1;
    init(-1);
}

cUnit::~cUnit() {
}

/**
 * Poor man solution to frequently update the dimensions of unit, better would be using events?
 * (onMove, onViewportMove, onViewportZoom?)
 */
void cUnit::thinkFast_position() {
    // keep updating dimensions
    dimensions.move(draw_x(), draw_y());
    if (mapCamera) {
        dimensions.resize(mapCamera->factorZoomLevel(getBmpWidth()),
                           mapCamera->factorZoomLevel(getBmpHeight()));
    }
}

bool cUnit::isMovingBetweenCells() {
    // when not perfectly divisible then it is 'between' cells.
    return ((int)posX) % TILESIZE_WIDTH_PIXELS != 0 || ((int)posY) % TILESIZE_HEIGHT_PIXELS != 0;
}

bool cUnit::isDamaged() {
    return iHitPoints < getUnitInfo().hp;
}

void cUnit::restoreFromTempHitPoints() {
    iHitPoints = iTempHitPoints; // restore true hitpoints
    int maxHp = getUnitInfo().hp;
    if (iHitPoints > maxHp) {
        iHitPoints = maxHp;
    }
    iTempHitPoints = -1; // get rid of this hack
}

void cUnit::setMaxHitPoints() {
    iHitPoints = getUnitInfo().hp;
}

bool cUnit::canSquishInfantry() {
    return getUnitInfo().squish;
}

bool cUnit::canBeSquished() {
    return getUnitInfo().canBeSquished;
}

cPlayer *cUnit::getPlayer() {
    return &players[iPlayer];
}

bool cUnit::isSaboteur() {
    return iType == SABOTEUR;
}

void cUnit::move_to(int iGoalCell) {
    eUnitActionIntent intent = eUnitActionIntent::INTENT_MOVE;

    int structureID = -1;
    int unitID = -1;
    if (iGoalCell > -1) {
        structureID = map.getCellIdStructuresLayer(iGoalCell);
        unitID = map.getCellIdUnitLayer(iGoalCell);
    }

    if (structureID > -1) {
        cAbstractStructure *pStructure = structure[structureID];
        if (pStructure && pStructure->isValid()) {
            bool friendlyStructure = getPlayer()->isSameTeamAs(pStructure->getPlayer());
            if (friendlyStructure) {
                if (isInfantryUnit()) {
                    structureID = -1; // reset back, we don't allow capturing own/allied buildings
                } else if (isHarvester()) {
                    if (pStructure->getType() == REFINERY) {
                        intent = eUnitActionIntent::INTENT_UNLOAD_SPICE;
                    } else if (pStructure->getType() == REPAIR) {
                        intent = eUnitActionIntent::INTENT_REPAIR;
                    }
                } else {
                    if (pStructure->getType() == REPAIR) {
                        intent = eUnitActionIntent::INTENT_REPAIR;
                    }
                }
            } else {
                // if capturable... (TODO)
                if (isInfantryUnit() || isSaboteur()) {
                    intent = eUnitActionIntent::INTENT_CAPTURE;
                } else {
                    //
                    intent = eUnitActionIntent::INTENT_MOVE;
                }
            }
        }
    }

    move_to(iGoalCell, structureID, unitID, intent);
}

void cUnit::setCell(int cll) {
    this->iCell = cll;
    setPosX(map.getAbsoluteXPositionFromCell(cll));
    setPosY(map.getAbsoluteYPositionFromCell(cll));
}

void cUnit::assignMission(int aMission) {
    log(fmt::format("I'm being assigned to mission {} (prev mission was {})", aMission, mission));
    mission = aMission;
}

bool cUnit::isAssignedAnyMission() {
    return mission > -1;
}

bool cUnit::isAssignedMission(int aMission) {
    return mission == aMission;
}

void cUnit::unAssignMission() {
    mission = -1;
}

int cUnit::getPlayerId() const {
     return iPlayer;
}

int cUnit::getType() const {
    return iType;
}

std::string cUnit::eUnitActionIntentString(eUnitActionIntent intent) {
    switch (intent) {
        case eUnitActionIntent::INTENT_CAPTURE: return "INTENT_CAPTURE";
        case eUnitActionIntent::INTENT_MOVE: return "INTENT_MOVE";
        case eUnitActionIntent::INTENT_ATTACK: return "INTENT_ATTACK";
        case eUnitActionIntent::INTENT_NONE: return "INTENT_NONE";
        case eUnitActionIntent::INTENT_REPAIR: return "INTENT_REPAIR";
        case eUnitActionIntent::INTENT_UNLOAD_SPICE: return "INTENT_UNLOAD_SPICE";
        default:
            assert(false);
            break;
    }
    return "";
}

/**
 * Returns true when a unit is surrounded by things that cant move, ie structures or terrain.
 * @return
 */
bool cUnit::isUnableToMove() {
    if (map.occupied(CELL_LEFT(iCell), iID) &&
        map.occupied(CELL_RIGHT(iCell), iID) &&
        map.occupied(CELL_ABOVE(iCell), iID) &&
        map.occupied(CELL_UNDER(iCell), iID) &&
        map.occupied(CELL_L_LEFT(iCell), iID) &&
        map.occupied(CELL_L_RIGHT(iCell), iID) &&
        map.occupied(CELL_U_RIGHT(iCell), iID) &&
        map.occupied(CELL_U_LEFT(iCell), iID)) {
        return true;
    }

    return false;
}

void cUnit::move_to_enter_structure(cAbstractStructure *pStructure, eUnitActionIntent intent) {
    if (!pStructure) return;
    if (!pStructure->isValid()) return;

    // found a candidate, lets check if we can find a carry-all that will bring us
    if (!pStructure->hasUnitHeadingTowards() && !pStructure->hasUnitWithin()) {
        pStructure->unitHeadsTowardsStructure(iID);
        iStructureID = pStructure->getStructureId();
    } else {
        // we chose to head towards this structure, but we can't claim it; this is
        // the situation that more units than available structures are heading towards the same structure
        // in this case we only remember the structureId, as we still have the intent to head towards it.
        // we let the other code that with "entering a structure" deal with any possibly already occupied
        // structure stuff.
        iStructureID = pStructure->getStructureId();
    }
    move_to(pStructure->getRandomStructureCell(), pStructure->getStructureId(), -1, intent);
}

cAbstractStructure *cUnit::findBestStructureCandidateToHeadTo(int structureType) {
    cAbstractStructure * candidate = findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(structureType);
    // TODO: we could do this even better, by scanning the structures once. Remembering distance and a 'state'
    // so to speak, ie "AVAILABLE, NOT AVAILABLE, ALREADY_HEADING", etc. That way we only need to scan once, and don't
    // need to call these functions multiple times. Also, the logic could be more smart then which (distance) to pick.
    // but for now settle with this - as I want 0.6.0 get done at some point. So deliberately deferring the optimisation
    // in favor of getting things done.
    if (candidate) {
        return candidate;
    }

    candidate = findClosestAvailableStructureType(structureType);
    if (candidate) {
        return candidate;
    }

    return findClosestStructureType(structureType);
}

void cUnit::findBestStructureCandidateAndHeadTowardsItOrWait(int structureType, bool allowCarryallTransfer) {
    iFrame = 0; // stop animating
    assert(structureType > -1);

    log(fmt::format("cUnit::findBestStructureCandidateAndHeadTowardsItOrWait - Going to look for a [{}]",
                    sStructureInfo[structureType].name));

    cAbstractStructure *candidate = findBestStructureCandidateToHeadTo(structureType);

    if (!candidate) {
        // none found, wait
        TIMER_thinkwait = 10;
        return;
    }

    if (!allowCarryallTransfer) {
        move_to_enter_structure(candidate, INTENT_UNLOAD_SPICE);
        TIMER_movewait = 0;
        return;
    }

    int destCell = candidate->getCell() + 2;

    // try to get a carry-all to help when a bit bigger distance
    if (map.distance(iCell, destCell) > 4) {
        if (findAndOrderCarryAllToBringMeToStructureAtCell(candidate, destCell)) {
            return;
        }
    }

    // no Carry-all found, or distance too short
    move_to_enter_structure(candidate, INTENT_UNLOAD_SPICE);
    TIMER_movewait = 0;
}

bool cUnit::findAndOrderCarryAllToBringMeToStructureAtCell(cAbstractStructure *candidate, int destCell) {
    int r = CARRYALL_FREE_FOR_TRANSFER(iPlayer);
    if (r < 0) {
        return false;
    }

    cUnit &carryAll = unit[r];
    carryAll.carryAll_transferUnitTo(iID, destCell);

    // todo: "getCellWhereToBringUnit"? where to enter structure so to speak?
    awaitBeingPickedUpToBeTransferedByCarryAllToStructure(candidate);

    // TODO: somehow remember we want to do something here!?
    return true;
}

void cUnit::carryAll_transferUnitTo(int unitIdToTransfer, int destinationCell) {
    carryall_order(unitIdToTransfer, eTransferType::PICKUP, destinationCell, -1);
}

void cUnit::awaitBeingPickedUpToBeTransferedByCarryAllToStructure(cAbstractStructure *candidate) {
    TIMER_movewait = 500; // wait for pickup!
    TIMER_thinkwait = 500;
    if (!candidate->hasUnitHeadingTowards() && !candidate->hasUnitWithin()) {
        candidate->unitHeadsTowardsStructure(iID);
        iStructureID = candidate->getStructureId();
    } else {
        // we chose to head towards this structure, but we can't claim it; this is
        // the situation that more units than available structures are heading towards the same structure
        // in this case we only remember the structureId, as we still have the intent to head towards it.
        // we let the other code that with "entering a structure" deal with any possibly already occupied
        // structure stuff.
        iStructureID = candidate->getStructureId();
    }
}

void cUnit::hideUnit() {
    iTempHitPoints = iHitPoints;
    iHitPoints = -1; // 'kill' unit
}

bool cUnit::belongsTo(const cPlayer *pPlayer) const {
    if (pPlayer == nullptr) return false;
    return pPlayer->getId() == iPlayer;
}

bool cUnit::isMarkedForRemoval() {
    return bRemoveMe;
}

bool cUnit::isWithinViewport(cRectangle *viewport) const {
    if (viewport == nullptr) return false;
    return dimensions.isOverlapping(viewport);
}

void cUnit::draw_debug() {
    allegroDrawer->drawRectangle(bmp_screen, dimensions, makecol(255, 0, 255));
    putpixel(bmp_screen, center_draw_x(), center_draw_y(), makecol(255, 0, 255));
    alfont_textprintf(bmp_screen, game_font, draw_x(), draw_y(), makecol(255, 255, 255), "%d", iID);

    if (isSandworm()) {
        alfont_textprintf(bmp_screen, game_font, draw_x(), draw_y()-16, makecol(255, 255, 255), "%d / %d / %d", unitsEaten, TIMER_guard, TIMER_movewait);
    }
}

void cUnit::takeDamage(int damage) {
    iHitPoints -= damage;
    if (isDead()) {
        die(true, false);
    } else {
        if (boundParticleId < 0) {
            if (getUnitInfo().renderSmokeOnUnitWhenThresholdMet) {
                float hpThreshold = getUnitInfo().smokeHpFactor * getUnitInfo().hp;
                if (iHitPoints < hpThreshold) {
                    int particleId = cParticle::create(
                            pos_x_centered(),
                            pos_y_centered(),
                            D2TM_PARTICLE_SMOKE,
                            -1,
                            -1,
                            iID
                    );
                    boundParticleId = particleId;
                }
            }
        }

        if (iHitPoints < getUnitInfo().dieWhenLowerThanHP) {
            iHitPoints = 0; // to make it appear 'dead' for the rest of the code
            // unit does not explode in this case, simply vanishes
            die(false, false);
        }
    }
}

bool cUnit::isEligibleForRepair() {
    return isDamaged() && !isInfantryUnit() && !isAirbornUnit();
}

bool cUnit::isAbleToGuard() {
    return getUnitInfo().canGuard;
}

// called every 5 ms
// used for movement logic
void cUnit::thinkFast() {
    thinkFast_position();

    // aircraft
    if (isAirbornUnit()) {
        thinkFast_move_airUnit();
    } else {
        // move
        if (m_action == eActionType::MOVE || m_action == eActionType::ATTACK || isMovingBetweenCells()) {
            thinkFast_move();
        }
    }

    // guard
    if (m_action == eActionType::GUARD) {
        thinkFast_guard();
    }
}

void cUnit::thinkFast_guard_sandworm() {
    int iDistance = 9999;
    int unitIdToAttack = -1;

    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &potentialDinner = unit[i];
        if (i == iID) continue;
        if (!potentialDinner.isValid()) continue;
        if (potentialDinner.getPlayer()->isSameTeamAs(getPlayer())) continue;
        if (potentialDinner.isAirbornUnit()) continue;
        if (potentialDinner.isSandworm()) continue; // don't eat other worms

        double distance = map.distance(iCell, potentialDinner.iCell);

        if (distance <= getSight() && distance < iDistance) {
            iDistance = distance;
            unitIdToAttack = i;
        }
    }

    if (unitIdToAttack > -1) {
        cUnit &unitToAttack = unit[unitIdToAttack];

        if (unitToAttack.isValid()) {
//            s_GameEvent event{
//                    .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
//                    .entityType = eBuildType::UNIT,
//                    .entityID = unitToAttack.iID,
//                    .player = getPlayer(),
//                    .entitySpecificType = unitToAttack.getType(),
//                    .atCell = unitToAttack.iCell
//            };
//            game.onNotifyGameEvent(event);
        }

        attackUnit(unitIdToAttack);
    }
}

/**
 * Checks for units within given range (in cells).
 * @return
 */
int cUnit::findNearbyGroundUnitToAttack(int range) {
    // TODO: it would be nice if we could have a 'predicate' lambda here, so we can use that in the for loop
    // and have 1 function to do some of the logic. For now it is very explicit and duplicated.
    // See also findNearbyAirUnitToAttack, or the thinkFast_guard_sandworm logic.

    // TODO: another improvement would be to return list of ids in range, so we can order them
    // later by priority?

    int iDistance = 9999;
    int unitIdToAttack = -1;

    for (int i = 0; i < MAX_UNITS; i++) {
        if (i == iID) continue; // skip self
        cUnit &potentialThreat = unit[i];
        if (!potentialThreat.isValid()) continue;
        if (potentialThreat.belongsTo(getPlayer())) continue; // skip own units
        if (potentialThreat.isAirbornUnit()) continue; // skip all airborn units (only focus on ground units)
        if (getPlayer()->isSameTeamAs(potentialThreat.getPlayer())) continue; // skip same team players / allies
        if (!map.isVisible(potentialThreat.iCell, iPlayer)) continue; // skip non-visible potential enemy units

        int distance = map.distance(iCell, potentialThreat.iCell);

        if (distance <= range && distance < iDistance) {
            iDistance = distance;
            unitIdToAttack = i;
        }
    }

    return unitIdToAttack;
}

int cUnit::findNearbyAirUnitToAttack(int range) {
    int iDistance = 9999;
    int airUnitToAttack = -1;

    for (int i = 0; i < MAX_UNITS; i++) {
        if (i == iID) continue; // skip self
        cUnit &potentialThreat = unit[i];
        if (!potentialThreat.isValid()) continue;
        if (!potentialThreat.isAirbornUnit()) continue; // skip all non-airborn units right away
        if (potentialThreat.getPlayerId() == getPlayerId()) continue; // skip own units
        if (getPlayer()->isSameTeamAs(potentialThreat.getPlayer())) continue; // skip same team players / allies
        if (!potentialThreat.isAttackableAirUnit()) continue;
        if (!map.isVisible(potentialThreat.iCell, iPlayer)) continue; // skip non-visible potential enemy units

        int distance = map.distance(iCell, potentialThreat.iCell);

        if (distance <= range &&
            distance < iDistance) // closer than found thus far
        {
            iDistance = distance;
            airUnitToAttack = i;
        }
    }

    return airUnitToAttack;
}

int cUnit::findNearbyStructureThatCanDamageUnitsToAttack(int range) {
    // TODO: have some kind of lambda here as well

    // TODO: another improvement would be to return list of ids in range, so we can order them
    // later by priority?
    int structureIdToAttack = -1;
    int iDistance = 9999;

    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = structure[i];
        if (!pStructure) continue;
        if (!pStructure->isValid()) continue;
        if (getPlayer()->isSameTeamAs(pStructure->getPlayer())) continue;
        if (!map.isStructureVisible(pStructure, iPlayer)) continue; // not visible

        // ignore structures which cannot attack air or ground units
        if (!pStructure->canAttackAirUnits()) continue;
        if (!pStructure->canAttackGroundUnits()) continue;

        // see big comment about this in findNearbyStructureToAttack
        int distance = map.distance(iCell, pStructure->getRandomStructureCell());

        if (distance <= range && distance < iDistance) {
            iDistance = distance;
            structureIdToAttack = i;
        }
    }

    return structureIdToAttack;
}

int cUnit::findNearbyStructureToAttack(int range) {
    // TODO: have some kind of lambda here as well

    // TODO: another improvement would be to return list of ids in range, so we can order them
    // later by priority?
    int structureIdToAttack = -1;
    int iDistance = 9999;

    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = structure[i];
        if (!pStructure) continue;
        if (!pStructure->isValid()) continue;
        if (getPlayer()->isSameTeamAs(pStructure->getPlayer())) continue;
        if (!map.isStructureVisible(pStructure, iPlayer)) continue; // not visible

        // TODO: this is a bit tricky and hacky
        // basically you want to check against the 'closest' cell from that structure.
        // Example:
        // suppose you want to attack a Palace, and you are at the bottom left of that
        // structure. The palace's bottom-left cell would be closest to the unit and
        // hence be evaluated.
        //
        // the other approach would be to check cells around a unit, but that is potentially
        // even more expensive because then you will be checking for every unit, every cell in range
        // although we can read the ID of a structure.
        //
        // A quick calculation says:
        // 10 units, with a 'range' of 5 cells, gives (if we assume a square for now) ~ 5*5 = 25 - 1 (own cell) = 24
        // cells to evaluate. For every unit * 10 = 240 cells to evaluate.
        //
        // Regardless, this is something to think about. There are probably better/smarter ways to do this.
        int distance = map.distance(iCell, pStructure->getRandomStructureCell());

        if (distance <= range && distance < iDistance) {
            iDistance = distance;
            structureIdToAttack = i;
        }
    }

    return structureIdToAttack;
}

void cUnit::think_MVC() {
    cPlayer *pPlayer = getPlayer();
    if (pPlayer->isHuman()) {
        if (bSelected) {
            if (key[KEY_D]) {
                bool result = pPlayer->canPlaceStructureAt(iCell, CONSTYARD, iID).success;

                if (result) {
                    int iLocation = iCell;

                    die(false, false);

                    // place const yard
                    pPlayer->placeStructure(iLocation, CONSTYARD, 100);
                    return;
                }
            }
        }
    }
}

int cUnit::getTurnSpeed() {
    return sUnitInfo[iType].turnspeed;
}

void cUnit::think_harvester() {
    bool bFindRefinery = false;

    if (iCredits > 0 && bSelected && key[KEY_D]) {
        bFindRefinery = true;
    }

    // cell = goal cell (doing nothing)
    if (iCell == iGoalCell) {
        int cellType = map.getCellType(iCell);
        // when on spice, harvest
        if (cellType == TERRAIN_SPICE ||
            cellType == TERRAIN_SPICEHILL) {
            // do timer stuff
            if (iCredits < sUnitInfo[iType].credit_capacity)
                TIMER_harvest++;
        } else {
            // not on spice, find a new location
            if (iCredits < sUnitInfo[iType].credit_capacity) {
                // find harvest cell
                move_to(UNIT_find_harvest_spot(iID), -1, -1);
            } else {
                iFrame = 0;
                bFindRefinery = true;
                // find a refinery
            }
        }

        if (iCredits >= sUnitInfo[iType].credit_capacity)
            bFindRefinery = true;

        // when we should harvest...
        cPlayerDifficultySettings *difficultySettings = players[iPlayer].getDifficultySettings();
        if (TIMER_harvest > (difficultySettings->getHarvestSpeed(sUnitInfo[iType].harvesting_speed)) &&
            iCredits < sUnitInfo[iType].credit_capacity) {
            TIMER_harvest = 1;

            iFrame++;

            if (iFrame > 3)
                iFrame = 1;

            iCredits += sUnitInfo[iType].harvesting_amount;
            map.cellTakeCredits(iCell, sUnitInfo[iType].harvesting_amount);

            // turn into sand/spice (when spicehill)
            if (map.getCellCredits(iCell) <= 0) {
                if (cellType == TERRAIN_SPICEHILL) {
                    map.cellChangeType(iCell, TERRAIN_SPICE);
                    map.cellGiveCredits(iCell, rnd(100));
                } else {
                    map.cellChangeType(iCell, TERRAIN_SAND);
                    map.cellChangeTile(iCell, 0);
                }

                move_to(UNIT_find_harvest_spot(iID), -1, -1);

                mapEditor.smoothAroundCell(iCell);
            }
        }

        // refinery required, go find one that is available
        if (bFindRefinery) {
            findBestStructureCandidateAndHeadTowardsItOrWait(REFINERY, true);
            return;
        }
    } else {
        // ??
        iFrame = 0;
    }
}


// return new valid ID
int UNIT_NEW() {
    for (int i = 0; i < MAX_UNITS; i++)
        if (!unit[i].isValid())
            return i;

    return -1; // NONE
}

/**
 * Creates a new unit, when bOnStart is true, it will prevent AI players from moving a unit immediately a bit.
 * Assumes the creation of a unit is NOT a reinforcement.
 *
 * @param iCll
 * @param unitType
 * @param iPlayer
 * @param bOnStart (if true, then AI will *not* move unit away immediately to a nearby random position)
 * @return
 */
int UNIT_CREATE(int iCll, int unitType, int iPlayer, bool bOnStart) {
    return UNIT_CREATE(iCll, unitType, iPlayer, bOnStart, false);
}

/**
 * Creates a new unit, when bOnStart is true, it will prevent AI players from moving a unit immediately a bit.
 *
 *
 * @param iCll
 * @param unitType
 * @param iPlayer
 * @param bOnStart
 * @param isReinforement flag to set on event
 * @return
 */
int UNIT_CREATE(int iCll, int unitType, int iPlayer, bool bOnStart, bool isReinforcement) {
    if (!map.isValidCell(iCll)) {
        logbook("UNIT_CREATE: Invalid cell as param");
        return -1;
    }

    s_UnitInfo &sUnitType = sUnitInfo[unitType];

    // check if unit already exists on location
    if (!sUnitType.airborn && map.cellGetIdFromLayer(iCll, MAPID_STRUCTURES) > -1) {
        return -1; // cannot place unit, structure exists at location
    }

    int mapIdIndex = MAPID_UNITS;
    if (sUnitType.airborn) {
        mapIdIndex = MAPID_AIR;
    } else if (unitType == SANDWORM) {
        mapIdIndex = MAPID_WORMS;
    }

    // check if unit already exists on location
    if (map.cellGetIdFromLayer(iCll, mapIdIndex) > -1) {
        return -1; // cannot place unit
    }

    // check if placed on invalid terrain type
    if (unitType == SANDWORM) {
        if (!map.isCellPassableForWorm(iCll)) {
            return -1;
        }
    }

    bool validCell = map.canDeployUnitTypeAtCell(iCll, unitType);
    if (!validCell) {
        return -1;
    }

    int iNewId = UNIT_NEW();

    if (iNewId < 0) {
        logbook("UNIT_CREATE:Could not find new unit index");
        return -1;
    }

    cUnit &newUnit = unit[iNewId];
    newUnit.init(iNewId);

    newUnit.setCell(iCll);
    newUnit.iBodyFacing = rnd(8);
    newUnit.iHeadFacing = rnd(8);

    newUnit.iBodyShouldFace = newUnit.iBodyFacing;
    newUnit.iHeadShouldFace = newUnit.iHeadFacing;

    newUnit.iType = unitType;
    newUnit.setMaxHitPoints();
    newUnit.iGoalCell = iCll;

    newUnit.bSelected = false;
    newUnit.bHovered = false;

    newUnit.TIMER_bored = rnd(3000);
    newUnit.TIMER_guard = 20 + rnd(70);
    newUnit.recreateDimensions();

    // set (Correct!?) player id, when type is SANDWORM (!?)
    if (unitType == SANDWORM) {
        if (iPlayer != AI_WORM) {
            newUnit.log("ERROR: Wanted to create sandworm for player other than AI_WORM!?");
        }
        iPlayer = AI_WORM;
    }

    newUnit.iPlayer = iPlayer;

    // AI player immediately moves unit away
    if (iPlayer > 0 && iPlayer < AI_WORM && !sUnitType.airborn && !bOnStart) {
        int iF = UNIT_FREE_AROUND_MOVE(iNewId);

        if (iF > -1) {
            newUnit.log("Order move #2");
            unit[iNewId].move_to(iF);
        }
    }

    // Put on map too!:
    map.cellSetIdForLayer(iCll, mapIdIndex, iNewId);

    newUnit.updateCellXAndY();

    map.clearShroud(iCll, sUnitType.sight, iPlayer);

    s_GameEvent event {
            .eventType = eGameEventType::GAME_EVENT_CREATED,
            .entityType = eBuildType::UNIT,
            .entityID = iNewId,
            .player = newUnit.getPlayer(),
            .entitySpecificType = unitType,
            .atCell = -1,
            .isReinforce = isReinforcement
    };

    game.onNotifyGameEvent(event);

    return iNewId;
}

/*
  Pathfinder

  Last revision: 19/02/2006

  The pathfinder will first eliminate any possibility that it will fail.
  It will check if the unit is free to move, if the timer is set correctly
  and so forth.

  Then the actual FDS path finder starts. Which will output a 'reversed' traceable
  path.

  Eventually this path is converted back to a waypoint string for units, and also
  optimized (skipping attaching cells which are reachable and closer to goal or further on
  path string).

  Return possibilities:
  -1 = FAIL (goalcell = cell, or cannot find path)
  -2 = Cannot move, all surrounded (blocked)
  -3 = Too many paths created
  -4 = Offset is not 0 (moving between cells)
  -99= iUnitId is < 0 (invalid input)
  */
int CREATE_PATH(int iUnitId, int iPathCountUnits) {
    logbook("CREATE_PATH -- START");
    if (iUnitId < 0) {
        logbook("CREATE_PATH -- END 1");
        return -99; // Wut!?
    }

    cUnit &pUnit = unit[iUnitId];
    if (!pUnit.isValid()) {
        return -99; // for now...
    }

    // do not start calculating anything before we are on 0,0 x,y wise on a cell
    if (pUnit.isMovingBetweenCells()) {
        pUnit.log("CREATE_PATH -- END 2");
        return -4; // no calculation before we are straight on a cell
    }

    // Too many paths where created , so we wait a little.
    // make sure not to create too many paths at once
    if (game.m_pathsCreated > 40) {
        pUnit.log("CREATE_PATH -- END 3");
        pUnit.TIMER_movewait = (50 + rnd(50));
        return -3;
    }

    int iCell = pUnit.getCell(); // current cell

    // When the goal == cell, then skip.
    if (iCell == pUnit.iGoalCell) {
        pUnit.log("CREATE_PATH -- END 4");
        pUnit.log("ODD: The goal = cell?");
        return -1;
    }

    // when all around the unit there is no space, dont even bother
    if (pUnit.isUnableToMove()) {
        pUnit.log("CREATE_PATH -- END 5");
        pUnit.TIMER_movewait = 30 + rnd(50);
        return -2;
    }

    // Now start create path

    // Clear unit path settings (index & path string)
    memset(pUnit.iPath, -1, sizeof(pUnit.iPath));

    pUnit.iPathIndex = -1;

    // Search around a cell:
    int cx, cy, the_cll, ex, ey;
    int goal_cell = pUnit.iGoalCell;
    int controller = pUnit.iPlayer;

    game.m_pathsCreated++;
    memset(temp_map, -1, sizeof(temp_map));

    the_cll = -1;
    ex = -1;
    ey = -1;
    cx = map.getCellX(iCell);
    cy = map.getCellY(iCell);

    // set very first... our start cell
    temp_map[iCell].cost = ABS_length(cx, cy, map.getCellX(goal_cell), map.getCellY(goal_cell));
    temp_map[iCell].parent = -1;
    temp_map[iCell].state = CLOSED;

    bool valid = true;
    bool succes = false;
    bool found_one = false;

    int sx, sy;
    double cost = -1;

    // WHILE VALID TO RUN THIS LOOP
    while (valid) {
        // iCell reached Goal Cell
        if (iCell == goal_cell) {
            valid = false;
            succes = true;
            break;
        }

        int idOfStructureAtCell = map.cellGetIdFromLayer(iCell, MAPID_STRUCTURES);
        if (pUnit.iStructureID > -1) {
            if (idOfStructureAtCell == pUnit.iStructureID) {
                valid = false;
                succes = true;
                pUnit.log("Found structure ID");
                break;
            }
        }

        if (pUnit.iAttackStructure > -1) {
            if (idOfStructureAtCell == pUnit.iAttackStructure) {
                valid = false;
                succes = true;
                pUnit.log("Found attack structure ID");
                break;
            }
        }

        cx = map.getCellX(iCell);
        cy = map.getCellY(iCell);

        // starting position is cx-1 and cy-1
        sx = cx - 1;
        sy = cy - 1;

        // check for not going under zero
        ex = cx + 1;
        ey = cy + 1;

        // boundries
        FIX_BORDER_POS(sx, sy);
        FIX_BORDER_POS(ex, ey);

//        if (ex <= cx)
//            pUnit.log("CX = EX");
//        if (ey <= cy)
//            pUnit.log("CY = EY");

        cost = 999999999;
        the_cll = -1;
        found_one = false;

        // go around the cell we are checking!
        bool bail_out = false;

        // circle around cell X wise
        for (cx = sx; cx <= ex; cx++) {
            // circle around cell Y wise
            for (cy = sy; cy <= ey; cy++) {
                // only check the 'cell' that is NOT the current cell.
                int cll = map.getCellWithMapBorders(cx, cy);

                // skip invalid cells
                if (cll < 0)
                    continue;

                // DO NOT CHECK SELF
                if (cll == iCell)
                    continue;

                // Determine if it is a good cell to use or not:
                bool good = false; // not good by default

                // not a sandworm
                int cellType = map.getCellType(cll);
                if (!pUnit.isSandworm()) {
                    // Step by step determine if its good
                    // 2 fases:
                    // 1 -> Occupation by unit/structures
                    // 2 -> Occupation by terrain (but only when it is visible, since we do not want to have an
                    //      advantage or some super intelligence by units for unknown territories!)
                    int idOfUnitAtCell = map.getCellIdUnitLayer(cll);
                    int idOfStructureAtCell = map.getCellIdStructuresLayer(cll);

                    if (idOfUnitAtCell == -1 && idOfStructureAtCell == -1) {
                        // there is nothing on this cell, that is good
                        good = true;
                    }

                    if (idOfStructureAtCell > -1) {
                        // when the cell is a structure, and it is the structure we want to attack, it is good

                        if (pUnit.iAttackStructure > -1)
                            if (idOfStructureAtCell == pUnit.iAttackStructure)
                                good = true;

                        if (pUnit.iStructureID > -1)
                            if (idOfStructureAtCell == pUnit.iStructureID)
                                good = true;

                    }

                    // blocked by other then our own unit
                    if (idOfUnitAtCell > -1) {
                        // occupied by a different unit than ourselves
                        if (idOfUnitAtCell != iUnitId) {
                            int iUID = idOfUnitAtCell;

                            if (iPathCountUnits != 0) {
                                if (iPathCountUnits <= 0) {
                                    good = false;
//                                    char msg[255];
//                                    sprintf(msg,
//                                            "CREATE_PATH(unitId=%d) - iPathCountUnits < 0 - variable 'good' becomes 'false'",
//                                            iUnitId);
//                                    pUnit.log(msg);
                                }
                            }

                            cUnit &unitAtCell = unit[iUID];
                            if (!unitAtCell.getPlayer()->isSameTeamAs(pUnit.getPlayer())) {
                                // allow running over enemy infantry/squishable units
                                if (unitAtCell.isInfantryUnit() &&
                                    pUnit.canSquishInfantry()) // and the current unit can squish
                                    good = true; // its infantry we want to run over, so don't be bothered!
                            }
                            // it is not good, other unit blocks
                        } else {
                            good = true;
                        }
                    }

                    // is not visible, always good (since we don't know yet if its blocked!)
                    if (map.isVisible(cll, controller) == false) {
                        good = true;
                    } else {
                        // walls stop us
                        if (cellType == TERRAIN_WALL) {
                            good = false;
                        }

                        // When we are infantry, we move through mountains. However, normal units do not
                        if (!pUnit.isInfantryUnit()) {
                            if (cellType == TERRAIN_MOUNTAIN) {
                                good = false;
                            }
                        }
                    }
                } else {
                    // Sandworm only cares about terrain type for good/bad cells
                    good = map.isCellPassableForWorm(cll);
                }

                // it is the goal cell
                if (cll == goal_cell && good) {
                    the_cll = cll;
                    cost = 0;
                    succes = true;
                    found_one = true;
                    bail_out = true;
                    pUnit.log("CREATE_PATH: Found the goal cell, succes, bailing out");
                    break;
                }

                // when the cell (the attached one) is NOT the cell we are on and
                // the cell is CLOSED (not checked yet)
                if (cll != iCell &&                            // not checking on our own
                    temp_map[cll].state == CLOSED &&          // and is closed (else its not valid to check)
                    good) // and its not occupied (but may be occupied by own id!
                {
                    int gcx = map.getCellX(goal_cell);
                    int gcy = map.getCellY(goal_cell);

                    // calculate the cost
                    double d_cost = ABS_length(cx, cy, gcx, gcy) + temp_map[cll].cost;

                    // when the cost is lower then we had before
                    if (d_cost < cost) {
                        // when the cost is lower then the previous cost, then we set the new cost and we set the cell
                        the_cll = cll;
                        cost = d_cost;
                        // found a new cell, now decrease ipathcountunits
                        iPathCountUnits--;
                    }


/*
                 char msg[255];
                 sprintf(msg, "Waypoint found : cell %d - goalcell = %d", cll, goal_cell);
                 log(msg);*/

                } // END OF LOOP #2
            } // Y thingy

            // bail out
            if (bail_out) {
                //log("BAIL");
                break;
            }

        } // X thingy

        // When found a new c(e)ll;
        if (the_cll > -1) {
            // Open this one, so we do not check it again
            temp_map[the_cll].state = OPEN;
            temp_map[the_cll].parent = iCell;
            temp_map[the_cll].cost = cost;

            // Now set c to the cll
            iCell = the_cll;
            found_one = true;

            // DEBUG:
            // draws a SLAB when the cell is assigned for our path
            //map.cell[c].type = TERRAIN_SPICE;
            //map.cell[c].tile = 0;

            if (iCell == goal_cell) {
                valid = false;
                succes = true;
            }
        }

        if (found_one == false) {
            valid = false;
            succes = false;
            pUnit.log("FAILED TO CREATE PATH - nothing found to continue");
            break;
        }

    } // valid to run loop (and try to create a path)

    pUnit.log("CREATE_PATH -- valid loop finished");

    if (succes) {
        pUnit.log("CREATE_PATH -- success");
        // read path!
        int temp_path[MAX_PATH_SIZE];

        //log("NEW PATH FOUND");
        memset(temp_path, -1, sizeof(temp_path));
        //for (int copy=0; copy < PATH_MAX; copy++)
        // temp_path[copy] = -1;

        bool cp = true;

        int sc = iCell;
        int pi = 0;

        temp_path[pi] = sc;
        pi++;

        // while we should create a path
        while (cp) {
            int tmp = temp_map[sc].parent;
            if (tmp > -1) {
                // found terminator (PARENT=CURRENT)
                if (tmp == sc) {
                    cp = false;
                    continue;
                } else {
                    temp_path[pi] = tmp;
                    sc = temp_map[sc].parent;
                    pi++;
                }
            } else
                cp = false;

            if (pi >= MAX_PATH_SIZE)
                cp = false;

            if (sc == pUnit.getCell())
                cp = false;
        }

        // reverse
        int z = MAX_PATH_SIZE - 1;
        int a = 0;
        int iPrevCell = -1;

        while (z > -1) {
            if (temp_path[z] > -1) {
                // check if any other cell of temp_path also borders to the previous given cell, as that will save us time
                if (iPrevCell > -1) {
                    int iGoodZ = -1;

                    for (int sz = z; sz > 0; sz--) {
                        if (temp_path[sz] > -1) {

                            if (CELL_BORDERS(iPrevCell, temp_path[sz])) {
                                iGoodZ = sz;
                            }
                            //if (ABS_length(iCellGiveX(iPrevCell), iCellGiveY(iPrevCell), iCellGiveX(temp_path[sz]), iCellGiveY(temp_path[sz])) <= 1)
                            //  iGoodZ=sz;
                        } else
                            break;
                    }

                    if (iGoodZ < z && iGoodZ > -1)
                        z = iGoodZ;
                }

                pUnit.iPath[a] = temp_path[z];
                iPrevCell = temp_path[z];
                a++;
            }
            z--;
        }

        // optimize path
        //nextcell=cell;
        pUnit.iPathIndex = 1;

        // take the closest bordering cell as 'far' away to start with
        for (int i = 1; i < MAX_PATH_SIZE; i++) {
            if (pUnit.iPath[i] > -1) {
                if (CELL_BORDERS(pUnit.getCell(), pUnit.iPath[i]))
                    pUnit.iPathIndex = i;
            }
        }



        /*
        // debug debug
        for (int i=0; i <	MAX_PATH_SIZE; i++)
        {
           if (unit[iUnitId].iPath[i] > -1)
           {
               char msg[256];
               sprintf(msg, "WAYPOINT %d = %d ", iUnitId, i, unit[iUnitId].iPath[i]);
               unit[iUnitId].log(msg);
           }
        }*/


        pUnit.updateCellXAndY();
        pUnit.bCalculateNewPath = false;


        //log("SUCCES");
        return 0; // succes!

    } else {
        pUnit.log("CREATE_PATH -- not valid");

        // perhaps we can get closer when we DO take units into account?
        //path_id=-1;
    }

    pUnit.log("CREATE_PATH: Failed to create path!");
    return -1;
}


// find
int RETURN_CLOSE_GOAL(int iCll, int iMyCell, int iID) {
    //
    int iSize = 1;
    int iStartX = map.getCellX(iCll) - iSize;
    int iStartY = map.getCellY(iCll) - iSize;
    int iEndX = map.getCellX(iCll) + iSize;
    int iEndY = map.getCellX(iCll) + iSize;

    float dDistance = 9999;

    int ix = map.getCellX(iMyCell);
    int iy = map.getCellY(iMyCell);

    bool bSearch = true;

    int iTheClosest = -1;

    while (bSearch) {
        iStartX = map.getCellX(iCll) - iSize;
        iStartY = map.getCellY(iCll) - iSize;
        iEndX = map.getCellX(iCll) + iSize;
        iEndY = map.getCellY(iCll) + iSize;

        // Fix boundries
        FIX_BORDER_POS(iStartX, iStartY);
        FIX_BORDER_POS(iEndX, iEndY);

        // search
        for (int iSX = iStartX; iSX < iEndX; iSX++)
            for (int iSY = iStartY; iSY < iEndY; iSY++) {
                // find an empty cell
                int cll = map.getCellWithMapDimensions(iSX, iSY);

                float dDistance2 = ABS_length(iSX, iSY, ix, iy);

                int idOfStructureAtCell = map.getCellIdStructuresLayer(cll);
                int idOfUnitAtCell = map.getCellIdUnitLayer(cll);

                if ((idOfStructureAtCell < 0) && (idOfUnitAtCell < 0)) // no unit or structure at cell
                {
                    // depending on unit type, do not choose walls (or mountains)
                    int cellType = map.getCellType(cll);
                    if (sUnitInfo[unit[iID].iType].infantry) {
                        if (cellType == TERRAIN_MOUNTAIN)
                            continue; // do not use this one
                    }

                    if (cellType == TERRAIN_WALL)
                        continue; // do not use this one

                    if (dDistance2 < dDistance) {
                        dDistance = dDistance2;
                        iTheClosest = cll;
                    }
                }
            }

        if (iTheClosest > -1)
            return iTheClosest;

        iSize++;

        if (iSize > 9) {
            bSearch = false;
            break; // get out
        }
    }

    // fail
    return iCll;
}

int UNIT_find_harvest_spot(int id) {
    // finds the closest harvest spot
    cUnit &cUnit = unit[id];
    cUnit.updateCellXAndY();
    int cx = cUnit.getCellX();
    int cy = cUnit.getCellY();

    int TargetSpice = -1;
    int TargetSpiceHill = -1;

    // distances
    int TargetSpiceDistance = 40;
    int TargetSpiceHillDistance = 40;


    for (int i = 0; i < map.getMaxCells(); i++)
        if (map.getCellCredits(i) > 0 && i != cUnit.getCell()) {
            // check if its not out of reach
            int dx = map.getCellX(i);
            int dy = map.getCellY(i);

            // skip bordered ones
            if (map.isWithinBoundaries(dx, dy) == false)
                continue;

            /*
            if (dx <= 1) continue;
            if (dy <= 1) continue;

            if (dx >= (game.map_width-1))
              continue;

            if (dy >= (game.map_height-1))
              continue;*/

            int idOfUnitAtCell = map.getCellIdUnitLayer(i);
            if (idOfUnitAtCell > -1)
                continue;

            if (map.occupied(i, id))
                continue; // occupied

            int d = ABS_length(cx, cy, map.getCellX(i), map.getCellY(i));

            int cellType = map.getCellType(i);
            if (cellType == TERRAIN_SPICE) {
                if (d < TargetSpiceDistance) {
                    TargetSpice = i;
                    TargetSpiceDistance = d; // update distance
                }
            } else if (cellType == TERRAIN_SPICEHILL) {
                if (d < TargetSpiceHillDistance) {
                    TargetSpiceHill = i;
                    TargetSpiceHillDistance = d; // update distance
                }
            }

            if (TargetSpiceHillDistance < 3 && TargetSpiceDistance < 3)
                break;

        }

    // when distance is greater then 3 (meaning 'far' away)
    if (TargetSpiceHillDistance >= 3 && TargetSpiceDistance >= 3) {
        return TargetSpice;
    } else {
        // when both are close, prefer spice hill
        if (TargetSpiceHill > -1 && (TargetSpiceHillDistance <= TargetSpiceDistance))
            return TargetSpiceHill;
    }

    return TargetSpice;
}

/**
 * Reinforce:
 * Assumes this is a 'real' reinforcement. (ie triggered by map)
 * create a new unit by sending it
 *
 * @param iPlr player index
 * @param iTpe unit type
 * @param iCll location where to bring it
 * @param iStart where to start from

 */
void REINFORCE(int iPlr, int iTpe, int iCll, int iStart) {
   REINFORCE(iPlr, iTpe, iCll, iStart, true);
}

void REINFORCE(int iPlr, int iTpe, int iCll, int iStart, bool isReinforcement) {

    // handle invalid arguments
    if (iPlr < 0 || iTpe < 0)
        return;

    //if (iCll < 0 || iCll >= MAX_CELLS)
    if (map.isValidCell(iCll) == false)
        return;

    if (iStart < 0)
        iStart = iCll;

    int iStartCell = iFindCloseBorderCell(iStart);

    if (iStartCell < 0) {
        iStart += rnd(64);
        if (iStart >= map.getMaxCells())
            iStart -= 64;

        iStartCell = iFindCloseBorderCell(iStart);
    }

    if (iStartCell < 0) {
        logbook("ERROR (reinforce): Could not figure a startcell");
        return;
    }

    logbook(fmt::format("REINFORCE: Bringing unit type {} for player {}. Starting from cell {}, going to cell {}",
                        iTpe, iPlr, iStartCell, iCll));

    // STEP 2: create carryall
    int iUnit = UNIT_CREATE(iStartCell, CARRYALL, iPlr, true, isReinforcement);
    if (iUnit < 0) {
        // cannot create carry-all!
        logbook("ERROR (reinforce): Cannot create CARRYALL unit.");
        return;
    }

    // STEP 3: assign order to carryall
    int iCellX = map.getCellX(iStartCell);
    int iCellY = map.getCellY(iStartCell);
    int cx = map.getCellX(iCll);
    int cy = map.getCellY(iCll);

    int d = fDegrees(iCellX, iCellY, cx, cy);
    int f = face_angle(d); // get the angle

    unit[iUnit].iBodyShouldFace = f;
    unit[iUnit].iBodyFacing = f;
    unit[iUnit].iHeadShouldFace = f;
    unit[iUnit].iHeadFacing = f;

    unit[iUnit].carryall_order(-1, eTransferType::NEW_LEAVE, iCll, iTpe);
}

int CARRYALL_FREE_FOR_TRANSFER(int iPlayer) {
    // find a free carry all
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != iPlayer) continue;
        if (cUnit.iType != CARRYALL) continue; // skip non-carry-all units
        if (cUnit.m_transferType != eTransferType::NONE) continue; // skip busy carry-alls
        return i;
    }
    
    return -1;
}

/**
 * Finds a free carryall of the same player as unit iuID. Returns > -1 which is the ID of the
 * carry-all which is going to pick up the unit. Or < 0 if no carry-all has been found to transfer unit.
 * @param iuID
 * @param iGoal
 * @return
 */
int CARRYALL_TRANSFER(int iuID, int iGoal) {
    int carryAllUnitId = CARRYALL_FREE_FOR_TRANSFER(unit[iuID].iPlayer);
    if (carryAllUnitId > -1) {
        cUnit &cUnit = unit[carryAllUnitId];
        cUnit.carryall_order(iuID, eTransferType::PICKUP, iGoal, -1);
    }
    return carryAllUnitId;
}

///////////////////////////////////////////////////////////////////////
// REINFORCEMENT STUFF
///////////////////////////////////////////////////////////////////////
void INIT_REINFORCEMENT() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        reinforcements[i].iCell = -1;
        reinforcements[i].iPlayer = -1;
        reinforcements[i].iSeconds = -1;
        reinforcements[i].iUnitType = -1;
    }
}

// returns next free reinforcement index
int NEXT_REINFORCEMENT() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        // new one yey
        if (reinforcements[i].iCell < 0)
            return i;
    }

    return -1;
}

// set reinforcement
void SET_REINFORCEMENT(int iCll, int iPlyr, int iTime, int iUType) {
    int iIndex = NEXT_REINFORCEMENT();

    // do not allow falsy indexes.
    if (iIndex < 0)
        return;

    if (iCll < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid cell given");
        return;
    }

    if (iPlyr < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid plyr");
        return;
    }

    if (iTime < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid time given");
        return;
    }

    if (iUType < 0) {
        logbook("REINFORCEMENT: Cannot set; invalid unit type given");
        return;
    }

    logbook(fmt::format("[{}] Reinforcement: Controller = {}, House {}, Time {}, Type = {}",
                        iIndex, iPlyr, players[iPlyr].getHouse(), iTime, iUType));

    // DEBUG DEBUG
    if (!DEBUGGING)
        iTime *= 15;

    reinforcements[iIndex].iCell = iCll;
    reinforcements[iIndex].iPlayer = iPlyr;
    reinforcements[iIndex].iUnitType = iUType;
    reinforcements[iIndex].iSeconds = iTime;
}

int UNIT_FREE_AROUND_MOVE(int iUnit) {
    if (iUnit < 0) {
        logbook("Invalid unit");
        return -1;
    }

    cUnit &cUnit = unit[iUnit];

    cUnit.updateCellXAndY();
    int iStartX = cUnit.getCellX();
    int iStartY = cUnit.getCellY();

    int iWidth = rnd(4);

    if (cUnit.iType == HARVESTER)
        iWidth = 2;

    int iEndX = (iStartX + 1) + iWidth;
    int iEndY = (iStartY + 1) + iWidth;

    iStartX -= iWidth;
    iStartY -= iWidth;

    int iClls[50];
    memset(iClls, -1, sizeof(iClls));
    int foundCoordinates = 0;

    for (int x = iStartX; x < iEndX; x++) {
        for (int y = iStartY; y < iEndY; y++) {
            int cll = map.getCellWithMapBorders(x, y);

            if (cll > -1 && !map.occupied(cll)) {
                iClls[foundCoordinates] = cll;
                foundCoordinates++;
            }

        }
    }

    if (foundCoordinates < 1) return -1;

    return iClls[rnd(foundCoordinates)]; // random cell
}