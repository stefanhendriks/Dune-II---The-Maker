/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include <math.h>
#include "../../include/d2tmh.h"
#include "cUnit.h"



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

    isReinforcement = false; // set to true by REINFORCE when a carry-all is spawned to bring a 'real' reinforcement. So we can
                             // emit the proper CREATED game event later. :/

    fExperience = 0;

    iID = i;

    iType = 0;          // type of unit

    iHitPoints = -1;     // hitpoints of unit
    iTempHitPoints = -1; // temp hold back for 'reinforced' / 'dumping' and 'repairing' units

    iPlayer = -1;        // belongs to player

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


    iAction = ACTION_GUARD;
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
    iTransferType = -1;    // -1 = none, 0 = new , 1 = carrying existing unit
    // iUnitID = unit we CARRY (when TransferType == 1)
    // iTempHitPoints = hp of unit when transfertype = 1

    iCarryTarget = -1;    // Unit ID to carry, but is not carried yet
    iBringTarget = -1;    // Where to bring the carried unit (when iUnitID > -1)
    iNewUnitType = -1;    // new unit that will be brought, will be this type
    bPickedUp = false;        // picked up the unit?

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
    TIMER_wormeat = 0;
    TIMER_movedelay = 0;
}

void cUnit::recreateDimensions() {
    if (dimensions) {
        delete dimensions;
    }

    // set up dimensions
    dimensions = new cRectangle(draw_x(), draw_y(), getBmpWidth(), getBmpHeight());
}

void cUnit::die(bool bBlowUp, bool bSquish) {
    // Animation / Sound
    // TODO: update statistics player

    int iDieX = pos_x() + getUnitType().bmp_width / 2;
    int iDieY = pos_y() + getUnitType().bmp_height / 2;

    // when HARVESTER, check if there are any friends , if not, then deliver one
    if (iType == HARVESTER && // a harvester died
        players[iPlayer].hasAtleastOneStructure(REFINERY)) { // and its player still has a refinery

        // check if the player has any harvester left
        bool bFoundHarvester = false;
        for (int i = 0; i < MAX_UNITS; i++) {
            if (i != iID) {
                cUnit &theUnit = unit[i];

                if (theUnit.isValid() && theUnit.iType == HARVESTER) {
                    if (theUnit.iPlayer == iPlayer) {
                        bFoundHarvester = true;
                        break;
                    }
                }
            }
        }

        // No harvester found, deliver one
        if (!bFoundHarvester) {
            // deliver

            cAbstractStructure *refinery = nullptr;
            for (int k = 0; k < MAX_STRUCTURES; k++) {
                cAbstractStructure *theStructure = structure[k];
                if (!theStructure) continue;
                if (theStructure->getOwner() != iPlayer) continue;
                if (theStructure->getType() != REFINERY) continue;

                // found!
                refinery = theStructure;
                break;
            }

            // found a refinery, deliver harvester to that
            if (refinery) {
                REINFORCE(iPlayer, HARVESTER, refinery->getCell(), -1);
            }
        }
    } // a harvester died, check if we have to deliver a new one to the player

    if (bBlowUp) {
        if (iType == TRIKE || iType == RAIDER || iType == QUAD) {
            // play quick 'boom' sound and show animation
            PARTICLE_CREATE(iDieX, iDieY, EXPLOSION_TRIKE, -1, -1);
            play_sound_id_with_distance(SOUND_TRIKEDIE, distanceBetweenCellAndCenterOfScreen(iCell));

            if (rnd(100) < 30) {
                PARTICLE_CREATE(iDieX, iDieY - 24, OBJECT_SMOKE, -1, -1);
            }
        }

        if (iType == SIEGETANK || iType == DEVASTATOR && rnd(100) < 25) {
            if (iBodyFacing == FACE_UPLEFT ||
                iBodyFacing == FACE_DOWNRIGHT) {
                PARTICLE_CREATE(iDieX, iDieY, OBJECT_SIEGEDIE, iPlayer, -1);
            }
        }

        if (iType == TANK || iType == SIEGETANK || iType == SONICTANK || iType == LAUNCHER || iType == DEVIATOR ||
            iType == HARVESTER || iType == ORNITHOPTER || iType == MCV || iType == FRIGATE) {
            // play quick 'boom' sound and show animation
            if (rnd(100) < 50) {
                PARTICLE_CREATE(iDieX, iDieY, EXPLOSION_TANK_ONE, -1, -1);
                play_sound_id_with_distance(SOUND_TANKDIE2, distanceBetweenCellAndCenterOfScreen(iCell));
            } else {
                PARTICLE_CREATE(iDieX, iDieY, EXPLOSION_TANK_TWO, -1, -1);
                play_sound_id_with_distance(SOUND_TANKDIE, distanceBetweenCellAndCenterOfScreen(iCell));
            }

            if (rnd(100) < 30)
                PARTICLE_CREATE(iDieX, iDieY - 24, OBJECT_SMOKE, -1, -1);

            if (iType == HARVESTER) {
                game.shakeScreen(25);
                mapEditor.createField(iCell, TERRAIN_SPICE, ((iCredits + 1) / 7));
            }

            if (iType == ORNITHOPTER) {
                PARTICLE_CREATE(iDieX, iDieY, EXPLOSION_ORNI, -1, -1);
            }

            // Frigate death particle? (doesnt exist in Dune 2, but would be cool to have)
        }

        if (iType == DEVASTATOR) {
            int iOrgDieX = iDieX;
            int iOrgDieY = iDieY;


            // create a cirlce of explosions (big ones)
            iDieX -= 32;
            iDieY -= 32;

            for (int cx = 0; cx < 3; cx++)
                for (int cy = 0; cy < 3; cy++) {


                    for (int i = 0; i < 2; i++)
                        PARTICLE_CREATE(iDieX + (cx * 32), iDieY + (cy * 32), EXPLOSION_STRUCTURE01 + rnd(2), -1, -1);

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

                        if (structure[id]->getHitPoints() > 0) {

                            int iDamage = 150 + rnd(100);
                            structure[id]->damage(iDamage);

                            int iChance = 10;

                            if (structure[id] &&
                                structure[id]->getHitPoints() < (structures[structure[id]->getType()].hp / 2)) {
                                iChance = 30;
                            }

                            if (rnd(100) < iChance) {
                                long x = pos_x() + (mapCamera->getViewportStartX()) + 16 + (-8 + rnd(16));
                                long y = pos_y() + (mapCamera->getViewportStartY()) + 16 + (-8 + rnd(16));
                                PARTICLE_CREATE(x, y, OBJECT_SMOKE, -1, -1);
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


            PARTICLE_CREATE(iOrgDieX, iOrgDieY, OBJECT_BOOM02, -1, -1);

            PARTICLE_CREATE(iOrgDieX - 32, iOrgDieY, OBJECT_BOOM02, -1, -1);
            PARTICLE_CREATE(iOrgDieX + 32, iOrgDieY, OBJECT_BOOM02, -1, -1);
            PARTICLE_CREATE(iOrgDieX, iOrgDieY - 32, OBJECT_BOOM02, -1, -1);
            PARTICLE_CREATE(iOrgDieX, iOrgDieY + 32, OBJECT_BOOM02, -1, -1);

        }

        if (iType == TROOPER || iType == SOLDIER || iType == UNIT_FREMEN_ONE) {
            // create particle of dead body

            PARTICLE_CREATE(iDieX, iDieY, OBJECT_DEADINF02, iPlayer, -1);

            play_sound_id_with_distance(SOUND_DIE01 + rnd(5), distanceBetweenCellAndCenterOfScreen(iCell));
        }

        if (iType == TROOPERS || iType == INFANTRY || iType == UNIT_FREMEN_THREE) {
            // create particle of dead body

            PARTICLE_CREATE(iDieX, iDieY, OBJECT_DEADINF01, iPlayer, -1);

            play_sound_id_with_distance(SOUND_DIE01 + rnd(5), distanceBetweenCellAndCenterOfScreen(iCell));
        }
    } // blow up
    else {


    }

    if (bSquish) {

        // when we do not 'blow up', we died by something else. Only infantry will be 'squished' here now.
        if (iType == SOLDIER || iType == TROOPER || iType == UNIT_FREMEN_ONE) {
            PARTICLE_CREATE(iDieX, iDieY, EXPLOSION_SQUISH01 + rnd(2), iPlayer, iFrame);
            play_sound_id_with_distance(SOUND_SQUISH, distanceBetweenCellAndCenterOfScreen(iCell));
        } else if (iType == TROOPERS || iType == INFANTRY || iType == UNIT_FREMEN_THREE) {
            PARTICLE_CREATE(iDieX, iDieY, EXPLOSION_SQUISH03, iPlayer, iFrame);
            play_sound_id_with_distance(SOUND_SQUISH, distanceBetweenCellAndCenterOfScreen(iCell));
        }

    }

    // NOW IT IS FREE FOR USAGE AGAIN

    if (iStructureID > -1) {
        cAbstractStructure *pStructure = structure[iStructureID];
        if (pStructure && pStructure->isValid()) {
            pStructure->setAnimating(false);
        }
    }

    // Anyone who was attacking this unit is on guard
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue; // skip invalid
        if (i == iID) continue; // skip self
        if (cUnit.iAttackUnit != i) continue; // skip those who did not want to attack me
        cUnit.iAttackUnit = -1;
        cUnit.iGoalCell = cUnit.iCell;
        cUnit.iAction = ACTION_GUARD;

        // Ai will still move to this location
        log("Another move to");
        cUnit.move_to(iCell, -1, -1);
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

    game.onNotify(event);

    init(iID);    // re-init

    for (int i = 0; i < MAPID_MAX; i++) {
        if (i != MAPID_STRUCTURES) {
            map.remove_id(iID, i);
        }
    }
}


/**
 * Returns true when it belongs to a player and is alive. If it is dead it will return false.
 * If it has any invalid cell (which should never happen really) then it will return false as well.
 * @return
 */
bool cUnit::isValid() {
    if (iPlayer < 0)
        return false;

    // no hitpoints (dead) and not in a structure
    if (iHitPoints < 0 && iTempHitPoints < 0)
        return false;

    if (iCell < 0 || iCell >= map.getMaxCells())
        return false;

    return true;
}

int cUnit::pos_x() {
//    return (iCellX * TILESIZE_WIDTH_PIXELS) + iOffsetX;
    return posX;
}

int cUnit::pos_y() {
//    return (iCellY * TILESIZE_HEIGHT_PIXELS) + iOffsetY;
    return posY;
}

int cUnit::draw_x() {
    int bmpOffset = (TILESIZE_WIDTH_PIXELS - getBmpWidth()) / 2;
    return mapCamera->getWindowXPositionWithOffset(pos_x(), bmpOffset);
}

int cUnit::center_draw_x() {
    return draw_x() + (mapCamera->factorZoomLevel(getBmpWidth()) / 2);
}

int cUnit::center_draw_y() {
    return draw_y() + (mapCamera->factorZoomLevel(getBmpHeight()) / 2);
}

int cUnit::draw_y() {
    int bmpOffset = (TILESIZE_HEIGHT_PIXELS - getBmpHeight()) / 2;
    return mapCamera->getWindowYPositionWithOffset(pos_y(), bmpOffset);
}

int cUnit::getBmpHeight() const {
    return unitInfo[iType].bmp_height;
}

void cUnit::draw_spice() {
    float width_x = mapCamera->factorZoomLevel(getBmpWidth());
    int height_y = mapCamera->factorZoomLevel(4);
    int drawx = draw_x();
    int drawy = draw_y() - ((height_y * 2) + 2);

    int max = getUnitType().credit_capacity;
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
    return unitInfo[iType].bmp_width;
}

float cUnit::getHealthNormalized() {
    s_UnitP &unitType = getUnitType();
    float flMAX = unitType.hp;
    return (iHitPoints / flMAX);
}

void cUnit::draw_health() {
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

void cUnit::draw_path() {
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

    int ux = draw_x();
    int uy = draw_y();

    if (iType == SANDWORM) {
        Shimmer(20, ux + (mapCamera->getZoomedHalfTileSize()), uy + (mapCamera->getZoomedHalfTileSize()));
        return;
    }

    s_UnitP &unitType = getUnitType();
    int bmp_width = unitType.bmp_width;
    int bmp_height = unitType.bmp_height;

    // the multiplier we will use to draw the unit
    int bmp_head = convert_angle(iHeadFacing);
    int bmp_body = convert_angle(iBodyFacing);

    int start_x, start_y;

    // draw body first
    start_x = bmp_body * bmp_width;
    start_y = bmp_height * iFrame;

    // Selection box x, y position. Depends on unit size
    int iSelX = ux;
    int iSelY = uy;

    cPlayer &cPlayer = players[this->iPlayer];

    // Draw SHADOW
    BITMAP *shadow = cPlayer.getUnitShadowBitmap(iType, bmp_body, iFrame);
    float scaledWidth = mapCamera->factorZoomLevel(bmp_width);
    float scaledHeight = mapCamera->factorZoomLevel(bmp_height);

    if (shadow) {
        int destY = uy;

        if (iType == CARRYALL) {
            // adjust X and Y so it looks like a carry-all is 'higher up in the air'
            destY = uy + 24; // TODO; do something with height here? the closer to target, the less distant the shadow?
        }

        allegroDrawer->maskedStretchBlit(shadow, bmp_screen, 0, 0, bmp_width, bmp_height,
                                         ux, destY,
                                         scaledWidth, scaledHeight);
        destroy_bitmap(shadow);
    }

    // Draw BODY
    BITMAP *bitmap = cPlayer.getUnitBitmap(iType);
    if (bitmap) {
        allegroDrawer->maskedStretchBlit(bitmap, bmp_screen, start_x, start_y, bmp_width, bmp_height,
                                         ux, uy,
                                         scaledWidth,
                                         scaledHeight);
    } else {
        char msg[255];
        sprintf(msg, "unit of iType [%d] did not have a bitmap!?", iType);
        log(msg);
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
//       draw_sprite(bmp_screen, (BITMAP *)gfxdata[FOCUS].dat, iSelX/*+startpixel*/-2, iSelY);
        BITMAP *focusBitmap = (BITMAP *) gfxdata[FOCUS].dat;
        int bmp_width = focusBitmap->w;
        int bmp_height = focusBitmap->h;
        allegroDrawer->maskedStretchBlit(focusBitmap, bmp_screen, 0, 0, bmp_width, bmp_height, iSelX/*+startpixel*/- 2,
                                         iSelY, mapCamera->factorZoomLevel(bmp_width),
                                         mapCamera->factorZoomLevel(bmp_height));
    }


}

// TODO: only do this when iCell is updated
void cUnit::updateCellXAndY() {
    iCellX = map.getCellX(iCell);
    iCellY = map.getCellY(iCell);
}

void cUnit::attackUnit(int targetUnit) {
    char msg[255];
    sprintf(msg, "attackUnit() : target is [%d]", targetUnit);
    log(msg);
    attack(unit[targetUnit].iCell, targetUnit, -1, -1);
}

void cUnit::attackStructure(int targetStructure) {
    char msg[255];
    sprintf(msg, "attackStructure() : target is [%d]", targetStructure);
    log(msg);
    attack(structure[targetStructure]->getCell(), -1, targetStructure, -1);
}

void cUnit::attackCell(int cell) {
    char msg[255];
    sprintf(msg, "attackCell() : cell target is [%d]", cell);
    log(msg);
    attack(cell, -1, -1, -1);
}

void cUnit::attack(int iGoalCell, int iUnit, int iStructure, int iAttackCell) {
    // basically the same as move, but since we use iAction as ATTACK
    // it will think first in attack mode, determining if it will be CHASE now or not.
    // if not, it will just fire.

    char msg[255];
    sprintf(msg, "Attacking UNIT ID [%d], STRUCTURE ID [%d], ATTACKCLL [%d], GoalCell [%d]", iUnit, iStructure,
            iAttackCell, iGoalCell);
    log(msg);

    if (iUnit < 0 && iStructure < 0 && iAttackCell < 0) {
        log("What is this? Ordered to attack but no target?");
        return;
    }

    // TODO: We have somewhere else something with "intents", so this whole if statement should be removed / replaced?
    if (isSaboteur()) {
        // saboteur does not attack, but only captures
        move_to(iGoalCell, iStructure, -1, eUnitActionIntent::INTENT_CAPTURE);
        return;
    }

    iAction = ACTION_ATTACK;
    iGoalCell = iGoalCell;
    iAttackStructure = iStructure;
    iAttackUnit = iUnit;
    iAttackCell = iAttackCell;
    forgetAboutCurrentPathAndPrepareToCreateNewOne(rnd(5));
}

void cUnit::attackAt(int cell) {
    char msg[255];
    sprintf(msg, "attackAt() : cell target is [%d]", cell);
    log(msg);

    if (!map.isWithinBoundaries(cell)) {
        log("attackAt() : Invalid cell, aborting");
        return;
    }

    int unitId = map.getCellIdUnitLayer(cell);
    int structureId = map.getCellIdStructuresLayer(cell);
    sprintf(msg, "attackAt() : cell target is [%d], structureId [%d], unitId [%d]", cell, structureId, unitId);
    log(msg);

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

void cUnit::move_to(int iCll, int iStrucID, int iUnitID) {
    move_to(iCll, iStrucID, iUnitID, eUnitActionIntent::INTENT_MOVE);
}

void cUnit::move_to(int iCll, int iStrucID, int iUnitID, eUnitActionIntent intent) {
    char msg[255];
    sprintf(msg, "(move_to - START) : to cell [%d], iStrucID[%d], unitId[%d] (to attack, if > -1), intent[%s]", iCll, iStrucID, iUnitID, eUnitActionIntentString(intent));
    log(msg);
    iGoalCell = iCll;
    iStructureID = iStrucID;
    this->iUnitID = iUnitID;
    iAttackStructure = -1;
    iAttackCell = -1;

    // only when not moving (half on tile) reset nextcell
    if (!isMovingBetweenCells()) {
        iNextCell = -1;
    }

    iAction = ACTION_MOVE;
    this->intent = intent;

    forgetAboutCurrentPathAndPrepareToCreateNewOne(rnd(5));

    if (iStrucID > -1) {
        if (structure[iStrucID]) {
            structure[iStrucID]->iUnitID = iUnitID;
        }
    }

    log("(move_to - FINISHED)");
}


void cUnit::think_guard() {
    if (!isValid()) {
        return;
    }

    if (isAirbornUnit()) {
        iAction = ACTION_MOVE; // fly around man
        return;
    }

    if (isHarvester()) {
        iAction = ACTION_MOVE;
        return;
    }

    if (isSaboteur()) {
        return; // do nothing
    }

    TIMER_bored++; // we are bored ow yeah
    TIMER_guard++; // scan time

    if (TIMER_guard > 5) {
        // scan area
        TIMER_guard = 0 - (rnd(5)); // do not scan all at the same time

        updateCellXAndY();
        // scan
        int iDistance = 9999;
        int unitIdSelectedForAttacking = -1;

        // non airborn units thinking
        if (isSandworm()) {
            if (TIMER_wormeat > 0) {
                TIMER_wormeat--;
                return; // get back, not hungry just yet
            }

            for (int i = 0; i < MAX_UNITS; i++) {
                if (unit[i].isValid() && i != iID) {
                    // not ours and its visible
                    if (unit[i].iPlayer != iPlayer &&
                        !unit[i].isAirbornUnit()) {
                        int cellType = map.getCellType(unit[i].iCell);
                        if (cellType == TERRAIN_SAND ||
                            cellType == TERRAIN_HILL ||
                            cellType == TERRAIN_SPICE ||
                            cellType == TERRAIN_SPICEHILL) {
                            int distance = ABS_length(iCellX, iCellY, unit[i].iCellX, unit[i].iCellY);

                            if (distance <= unitInfo[iType].sight && distance < iDistance) {
                                // ATTACK
                                iDistance = distance;
                                unitIdSelectedForAttacking = i;
                                // log("WORM FOUND ENEMY");
                            }
                        } // valid terrain
                    }

                }
            }

        } else // not sandworm
        {
            int airUnitToAttack = -1;
            for (int i = 0; i < MAX_UNITS; i++) {
                if (i == iID) continue; // skip self
                cUnit &potentialThreath = unit[i];
                if (!potentialThreath.isValid()) continue;
                if (potentialThreath.getPlayerId() == getPlayerId()) continue; // skip own units

                bool bSameTeam = getPlayer()->isSameTeamAs(potentialThreath.getPlayer());
                if (bSameTeam) continue; // skip same team players / allies

                if (!map.isVisible(potentialThreath.iCell, iPlayer)) continue; // skip non-visible potential enemy units

                if (potentialThreath.isAirbornUnit() && // potential threat is air unit
                    !isAirbornUnit() &&                 // but I am not
                    !canAttackAirUnits())               // and I can't attack air units...
                    continue;                           // then bail

                int distance = ABS_length(iCellX, iCellY, potentialThreath.iCellX, potentialThreath.iCellY);

                // TODO: perhaps make this configurable, so you can set the 'aggressiveness' of units?
                int range = unitInfo[iType].sight + 3; // do react earlier than already in range.

                if (distance <= range && distance < iDistance) {
                    // ATTACK
                    iDistance = distance;
                    unitIdSelectedForAttacking = i;
                    // I am not air unit, enemy is.
                    if (!isAirbornUnit() && canAttackAirUnits() && potentialThreath.isAirbornUnit()) {
                        airUnitToAttack = i;
                    }
                }
            }

            if (airUnitToAttack > -1) {
                // air units override ground units for units that can attack air units ?
                unitIdSelectedForAttacking = airUnitToAttack;
            }

        }

        if (unitIdSelectedForAttacking > -1) {
            cUnit &unitToAttack = unit[unitIdSelectedForAttacking];

            if (unitToAttack.isValid()) {
                s_GameEvent event{
                        .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                        .entityType = eBuildType::UNIT,
                        .entityID = unitToAttack.iID,
                        .player = getPlayer(),
                        .entitySpecificType = unitToAttack.getType(),
                        .atCell = unitToAttack.iCell
                };

                game.onNotify(event);
            }

            if (!getPlayer()->isHuman()) {
                if (unitToAttack.isInfantryUnit() && canSquishInfantry()) {
                    // AI will try to squish infantry units
                    move_to(unitToAttack.iCell);
                } else {
                    attackUnit(unitIdSelectedForAttacking);
                }
            } else {
                attackUnit(unitIdSelectedForAttacking);
            }

            if (game.iMusicType == MUSIC_PEACE && iType != SANDWORM && iPlayer == HUMAN) {
                playMusicByType(MUSIC_ATTACK);

                // warning... bla bla
                if (unitToAttack.iType == SANDWORM)
                    play_voice(SOUND_VOICE_10_ATR); // wormsign
                else
                    play_voice(SOUND_VOICE_09_ATR); // enemy unit approaching
            }
            return;
        } else {
            int structureIdSelectedForAttacking = -1;

            if (!isSandworm() && iPlayer > HUMAN) {
                // nothing found yet to attack;
                // ai units will auto-attack structures nearby

                for (int i = 0; i < MAX_STRUCTURES; i++) {
                    cAbstractStructure *pStructure = structure[i];
                    if (pStructure && i != iID) {
                        bool bAlly = getPlayer()->isSameTeamAs(pStructure->getPlayer());

                        // not ours and its visible
                        if (map.isVisible(pStructure->getCell(), iPlayer) &&
                            !bAlly) {
                            int c = pStructure->getCell();
                            int c1 = pStructure->getCell();
                            int distance = ABS_length(iCellX, iCellY, map.getCellX(c1),
                                                      map.getCellY(c));

                            int sight = getUnitType().sight + 3;

                            if (distance <= sight && distance < iDistance) {
                                // ATTACK
                                iDistance = distance;
                                structureIdSelectedForAttacking = i;
                            }
                        }

                    }
                }
            }

            if (structureIdSelectedForAttacking > -1) {
                attackStructure(structureIdSelectedForAttacking);

                // !?!?
                if (game.iMusicType == MUSIC_PEACE && iType != SANDWORM && iPlayer == 0) {
                    playMusicByType(MUSIC_ATTACK);
                }
            }
        }

    }


    if (TIMER_bored > 3500) {
        TIMER_bored = 0;
        iBodyShouldFace = rnd(8);
        iHeadShouldFace = rnd(8);
    }

    // When bored we turn our body and head sometimes, so do it here:

}

bool cUnit::isSandworm() const {
    return iType == SANDWORM;
}

// NORMAL thinking
void cUnit::think() {
    if (TIMER_blink > 0)
        TIMER_blink--;

    cPlayer *pPlayer = getPlayer();
    if (iType == MCV) {
        if (pPlayer->isHuman()) {
            if (bSelected) {
                if (key[KEY_D]) {
                    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();

                    bool result = getPlayer()->canPlaceStructureAt(iCell, CONSTYARD, iID).success;

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

    // HEAD is not facing correctly
    if (!isAirbornUnit()) {
        if (iBodyFacing == iBodyShouldFace) {
            if (iHeadFacing != iHeadShouldFace) {
                TIMER_turn++;
                if (TIMER_turn > (unitInfo[iType].turnspeed)) {
                    TIMER_turn = 0;

                    iHeadFacing = determineNewFacing(iHeadFacing, iHeadShouldFace);

//                    int d = 1;
//
//                    int toleft = (iHeadFacing + 8) - iHeadShouldFace;
//                    if (toleft > 7) toleft -= 8;
//
//                    int toright = abs(toleft - 8);
//
//                    if (toright == toleft) d = -1 + (rnd(2));
//                    if (toleft > toright) d = 1;
//                    if (toright > toleft) d = -1;
//
//                    iHeadFacing += d;
//
//                    if (iHeadFacing < 0)
//                        iHeadFacing = 7;
//
//                    if (iHeadFacing > 7)
//                        iHeadFacing = 0;


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

    // when any unit is on a spice bloom, you got a problem, you die!
    int cellType = map.getCellType(iCell);
    if (cellType == TERRAIN_BLOOM
        && unitInfo[iType].airborn == false) {
        // change type of terrain to sand
        mapEditor.createCell(iCell, TERRAIN_SAND, 0);

        mapEditor.createField(iCell, TERRAIN_SPICE, 25 + (rnd(50)));

        // kill unit
        map.remove_id(iID, MAPID_UNITS);

        die(true, false);
        game.shakeScreen(20);
        return;
    }


    // --- think
    if (iType == ORNITHOPTER) {
        think_ornithopter(pPlayer);
        return;
    }

    // HARVESTERs logic here
    int idOfStructureAtCell = map.getCellIdStructuresLayer(iCell);

    if (iType == HARVESTER) {
        bool bFindRefinery = false;

        if (iCredits > 0 && bSelected && key[KEY_D]) {
            bFindRefinery = true;
        }

        // cell = goal cell (doing nothing)
        if (iCell == iGoalCell) {
            // when on spice, harvest
            if (cellType == TERRAIN_SPICE ||
                cellType == TERRAIN_SPICEHILL) {
                // do timer stuff
                if (iCredits < unitInfo[iType].credit_capacity)
                    TIMER_harvest++;
            } else {
                // not on spice, find a new location
                if (iCredits < unitInfo[iType].credit_capacity) {
                    // find harvest cell
                    move_to(UNIT_find_harvest_spot(iID), -1, -1);
                } else {
                    iFrame = 0;
                    bFindRefinery = true;
                    // find a refinery
                }
            }

            if (iCredits >= unitInfo[iType].credit_capacity)
                bFindRefinery = true;

            // when we should harvest...
            cPlayerDifficultySettings *difficultySettings = players[iPlayer].getDifficultySettings();
            if (TIMER_harvest > (difficultySettings->getHarvestSpeed(unitInfo[iType].harvesting_speed)) &&
                iCredits < unitInfo[iType].credit_capacity) {
                TIMER_harvest = 1;

                iFrame++;

                if (iFrame > 3)
                    iFrame = 1;

                iCredits += unitInfo[iType].harvesting_amount;
                map.cellTakeCredits(iCell, unitInfo[iType].harvesting_amount);

                // turn into sand/spice (when spicehill)
                if (map.getCellCredits(iCell) <= 0) {
                    if (cellType == TERRAIN_SPICEHILL) {
                        map.cellChangeType(iCell, TERRAIN_SPICE);
                        map.cellGiveCredits(iCell, rnd(100));
                    } else {
                        map.cellChangeType(iCell, TERRAIN_SAND);
                        map.cellChangeTile(iCell, 0);
                    }

                    // create new path to this thingy
                    //iGoalCell =  UNIT_find_harvest_spot(iID);
                    //iPathIndex=-1; // create new path

                    move_to(UNIT_find_harvest_spot(iID), -1, -1);

                    mapEditor.smoothAroundCell(iCell);
                }
            }


            // refinery required, go find one that is available
            if (bFindRefinery) {
                iFrame = 0;
                char msg[255];
                sprintf(msg, "Going to look for a refinery, playerId [%d], cell [%d]", iPlayer, iCell);
                log(msg);
                int refineryStructureId = structureUtils.findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(
                        iCell, REFINERY, &players[iPlayer]);

                if (refineryStructureId < 0) {
                    // none found, wait
                    TIMER_thinkwait = 10;
                    return;
                }

                // found a refinery, lets check if we can find a carry-all that will bring us
                cAbstractStructure *refinery = structure[refineryStructureId];
                refinery->setAnimating(true);

                // how? carry-all or ride?
                int r = CARRYALL_TRANSFER(iID, refinery->getCell() + 2);

                iStructureID = refineryStructureId;

                if (r < 0) {
                    char msg[255];
                    sprintf(msg, "Returning to refinery ID %d", refineryStructureId);
                    log(msg);
                    move_to(refinery->getCell() + rnd(2) + (rnd(2) * map.getWidth()), refineryStructureId, -1, INTENT_UNLOAD_SPICE); // move yourself...
                    TIMER_movewait = 0;
                } else {
                    TIMER_movewait = 500; // wait for pickup!
                    TIMER_thinkwait = 500;
                }
            }

        } else {
            // ??
            iFrame = 0;
        }
    }

    // When this is a carry-all, show proper animation when filled
    if (iType == CARRYALL) {
        think_carryAll();
    }
}

void cUnit::think_carryAll() {// A carry-all has something when:
// - it carries a unit (iUnitID > -1)
// - it has the flag TRANSFER_NEW_

    if ((iTransferType == TRANSFER_NEW_STAY ||
         iTransferType == TRANSFER_NEW_LEAVE ||
         iTransferType == TRANSFER_PICKUP) || iUnitID > -1) {

        // when picking up a unit.. only draw when picked up
        if (iTransferType == TRANSFER_PICKUP && bPickedUp)
            iFrame = 1;

        // any other transfer, means it is filled from start...
        if (iTransferType != TRANSFER_PICKUP)
            iFrame = 1;
    } else {
        iFrame = 0;
    }
}

void cUnit::think_ornithopter(cPlayer *pPlayer) {// flap with your wings
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

    float turnspeed = unitInfo[iType].turnspeed;
    if (isAirbornUnit()) {
        // when closer to goal, turnspeed decreases.
        double distance = map.distance(iCell, iGoalCell);
        int distanceInCells = 6;
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
void cUnit::think_move_air() {
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
        if (iTransferType == TRANSFER_DIE) {
            // kill (probably reached border or something)
            die(false, false);
            return;
        }

        // transfer wants to pickup and drop a unit...
        if (iTransferType == TRANSFER_PICKUP) {
            if (iUnitID > -1) {
                // Not yet picked up the unit
                cUnit &unitToPickupOrDrop = unit[iUnitID];
                if (!bPickedUp) {
                    if (!unitToPickupOrDrop.isValid()) {
                        iTransferType = TRANSFER_NONE; // nope...
                        return;
                    }

                    // I believe this statement is always true, as we do not set flag
                    // on unit that is being picked up
                    if (unitToPickupOrDrop.bPickedUp == false) {
                        // check where the unit is:
                        int iTheGoal = unitToPickupOrDrop.iCell;

                        if (iCell == iTheGoal) {
                            // when this unit is NOT moving
                            if (!unitToPickupOrDrop.isMovingBetweenCells()) {
                                TIMER_movedelay = 300; // this will make carry-all speed up slowly

                                bPickedUp = true; // set state in aircraft, that it has picked up a unit

                                // so we set the tempHitpoints so the unit 'dissapears' from the map without being
                                // really dead.
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

                                PARTICLE_CREATE(pufX, pufY, OBJECT_CARRYPUFF, -1, -1);

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
                                // forget about this
                                iGoalCell = iCell;
                                iTransferType = TRANSFER_NONE;
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

                            // dump it here
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
                            map.clearShroud(iCell, unitToPickupOrDrop.getUnitType().sight, iPlayer);

                            int unitIdOfUnitThatHasBeenPickedUp = iUnitID;

                            iUnitID = -1;         // reset this
                            iTempHitPoints = -1; // reset this
                            iTransferType = TRANSFER_NONE; // done

                            // WHEN DUMPING A HARVESTER IN A REFINERY
                            // make it enter the refinery instantly
                            if (unitToPickupOrDrop.isHarvester() && unitToPickupOrDrop.iStructureID > -1) {
                                // valid structure
                                cAbstractStructure *structureUnitWantsToEnter = structure[unitToPickupOrDrop.iStructureID];
                                if (structureUnitWantsToEnter && structureUnitWantsToEnter->isValid()) {
                                    // when this structure is not occupied
                                    if (structureUnitWantsToEnter->iUnitID < 0) {
                                        // get in!
                                        structureUnitWantsToEnter->setAnimating(false);
                                        structureUnitWantsToEnter->iUnitID = unitIdOfUnitThatHasBeenPickedUp;  // !!
                                        structureUnitWantsToEnter->setFrame(0);

                                        // store this
                                        unitToPickupOrDrop.iTempHitPoints = unitToPickupOrDrop.iHitPoints;
                                        unitToPickupOrDrop.iHitPoints = -1; // 'kill' unit
                                        unitToPickupOrDrop.setCell(structureUnitWantsToEnter->getCell());
                                        unitToPickupOrDrop.updateCellXAndY();

                                        map.remove_id(unitIdOfUnitThatHasBeenPickedUp, MAPID_UNITS);
                                    } // enter..
                                }
                            }

                            int pufX = (pos_x() + getBmpWidth() / 2);
                            int pufY = (pos_y() + getBmpHeight() / 2);
                            PARTICLE_CREATE(pufX, pufY, OBJECT_CARRYPUFF, -1, -1);
                        } else {
                            if (DEBUGGING)
                                log("Could not dump here, searching other spot");

                            // find a new spot
                            updateCellXAndY();
                            int rx = (iCellX - 2) + rnd(5);
                            int ry = (iCellY - 2) + rnd(5);
                            FIX_BORDER_POS(rx, ry);

                            iGoalCell = map.getCellWithMapDimensions(rx, ry);

                            iBringTarget = iGoalCell;
                            return;
                        }
                    }
                }
            } else {
                iTransferType = TRANSFER_NONE; // unit is not valid?
                return;
            }
        }

        // transfer is to create a new unit
        if (iTransferType == TRANSFER_NEW_LEAVE ||
            iTransferType == TRANSFER_NEW_STAY) {
            // bring a new unit

            if (iType == FRIGATE) {
                int iStrucId = map.getCellIdStructuresLayer(iCell);

                if (iStrucId > -1) {
                    iGoalCell = iFindCloseBorderCell(iCell);
                    iTransferType = TRANSFER_DIE;

                    structure[iStrucId]->setFrame(4); // show package on this structure
                    structure[iStrucId]->setAnimating(true); // keep animating
                    ((cStarPort *) structure[iStrucId])->setFrigateDroppedPackage(true);
                } else {
                    int closestStarport = structureUtils.findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(
                            iCell, STARPORT, &players[iPlayer]);

                    // find closest Starport to deliver next (Starport got mid-way destroyed)
                    if (closestStarport < 0) {
                        // not found, die
                        die(true, false);
                    } else {
                        iGoalCell = structure[closestStarport]->getCell();
                        structure[closestStarport]->setAnimating(true); // start animating
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

                    map.cellSetIdForLayer(iCell, MAPID_UNITS, id);
                }

                // now make sure this carry-all will not be drawn as having a unit:
                iNewUnitType = -1;

                // depending on transfertype...
                if (iTransferType == TRANSFER_NEW_LEAVE) {
                    iTransferType = TRANSFER_DIE;

                    // find a new border cell close to us... to die
                    iGoalCell = iFindCloseBorderCell(iCell);
                    return;
                } else if (iTransferType == TRANSFER_NEW_STAY) {
                    // reset transfertype:
                    iTransferType = TRANSFER_NONE;
                    return;
                }

            } else {
                // find a new spot for delivery
                updateCellXAndY();
                int rx = (iCellX - 4) + rnd(7);
                int ry = (iCellY - 4) + rnd(7);
                FIX_BORDER_POS(rx, ry);

                iGoalCell = map.getCellWithMapDimensions(rx, ry);
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
    if (iUnitID > -1 || (iTransferType != TRANSFER_DIE && iTransferType != TRANSFER_NONE)) {
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
                        PARTICLE_CREATE(pufX, pufY, OBJECT_CARRYPUFF, -1, -1);
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

//    int movespeed = getUnitType().speed;
    int movespeed = 2; // 2 pixels, the actual 'speed' is done by the delay above using TIMER_move! :/
    posX += cos(angle) * movespeed;
    posY += sin(angle) * movespeed;

    iCell = mapCamera->getCellFromAbsolutePosition(posX, posY);

    updateCellXAndY();
    map.cellSetIdForLayer(iCell, MAPID_AIR, iID);
}

// Carryall-order
//
// Purpose:
// Order a carryall to pickup a unit, or send a new unit (depending on iTransfer)
//
void cUnit::carryall_order(int iuID, int iTransfer, int iBring, int iTpe) {
    if (iTransferType > -1)
        return; // we cannot do multiple things at a time!!

    if (iTransfer == TRANSFER_NEW_STAY || iTransfer == TRANSFER_NEW_LEAVE) {

        // bring a new unit, depending on the iTransfer the carryall who brings this will be
        // removed after he brought the unit...

        // when iTranfer is 0 or 2, the unit is just created by REINFORCE() and this function
        // sets the target and such.

        iTransferType = iTransfer;

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
    } else if (iTransfer == TRANSFER_PICKUP && iuID > -1) {

        // the carryall must pickup the unit, and then bring it to the iBring stuff
        if (unit[iuID].isValid()) {
            iTransferType = iTransfer;

            iGoalCell = unit[iuID].iCell; // first go to the target to pick it up
            iCarryTarget = unit[iuID].iCell; // same here...

            iNewUnitType = -1;

            // where to bring it to
            iBringTarget = iBring; // bring

            bPickedUp = false;

            // what unit do we carry? (in case the unit moves away)
            iUnitID = iuID;
        }
    }
}

void cUnit::shoot(int iShootCell) {
    // particles are rendered at the center, so do it here as well
    int iShootX = pos_x() + (getBmpWidth() / 2);
    int iShootY = pos_y() + (getBmpHeight() / 2);
    int bmp_head = convert_angle(iHeadFacing);

    if (iType == TANK) {
        PARTICLE_CREATE(iShootX, iShootY, OBJECT_TANKSHOOT, -1, bmp_head);
    } else if (iType == SIEGETANK) {
        PARTICLE_CREATE(iShootX, iShootY, OBJECT_SIEGESHOOT, -1, bmp_head);
    }

    int bulletType = unitInfo[iType].bulletType;
    if (bulletType < 0) return; // no bullet type to spawn

    int iBull = create_bullet(bulletType, iCell, iShootCell, iID, -1);

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
        bool unitWhoShotMeIsInfantry = unitWhoShotMe.isInfantryUnit();
        bool unitWhoShotMeIsAirborn = unitWhoShotMe.isAirbornUnit();

        // only act when we are doing 'nothing'...
        if (iAction == ACTION_GUARD) {
            // only auto attack back when it is not an airborn unit
            // note: guard state already takes care of scanning for air units and attacking them
            if (!unitWhoShotMeIsAirborn) {
                attackUnit(iShotUnit);
            }
        }

        if (iPlayer > HUMAN) {
            if (isHarvester()) {
                if (unit[iShotUnit].isInfantryUnit() && !isMovingBetweenCells()) {
                    // this harvester will try to run over the infantry that attacks it
                    move_to(unit[iShotUnit].iCell);
                } else {
                    // under attack, retreat to base? find nearby units to help out?
                }

            } else {
                if (iAction != ACTION_ATTACK) {
                    if (canSquishInfantry() && unitWhoShotMeIsInfantry) {
                        // AI tries to run over infantry units that attack it
                        move_to(unit[iShotUnit].iCell);
                    } else {
                        // else simply shoot it
                        if (!unitWhoShotMeIsAirborn) {
                            attackUnit(iShotUnit);
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
                            unitInfo[iType].range) {
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
    }

    // for infantry , infantry turns into soldier and troopers into trooper when on 50% damage.
    if (isInfantryUnit()) {
        if (iType == INFANTRY || iType == TROOPERS) {
            // turn into soldier or trooper when on 50% health
            if (iHitPoints <= (unitInfo[iType].hp / 3)) {
                // leave 2 dead bodies (of 3 ;))

                // turn into single one
                if (iType == INFANTRY)
                    iType = SOLDIER;

                if (iType == TROOPERS)
                    iType = TROOPER;

                iHitPoints = unitInfo[iType].hp;

                int half = 16;
                int iDieX = pos_x() + half;
                int iDieY = pos_y() + half;

                PARTICLE_CREATE(iDieX, iDieY, OBJECT_DEADINF01, iPlayer, -1);

                play_sound_id_with_distance(SOUND_DIE01 + rnd(5),
                                            distanceBetweenCellAndCenterOfScreen(iCell));

            }
        }
    }
}

void cUnit::log(const char *txt) const {
    // logs unit stuff, but gives unit information
    char msg[512];
    sprintf(msg, "[UNIT[%d]: type = %d(=%s), iCell = %d, iGoalCell = %d] '%s'", iID, iType, unitInfo[iType].name, iCell, iGoalCell, txt);
    players[iPlayer].log(msg);
}

void cUnit::think_attack() {
    updateCellXAndY();

    cUnit * attackUnit = nullptr;
    if (iAttackUnit > -1) {
        attackUnit = &unit[iAttackUnit];
        if (attackUnit && !attackUnit->isValid() || attackUnit->isDead()) {
            iAttackUnit = -1;
            iAction = ACTION_GUARD;
            return;
        }
    }

    if (iType == SANDWORM) {
        if (iAttackUnit > -1) {
            iGoalCell = attackUnit->iCell;
            if (iGoalCell == iCell) {
                // eat
                attackUnit->die(false, false);
                int half = 16;
                int iParX = pos_x() + half;
                int iParY = pos_y() + half;

                PARTICLE_CREATE(iParX, iParY, OBJECT_WORMEAT, -1, -1);
                play_sound_id_with_distance(SOUND_WORM, distanceBetweenCellAndCenterOfScreen(iCell));
                iAction = ACTION_GUARD;
                iAttackUnit = -1;
                forgetAboutCurrentPathAndPrepareToCreateNewOne();
                TIMER_wormeat += rnd(150);
                return;
            } else {
                int cellType = map.getCellType(attackUnit->iCell);
                if (cellType != TERRAIN_SAND &&
                    cellType != TERRAIN_HILL &&
                    cellType != TERRAIN_SPICE &&
                    cellType != TERRAIN_SPICEHILL) {

                    iAction = ACTION_GUARD;
                    iAttackUnit = -1;
                } else {
                    iAction = ACTION_CHASE;
                    forgetAboutCurrentPathAndPrepareToCreateNewOne();
                }
            }
        } else {
            iAction = ACTION_GUARD;
        }
        return;
    }

    // make sure the goalcell is correct
    if (iAttackUnit > -1) {
        iGoalCell = attackUnit->iCell;
    }

    cAbstractStructure *pStructure = nullptr;
    if (iAttackStructure > -1) {
        pStructure = structure[iAttackStructure];
    }

    if (iAttackStructure > -1) {
        if (pStructure && pStructure->isValid()) {
            iGoalCell = pStructure->getCell();
        } else {
            iAttackUnit = -1;
            iAttackStructure = -1;
            iGoalCell = iCell;
            iAction = ACTION_GUARD;
            return;
        }

    }

    if (iAttackCell > -1) {
        iGoalCell = iAttackCell;
    }

    if (iAttackUnit > -1) {
        if (attackUnit->isDead()) {
            iAttackUnit = -1;
            iGoalCell = iCell;
            iAction = ACTION_GUARD;
            log("Destroyed unit target");
            return;
        }
    }

    if (iAttackStructure > -1 && pStructure) {
        if (pStructure->isDead()) {
            iAttackStructure = -1;
            iGoalCell = iCell;
            iAction = ACTION_GUARD;
            log("Destroyed structure target");
            return;
        }
    }

    if (iAttackCell > -1) {
        if (map.getCellType(iAttackCell) == TERRAIN_BLOOM) {
            // this is ok
        } else {
            if (map.getCellHealth(iAttackCell) < 0) {
                // it is destroyed
                iAttackCell = -1;
                iGoalCell = iCell;
                iAction = ACTION_GUARD;
            }
        }
    }

    int iDestX = map.getCellX(iGoalCell);
    int iDestY = map.getCellY(iGoalCell);

    // Distance check
    int distance = ABS_length(iCellX, iCellY, iDestX, iDestY);

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
        s_UnitP &unitType = getUnitType();

        // NON AIRBORN UNITS ATTACK THINKING
        int minDistance = 2;

        if (distance > minDistance && distance <= getRange()) {
            // when this function returns true, it is done firing bullets
            if (setAngleTowardsTargetAndFireBullets(distance)) {
                int randomCellFrom = map.getRandomCellFrom(iGoalCell, 16);
                int rx = map.getCellX(randomCellFrom);
                int ry = map.getCellY(randomCellFrom);

                iAttackUnit = -1;
                iAttackStructure = -1;
                iAction = ACTION_MOVE;
                iGoalCell = map.getCellWithMapDimensions(rx, ry);
            }
        } else {
            // stop attacking, move instead?
            iAction = ACTION_MOVE;
            iAttackUnit = -1;
            iAttackStructure = -1;
        }
    }
}

int cUnit::getFaceAngleToCell(int cell) const {
    int d = fDegrees(iCellX, iCellY, map.getCellX(cell), map.getCellY(cell));
    return face_angle(d); // get the angle
}

void cUnit::startChasingTarget() {
    if (iAttackStructure > -1) {
        iAction = ACTION_CHASE;
        // a structure does not move, so don't need to re-calculate path?
//        forgetAboutCurrentPathAndPrepareToCreateNewOne();
    } else if (iAttackUnit > -1) {
        cUnit * attackUnit = &unit[iAttackUnit];
        // chase unit, but only when ground unit
        if (!attackUnit->isSandworm() && !attackUnit->isAirbornUnit()) {
            iAction = ACTION_CHASE;
            // only think of new path when our target moved
            if (attackUnit->getCell() != iGoalCell) {
                forgetAboutCurrentPathAndPrepareToCreateNewOne();
            }
        } else {
            // do not chase sandworms or other air units, very ... inconvenient
            iAction = ACTION_GUARD;
            iGoalCell = iCell;
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }
    }
}

bool cUnit::setAngleTowardsTargetAndFireBullets(int distance) {
    s_UnitP &unitType = getUnitType();
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
    return getUnitType().range;
}

s_UnitP &cUnit::getUnitType() const {
    return unitInfo[iType];
}

// thinking about movement (which is called upon a faster rate)
void cUnit::think_move() {
    if (!isValid()) {
        assert(false && "Expected to have a valid unit calling think_move()");
    }

    // this is about non-aircraft only, so bail for aircraft units
    if (isAirbornUnit()) {
        return;
    }

    if (iTempHitPoints > -1) {
        return;
    }

    if (TIMER_movewait > 0) {
        TIMER_movewait--;
        return;
    }

    // when there is a valid goal cell (differs), then we go further
    if (iGoalCell == iCell) {
        iAction = ACTION_GUARD; // do nothing
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
                char msg[255];
                int iResult = CREATE_PATH(iID, 0); // do not take units into account yet
                sprintf(msg, "Create path ... result = %d", iResult);
                log(msg);

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

                        game.onNotify(event);

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
                        if (pStructure->iUnitID > -1 || // structure is occupied by unit
                            !pStructure->getPlayer()->isSameTeamAs(getPlayer())) { // or (no longer) of my team

                            // find alternative structure type nearby
                            int iNewID = structureUtils.findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(
                                    iCell,
                                    pStructure->getType(),
                                    getPlayer());

                            if (iNewID > -1 && iNewID != iStructureID) {
                                iStructureID = iNewID;
                                move_to(structure[iNewID]->getCell());
                            } else {
                                // stop moving (it is occupied!)
                                iNextCell = iCell;
                                iGoalCell = iCell;
                                TIMER_movewait = 100; // we wait
                                return;
                            }
                            bOccupied = true;
                        } else {  // structure is occupied by unit
                            // not occupied, continue!
                        }
                    } else if (intent == eUnitActionIntent::INTENT_CAPTURE) {
                        if (pStructure->getPlayer()->isSameTeamAs(getPlayer())) {
                            // stop!
                            iStructureID = -1;
                            iGoalCell = iCell;
                            iNextCell = iCell;
                            TIMER_movewait = 100; // we wait
                            bOccupied = true; // obviously
                            return;
                        }
                    }
                } else {
                    // structure is no longer valid, what now?
                    // .. not occupied?
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

    // TODO: this "think_move" thing should be abstracted somewhere, so that "occupied" becomes an abstraction
    // as well and thus this kind of hack can be prevented.
    if (iType == SANDWORM) {
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
                        if (!pStructure->hasUnitWithin()) {
                            // unit enters structure!
                            pStructure->setAnimating(false);
                            pStructure->iUnitID = iID;
                            pStructure->setFrame(0);

                            // store this
                            iTempHitPoints = iHitPoints;
                            iHitPoints = -1; // 'kill' unit

                            map.remove_id(iID, MAPID_UNITS);
                            setCell(pStructure->getCell());

                            log("-> Enter structure #3");
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
                            pStructure->damage(getUnitType().damageOnEnterStructure);
                            die(true, false);
                        } else {
                            if (pStructure->getHitPoints() < pStructure->getCaptureHP()) {
                                // make structure switch sides
                                pStructure->getsCapturedBy(getPlayer());
                                die(false, false);
                            } else {
                                // the unit will die and inflict damage
                                die(true, false);
                                pStructure->damage(getUnitType().damageOnEnterStructure);
                            }
                        }
                        return; // unit is dead, no need to go further
                    } else {
                        int i = 5;
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
        // note, no AIRBORN here (27/03/2021 - I guess this is because this method is not called by think_move_air())
        map.cellSetIdForLayer(iNextCell, MAPID_UNITS, iID);
    } else {
        map.cellSetIdForLayer(iNextCell, MAPID_WORMS, iID);

        // when sandworm, add particle stuff
        PARTICLE_CREATE(posX, posY, OBJECT_WORMTRAIL, -1, -1);
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
            if (bToLeft > -1 && bToDown < 0)
                PARTICLE_CREATE(iParX, iParY, TRACK_HOR, -1, -1);

            // vertical, when only going vertical
            if (bToDown > -1 && bToLeft < 0)
                PARTICLE_CREATE(iParX, iParY, TRACK_VER, -1, -1);

            // diagonal when going both ways
            if (bToDown > -1 && bToLeft > -1) {
                if (bToDown == 0) {
                    // going up
                    if (bToLeft == 1)
                        PARTICLE_CREATE(iParX, iParY, TRACK_DIA, -1, -1);
                    else
                        PARTICLE_CREATE(iParX, iParY, TRACK_DIA2, -1, -1);

                } else {
                    if (bToLeft == 0)
                        PARTICLE_CREATE(iParX, iParY, TRACK_DIA, -1, -1);
                    else
                        PARTICLE_CREATE(iParX, iParY, TRACK_DIA2, -1, -1);
                }

            }
        }
    }

    // movement in pixels
    if (bToLeft == 0)
        posX--;
    else if (bToLeft == 1)
        posX++;

    if (bToDown == 0)
        posY++;
    else if (bToDown == 1)
        posY--;

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
        if (iAction == ACTION_CHASE) {
            iAction = ACTION_ATTACK;
            // next time we think, will be checking for distance, etc
        }

        // movement to cell complete
        if (iType == SANDWORM) {
            map.cellResetIdFromLayer(iCell, MAPID_WORMS);
        } else {
            map.cellResetIdFromLayer(iCell, MAPID_UNITS);
        }

        setCell(iNextCell);
        iPathIndex++;
        iPathFails = 0; // we change this to 0... every cell

        // POLL now
        updateCellXAndY();

        // quick scan for infantry we squish
        // TODO: this can be sped up
        for (int iq = 0; iq < MAX_UNITS; iq++) {
            cUnit &potentialDeadUnit = unit[iq];
            if (!potentialDeadUnit.isValid()) continue;
            if (potentialDeadUnit.isSandworm()) continue; // sandworms cannot be squished
            if (!potentialDeadUnit.isInfantryUnit()) continue; // (TODO: units[unit[iq].iType].canBeSquished)
            if (potentialDeadUnit.iPlayer == iPlayer)
                continue; // can't squish own units (but we can squish allied units?)

            if (potentialDeadUnit.iCell == iCell) {
                if (potentialDeadUnit.isSaboteur()) {
                    takeDamage(potentialDeadUnit.getUnitType().damageOnEnterStructure);
                }

                // die
                potentialDeadUnit.die(false, true);
            }
        }

        if (iPlayer == AI_CPU5 && players[HUMAN].isHouse(ATREIDES)) {
            // TODO: make this work for all allied forces
            // hackish way to get Fog of war clearance by allied fremen units (super weapon).
            map.clearShroud(iCell, unitInfo[iType].sight, HUMAN);
        }

        map.clearShroud(iCell, unitInfo[iType].sight, iPlayer);

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

        TIMER_movewait = 2 + ((getUnitType().speed + iSlowDown) * 3);
        return eUnitMoveToCellResult::MOVERESULT_AT_CELL;
    }
    return eUnitMoveToCellResult::MOVERESULT_BUSY;
}

/**
 * Clears the created path, resets next-cell to current cell. Sets timer to wait to 100. So that
 * after that the unit will think of a new path to create.
 */
void cUnit::forgetAboutCurrentPathAndPrepareToCreateNewOne() {
    forgetAboutCurrentPathAndPrepareToCreateNewOne(100);
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

bool cUnit::isInfantryUnit() {
    return unitInfo[iType].infantry;
}

cUnit::cUnit() {
    dimensions = nullptr;
    mission = -1;
    init(-1);
}

cUnit::~cUnit() {
    if (dimensions) delete dimensions;
}

/**
 * Poor man solution to frequently update the dimensions of unit, better would be using events?
 * (onMove, onViewportMove, onViewportZoom?)
 */
void cUnit::think_position() {
    // keep updating dimensions
    dimensions->move(draw_x(), draw_y());
    if (mapCamera) {
        dimensions->resize(mapCamera->factorZoomLevel(getBmpWidth()),
                           mapCamera->factorZoomLevel(getBmpHeight()));
    }
}

bool cUnit::isMovingBetweenCells() {
    // when not perfectly divisible then it is 'between' cells.
    return ((int)posX) % TILESIZE_WIDTH_PIXELS != 0 || ((int)posY) % TILESIZE_HEIGHT_PIXELS != 0;
}

bool cUnit::isDamaged() {
    return iHitPoints < getUnitType().hp;
}

void cUnit::restoreFromTempHitPoints() {
    iHitPoints = iTempHitPoints; // restore true hitpoints
    iTempHitPoints = -1; // get rid of this hack
}

void cUnit::setMaxHitPoints() {
    iHitPoints = getUnitType().hp;
}

bool cUnit::canSquishInfantry() {
    return getUnitType().squish;
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
                        // unit ordered to move to refinery, let refinery animate about that.
                        pStructure->setAnimating(true);
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
    this->posX = map.getAbsoluteXPositionFromCell(cll);
    this->posY = map.getAbsoluteYPositionFromCell(cll);
}

void cUnit::assignMission(int aMission) {
    char msg[255];
    sprintf(msg, "I'm being assigned to mission %d (prev mission was %d)", aMission, mission);
    log(msg);
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
 * @param bOnStart
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

    s_UnitP &sUnitType = unitInfo[unitType];

    // check if unit already exists on location
    if (!sUnitType.airborn && map.cellGetIdFromLayer(iCll, MAPID_STRUCTURES) > -1) {
        return -1; // cannot place unit, structure exists at location
    }

    int mapIdIndex = MAPID_UNITS;
    if (sUnitType.airborn) {
        mapIdIndex = MAPID_AIR;
    } else if (sUnitType.type == SANDWORM) {
        mapIdIndex = MAPID_WORMS;
    }

    // check if unit already exists on location
    if (map.cellGetIdFromLayer(iCll, mapIdIndex) > -1) {
        return -1; // cannot place unit
    }

    // check if placed on invalid terrain type
    int cellType = map.getCellType(iCll);
    if (unitType == SANDWORM) {
        if (cellType != TERRAIN_SAND &&
            cellType != TERRAIN_SPICE &&
            cellType != TERRAIN_HILL &&
            cellType != TERRAIN_SPICEHILL)
            return -1;
    }


    // not airborn, and not infantry, may not be placed on walls and mountains.
    if (!sUnitType.infantry && !sUnitType.airborn) {
        if (cellType == TERRAIN_MOUNTAIN || cellType == TERRAIN_WALL) {
            return -1;
        }
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

    newUnit.TIMER_bored = rnd(3000);
    newUnit.TIMER_guard = -20 + rnd(70);
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

    if (unitType == SANDWORM) {
        map.cellSetIdForLayer(iCll, MAPID_WORMS, iNewId);
    } else if (!newUnit.isAirbornUnit()) {  // not airborn
        newUnit.iPlayer = iPlayer;
        map.cellSetIdForLayer(iCll, MAPID_UNITS, iNewId);
    } else {
        // aircraft
        newUnit.iPlayer = iPlayer;
        map.cellSetIdForLayer(iCll, MAPID_AIR, iNewId);
    }

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

    game.onNotify(event);

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
    if (game.paths_created > 40) {
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

    game.paths_created++;
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
    bool is_worm = (pUnit.iType == SANDWORM);

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

        if (ex <= cx)
            pUnit.log("CX = EX");
        if (ey <= cy)
            pUnit.log("CY = EY");

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

                // Determine if its a good cell to use or not:
                bool good = false; // not good by default

                // not a sandworm
                int cellType = map.getCellType(cll);
                if (!is_worm) {
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
                                    char msg[255];
                                    sprintf(msg,
                                            "CREATE_PATH(unitId=%d) - iPathCountUnits < 0 - variable 'good' becomes 'false'",
                                            iUnitId);
                                    pUnit.log(msg);
                                }
                            }

                            cUnit &unitAtCell = unit[iUID];
                            if (!unitAtCell.getPlayer()->isSameTeamAs(pUnit.getPlayer())) {
                                // allow running over enemy infantry/squishable units
                                if (unitAtCell.isInfantryUnit() &&
                                    pUnit.canSquishInfantry()) // and the current unit can squish
                                    good = true; // its infantry we want to run over, so don't be bothered!
                            }
                            //good=false; // it is not good, other unit blocks
                        } else {
                            good = true;
                        }
                    }


                    // is not visible, always good (since we dont know yet if its blocked!)
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
                    // is worm

                    // when not on sand, on spice or on sandhill, it is BAD
                    if (cellType == TERRAIN_SAND ||
                        cellType == TERRAIN_SPICE ||
                        cellType == TERRAIN_HILL ||
                        cellType == TERRAIN_SPICEHILL) {
                        good = true;
                    } else {
                        good = false;
                    }
                }

                // it is the goal cell
                if (cll == goal_cell) {
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
                    (good)) // and its not occupied (but may be occupied by own id!
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
                // check if any other cell of temp_path also borders to the previous given cell, as that will safe us time
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
                    if (unitInfo[unit[iID].iType].infantry) {
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

    char msg[255];
    sprintf(msg, "REINFORCE: Bringing unit type %d for player %d. Starting from cell %d, going to cell %d", iTpe, iPlr, iStartCell, iCll);
    logbook(msg);

    // STEP 2: create carryall
    int iUnit = UNIT_CREATE(iStartCell, CARRYALL, iPlr, true, isReinforcement);

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

    unit[iUnit].carryall_order(-1, TRANSFER_NEW_LEAVE, iCll, iTpe);
}

/**
 * Finds a free carryall of the same player as unit iuID. Returns > -1 which is the ID of the
 * carry-all which is going to pick up the unit. Or < 0 if no carry-all has been found to transfer unit.
 * @param iuID
 * @param iGoal
 * @return
 */
int CARRYALL_TRANSFER(int iuID, int iGoal) {
    // find a free carry all, and bring unit to goal..
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != unit[iuID].iPlayer) continue;
        if (cUnit.iType != CARRYALL) continue; // skip non-carry-all units
        if (cUnit.iTransferType != TRANSFER_NONE) continue; // skip busy carry-alls

        // assign an order
        cUnit.carryall_order(iuID, TRANSFER_PICKUP, iGoal, -1);
        return i;
    }

    return -1; // fail
}

/**
 * Deselect all units (not player specific)
 * TODO: Move to unitUtils class.
 *
 */
void UNIT_deselect_all() {
    for (int i = 0; i < MAX_UNITS; i++) {
        if (unit[i].isValid()) {
            unit[i].bSelected = false;
        }
    }
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

// done every second
void THINK_REINFORCEMENTS() {
    for (int i = 0; i < MAX_REINFORCEMENTS; i++) {
        if (reinforcements[i].iCell > -1) {
            if (reinforcements[i].iSeconds > 0) {
                reinforcements[i].iSeconds--;
                continue; // next one
            } else {
                // deliver
                REINFORCE(reinforcements[i].iPlayer, reinforcements[i].iUnitType, reinforcements[i].iCell,
                          players[reinforcements[i].iPlayer].getFocusCell());

                // and make this unvalid
                reinforcements[i].iCell = -1;
            }
        }
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

    char msg[255];
    sprintf(msg, "[%d] Reinforcement: Controller = %d, House %d, Time %d, Type = %d", iIndex, iPlyr,
            players[iPlyr].getHouse(), iTime, iUType);
    logbook(msg);

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

