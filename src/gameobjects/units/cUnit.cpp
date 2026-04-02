/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cUnit.h"
#include "cUnits.h"


#include "game/cGame.h"
#include "include/d2tmc.h"
#include "utils/cLog.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/structures/cStarPort.h"
#include "map/cMapCamera.h"
#include "map/cMapEditor.h"
#include "map/MapGeometry.hpp"
#include "player/cPlayer.h"
#include "player/cPlayers.h"
#include "utils/cSoundPlayer.h"
#include "utils/Graphics.hpp"
#include "utils/RNG.hpp"
#include <SDL2/SDL.h>
#include <format>
#include "drawers/cTextDrawer.h"
#include "gameobjects/units/cPathFinder.h"
#include "utils/d2tm_math.h"

#include <cmath>

#include "data/gfxaudio.h"

// Class specific on top
// Globals on bottom

void cUnit::init(int i)
{
    mission = -1;
    boundParticleId = -1;
    m_bSelected = false;
    rendering.bHovered  = false;

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
    movement.iNextCell = -1;      // where to move to (next cell)
    movement.iGoalCell = -1;      // the goal cell (goal of path)
    iCell = -1;          // cell of unit
    iCellX = -1;
    iCellY = -1;
    posX = -1;
    posY = -1;
    memset(movement.iPath, -1, sizeof(movement.iPath));    // path of unit
    movement.iPathIndex = -1;     // where are we?
    movement.iPathFails = 0;
    movement.bCalculateNewPath = false;

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
    m_bSelected = false;

    // Action given code
    iUnitID = -1;        // Unit ID to attack/pickup, etc
    iStructureID = -1;   // structure ID to attack/bring to (refinery)

    // Carry-All specific
    m_transferType = eTransferType::NONE;
    // iUnitIDWithinStructure = unit we CARRY (when TransferType == 1)
    // iTempHitPoints = hp of unit when transfertype = 1

    iBringTarget = -1;    // Where to bring the carried unit (when iUnitIDWithinStructure > -1)
    iNewUnitType = -1;    // new unit that will be brought, will be this type
    bPickedUp = false;    // carry-all/frigate: picked up the unit? (TODO: change this into states!)
    willBePickedUpBy = -1;// ID of the carry-all that will pick up this unit

    // harv
    iCredits = 0;

    // Drawing
    rendering.iBodyFacing = 0;    // Body of tanks facing
    rendering.iHeadFacing = 0;    // Head of tanks facing
    rendering.iBodyShouldFace = rendering.iBodyFacing;    // where should the unit body look at?
    rendering.iHeadShouldFace = rendering.iHeadFacing;    // where should th eunit look at?

    rendering.iFrame = 0;

    // TIMERS
    // TIMER_blink = 0;
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

void cUnit::recreateDimensions()
{
    // set up dimensions
    dimensions = cRectangle(draw_x(), draw_y(), getBmpWidth(), getBmpHeight());
}

void cUnit::setBoundParticleId(int particleId)
{
    this->boundParticleId = particleId;
}

void cUnit::die(bool bBlowUp, bool bSquish)
{
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
        cParticle &pParticle = game.m_particles[boundParticleId];
        if (pParticle.isValid()) {
            pParticle.die();
        }
        boundParticleId = -1;
    }

    // Animation / Sound

    // Anyone who was attacking this unit is on actionGuard
    for (int i = 0; i < game.m_Units.size(); i++) {
        cUnit &pUnit = game.getUnit(i);
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
            cUnit &pUnit = game.getUnit(iUnitID);
            if (pUnit.isValid()) {
                pUnit.willBePickedUpBy = -1; // no longer being picked up by this one
            }
        }
    }
    else {
        tellCarryAllThatWouldPickMeUpToForgetAboutMe();
    }

    if (bBlowUp) {
        createExplosionParticle();
    }

    if (bSquish) {
        createSquishedParticle();
    }

    if (iStructureID > -1) {
        cAbstractStructure *pStructure = game.m_pStructures[iStructureID];
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

    game.m_map.remove_id(iID, MAPID_UNITS);
    game.m_map.remove_id(iID, MAPID_AIR);
    game.m_map.remove_id(iID, MAPID_WORMS);
}

void cUnit::createSquishedParticle()
{
    int iDieX = pos_x_centered();
    int iDieY = pos_y_centered();
    int iHouse = getPlayer()->getHouse();
    // when we do not 'blow up', we died by something else. Only infantry will be 'squished' here now.
    if (iType == SOLDIER || iType == TROOPER || iType == UNIT_FREMEN_ONE) {
        int iType1 = D2TM_PARTICLE_SQUISH01 + RNG::rnd(2);
        cParticle::create(iDieX, iDieY, iType1, iHouse, rendering.iFrame);
        game.playSoundWithDistance(SOUND_SQUISH, distanceBetweenCellAndCenterOfScreen(iCell));
    }
    else if (iType == TROOPERS || iType == INFANTRY || iType == UNIT_FREMEN_THREE) {
        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_SQUISH03, iHouse, rendering.iFrame);
        game.playSoundWithDistance(SOUND_SQUISH, distanceBetweenCellAndCenterOfScreen(iCell));
    }
}

void cUnit::createExplosionParticle()
{
    int iDieX = pos_x_centered();
    int iDieY = pos_y_centered();

    auto mapEditor = cMapEditor(game.m_map);

    if (iType == TRIKE || iType == RAIDER || iType == QUAD) {
        // play quick 'boom' sound and show animation
        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_TRIKE, -1, -1);
        game.playSoundWithDistance(SOUND_TRIKEDIE, distanceBetweenCellAndCenterOfScreen(iCell));

        if (RNG::rnd(100) < 30) {
            cParticle::create(iDieX, iDieY - 24, D2TM_PARTICLE_SMOKE_WITH_SHADOW, -1, -1);
        }
    }

    if ((iType == SIEGETANK || iType == DEVASTATOR) && RNG::rnd(100) < 25) {
        if (rendering.iBodyFacing == FACE_UPLEFT || rendering.iBodyFacing == FACE_DOWNRIGHT) {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_SIEGEDIE, iPlayer, -1);
        }
    }

    if (iType == TANK || iType == SIEGETANK || iType == SONICTANK || iType == LAUNCHER || iType == DEVIATOR ||
            iType == HARVESTER || iType == ORNITHOPTER || iType == CARRYALL || iType == MCV || iType == FRIGATE) {
        // play quick 'boom' sound and show animation
        if (RNG::rnd(100) < 50) {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_TANK_ONE, -1, -1);
            game.playSoundWithDistance(SOUND_TANKDIE2, distanceBetweenCellAndCenterOfScreen(iCell));
        }
        else {
            cParticle::create(iDieX, iDieY, D2TM_PARTICLE_EXPLOSION_TANK_TWO, -1, -1);
            game.playSoundWithDistance(SOUND_TANKDIE, distanceBetweenCellAndCenterOfScreen(iCell));
        }

        if (RNG::rnd(100) < 30) {
            cParticle::create(iDieX, iDieY - 24, D2TM_PARTICLE_SMOKE_WITH_SHADOW, -1, -1);
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
                    int iType1 = D2TM_PARTICLE_EXPLOSION_STRUCTURE01 + RNG::rnd(2);
                    cParticle::create(iDieX + (cx * 32), iDieY + (cy * 32), iType1, -1, -1);
                }

                if (RNG::rnd(100) < 35)
                    game.playSoundWithDistance(SOUND_TANKDIE + RNG::rnd(2), distanceBetweenCellAndCenterOfScreen(iCell));

                // calculate cell and damage stuff around this
                int cll = game.m_map.getGeometry().getCellWithMapBorders((iCellX - 1) + cx, (iCellY - 1) + cy);

                if (cll < 0 || cll == iCell)
                    continue; // do not do own cell

                if (game.m_map.getCellType(cll) == TERRAIN_WALL) {
                    // damage this type of wall...
                    game.m_map.cellTakeDamage(cll, 150);

                    if (game.m_map.getCellHealth(cll) < 0) {
                        // remove wall, turn into smudge:
                        mapEditor.createCell(cll, TERRAIN_ROCK, 0);

                        mapEditor.smoothAroundCell(cll);

                        game.m_map.smudge_increase(SmudgeType::S_WALL, cll);
                    }
                }

                // damage surrounding units
                int idOfUnitAtCell = game.m_map.getCellIdUnitLayer(cll);
                if (idOfUnitAtCell > -1) {
                    int id = idOfUnitAtCell;

                    if (game.getUnit(id).iHitPoints > 0) {
                        game.getUnit(id).iHitPoints -= 150;

                        // NO HP LEFT, DIE
                        if (game.getUnit(id).iHitPoints <= 1)
                            game.getUnit(id).die(true, false);
                    } // only die when the unit is going to die
                }

                int idOfStructureAtCell = game.m_map.getCellIdStructuresLayer(cll);
                if (idOfStructureAtCell > -1) {
                    // structure hit!
                    int id = idOfStructureAtCell;

                    cAbstractStructure *pStructure = game.m_pStructures[id];
                    assert(pStructure);
                    if (pStructure->getHitPoints() > 0) {

                        int iDamage = 150 + RNG::rnd(100);
                        pStructure->damage(iDamage, -1); // no need to pass unit ID, as it is dead anyway

                        int iChance = 10;

                        if (pStructure->getHitPoints() < (game.structureInfos[pStructure->getType()].hp / 2)) {
                            iChance = 30;
                        }

                        if (RNG::rnd(100) < iChance) {
                            long x = pos_x() + (game.m_mapCamera->getViewportStartX()) + 16 + (-8 + RNG::rnd(16));
                            long y = pos_y() + (game.m_mapCamera->getViewportStartY()) + 16 + (-8 + RNG::rnd(16));
                            cParticle::create(x, y, D2TM_PARTICLE_SMOKE_WITH_SHADOW, -1, -1);
                        }
                    }
                }


                int cellType = game.m_map.getCellType(cll);
                if (cellType == TERRAIN_ROCK) {
                    if (cellType != TERRAIN_WALL)
                        game.m_map.cellTakeDamage(cll, 30);

                    if (game.m_map.getCellHealth(cll) < -25) {
                        game.m_map.smudge_increase(SmudgeType::S_ROCK, cll);
                        game.m_map.cellGiveHealth(cll, RNG::rnd(25));
                    }
                }
                else if (cellType == TERRAIN_SAND ||
                         cellType == TERRAIN_HILL ||
                         cellType == TERRAIN_SPICE ||
                         cellType == TERRAIN_SPICEHILL) {
                    if (cellType != TERRAIN_WALL)
                        game.m_map.cellTakeDamage(cll, 30);

                    if (game.m_map.getCellHealth(cll) < -25) {
                        game.m_map.smudge_increase(SmudgeType::S_SAND, cll);
                        game.m_map.cellGiveHealth(cll, RNG::rnd(25));
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

        game.playSoundWithDistance(SOUND_DIE01 + RNG::rnd(5), distanceBetweenCellAndCenterOfScreen(iCell));
    }

    if (iType == TROOPERS || iType == INFANTRY || iType == UNIT_FREMEN_THREE) {
        // create particle of dead body

        cParticle::create(iDieX, iDieY, D2TM_PARTICLE_DEADINF01, iPlayer, -1);

        game.playSoundWithDistance(SOUND_DIE01 + RNG::rnd(5), distanceBetweenCellAndCenterOfScreen(iCell));
    }
}

/**
 * Any unit this structure was "interacting with" (ie, wanted to enter), make sure
 * to remove that connection.
 */
void cUnit::unitWillNoLongerBeInteractingWithStructure() const
{
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
bool cUnit::isValid() const
{
    if (iPlayer < 0)
        return false;

    // invalid cell, not good
    if (iCell < 0 || iCell >= game.m_map.getMaxCells())
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

int cUnit::pos_x()
{
    return posX;
}

int cUnit::pos_y()
{
    return posY;
}

int cUnit::draw_x()
{
    return draw_x(getBmpWidth());
}

int cUnit::draw_x(int bmpWidth)
{
    // Example:
    // unit with 48 width (harvester, carry-all)
    // (32 - 48) = -16 / 2 = -8
    int bmpOffset = (TILESIZE_WIDTH_PIXELS - bmpWidth) / 2;
    // x is -8 pixels (ie, 8 pixels more to the left than top-left corner (pos_x) of a cell
    return game.m_mapCamera->getWindowXPositionWithOffset(pos_x(), bmpOffset);
}

int cUnit::draw_y()
{
    return draw_y(getBmpHeight());
}

int cUnit::draw_y(int bmpHeight)
{
    // same as draw_x
    int bmpOffset = (TILESIZE_HEIGHT_PIXELS - bmpHeight) / 2;
    return game.m_mapCamera->getWindowYPositionWithOffset(pos_y(), bmpOffset);
}

int cUnit::center_draw_x()
{
    return game.m_mapCamera->getWindowXPosition(pos_x_centered());
}

int cUnit::center_draw_y()
{
    return game.m_mapCamera->getWindowYPosition(pos_y_centered());
}

int cUnit::getBmpHeight() const
{
    return game.unitInfos[iType].bmp_height;
}

void cUnit::draw_spice()
{
    float width_x = game.m_mapCamera->factorZoomLevel(getBmpWidth());
    int height_y = game.m_mapCamera->factorZoomLevel(4);
    int drawx = draw_x();
    int drawy = draw_y() - ((height_y * 2) + 2);

    int max = getUnitInfo().credit_capacity;
    int w = healthBar(width_x, iCredits, max);

    // bar itself
    global_renderDrawer->renderRectFillColor(drawx, drawy, width_x, height_y, 0, 0, 0,ShadowTrans);
    global_renderDrawer->renderRectFillColor(drawx, drawy, w, height_y, 255, 91, 1,ShadowTrans);

    // bar around it (only when it makes sense due zooming)
    if (height_y > 2) {
        global_renderDrawer->renderRectColor(drawx, drawy,width_x, height_y, 255, 255, 255,ShadowTrans);
    }
}

int cUnit::getBmpWidth() const
{
    return game.unitInfos[iType].bmp_width;
}

float cUnit::getHealthNormalized()
{
    s_UnitInfo &unitType = getUnitInfo();
    float flMAX = unitType.hp;
    return (iHitPoints / flMAX);
}

float cUnit::getTempHealthNormalized()
{
    s_UnitInfo &unitType = getUnitInfo();
    float flMAX = unitType.hp;
    return (iTempHitPoints / flMAX);
}

void cUnit::draw_health()
{
    if (iHitPoints < 0) return;

    // draw units health
    float width_x = game.m_mapCamera->factorZoomLevel(getBmpWidth());
    int height_y = game.m_mapCamera->factorZoomLevel(4);
    int drawx = draw_x();
    int drawy = draw_y() - (height_y + 2);

    float healthNormalized = getHealthNormalized();

    int w = healthNormalized * width_x;
    int r = (1.1 - healthNormalized) * 255;
    int g = healthNormalized * 255;

    if (r > 255) r = 255;

    // bar itself
    global_renderDrawer->renderRectFillColor(drawx, drawy, width_x, height_y, 0, 0, 0,ShadowTrans);
    global_renderDrawer->renderRectFillColor(drawx, drawy, (w - 1), height_y, (Uint8)r,(Uint8)g, 32,ShadowTrans);

    // bar around it (only when it makes sense due zooming)
    if (height_y > 2) {
        global_renderDrawer->renderRectColor(drawx, drawy, width_x, height_y, 255, 255, 255,ShadowTrans);
    }
}

void cUnit::draw_group(cTextDrawer* textDrawer)
{
    if (iHitPoints < 0) return;

    int height_y = game.m_mapCamera->factorZoomLevel(4);
    int drawx = draw_x();
    int drawy = draw_y() - (height_y + 2);
    // draw group
    // TODO: make text smaller depending on zoom factor?
    if (iGroup > 0 && iPlayer == HUMAN) {
        // @mira I don't fix group name without acces to textDrawer: fixed on ctx branch
        textDrawer->drawText(drawx + 26, drawy - 11, Color::black(),std::format("{}",iGroup));
        textDrawer->drawText(drawx + 26, drawy - 12, Color::white(),std::format("{}",iGroup));
    }
}

// this method returns the amount of percent extra damage may be done
float cUnit::fExpDamage()
{
    if (fExperience < 1) return 0; // no stars

    // MAX EXPERIENCE = 10 (9 stars)

    // A unit can do 2x damage on full experience; being very powerfull.
    // it does take a long time to get there though.
    float fResult = (fExperience / 10);

    // never make a unit weaker.
    if (fResult < 1.0) fResult = 1.0F;
    return fResult;
}

void cUnit::draw_experience()
{
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
        global_renderDrawer->renderSprite(gfxdata->getTexture(OBJECT_STAR_01 + iStarType), drawx + i * 9, drawy, ShadowTrans);
    }
}

void cUnit::draw_path() const
{
    // for debugging purposes
    if (iCell == movement.iGoalCell)
        return;

    if (movement.iPath[0] < 0)
        return;

    int halfTile = 16;
    int iPrevX = game.m_mapCamera->getWindowXPositionFromCellWithOffset(movement.iPath[0], halfTile);
    int iPrevY = game.m_mapCamera->getWindowYPositionFromCellWithOffset(movement.iPath[0], halfTile);

    for (int i = 1; i < MAX_PATH_SIZE; i++) {
        if (movement.iPath[i] < 0) break;
        int iDx = game.m_mapCamera->getWindowXPositionFromCellWithOffset(movement.iPath[i], halfTile);
        int iDy = game.m_mapCamera->getWindowYPositionFromCellWithOffset(movement.iPath[i], halfTile);

        if (i == movement.iPathIndex) { // current node we navigate to
            global_renderDrawer->renderLine(iPrevX, iPrevY, iDx, iDy, Color{255, 255, 255,255});
        }
        else if (movement.iPath[i] == movement.iGoalCell) {
            // end of path (goal)
            global_renderDrawer->renderLine(iPrevX, iPrevY, iDx, iDy, Color{255, 0, 0,255});
        }
        else {
            // everything else
            global_renderDrawer->renderLine(iPrevX, iPrevY, iDx, iDy, Color{255, 255, 64,255});
        }

        // draw a line from previous to current
        iPrevX = iDx;
        iPrevY = iDy;
    }
}

void cUnit::draw()
{
    if (isHidden()) {
        // temp hitpoints filled, meaning it is not visible (but not dead). Ie, it is being repaired, or transfered
        // by carry-all
        return;
    }

    // Selection box x, y position. Depends on unit size
    const int ux = draw_x();
    const int uy = draw_y();

    if (isSandworm()) {
        return;
    }

    s_UnitInfo &unitType = getUnitInfo();
    const int bmp_width = unitType.bmp_width;
    const int bmp_height = unitType.bmp_height;

    // the multiplier we will use to draw the unit
    const int bmp_head = convertAngleToDrawIndex(rendering.iHeadFacing);
    const int bmp_body = convertAngleToDrawIndex(rendering.iBodyFacing);

    // draw body first
    int start_x = bmp_body * bmp_width;
    int start_y = bmp_height * rendering.iFrame;

    cPlayer &cPlayer = game.getPlayer(this->iPlayer);

    const float scaledWidth = game.m_mapCamera->factorZoomLevel(bmp_width);
    const float scaledHeight = game.m_mapCamera->factorZoomLevel(bmp_height);

    Texture *shadow = cPlayer.getUnitShadowBitmap(iType);
    int roundedScaledWidth = static_cast<int>(round(scaledWidth));
    int roundedScaledHeight = static_cast<int>(round(scaledHeight));
    if (shadow) {
        cRectangle src = {start_x, start_y, bmp_width, bmp_height};
        cRectangle dest = {ux, uy, roundedScaledWidth, roundedScaledHeight};
        if (iType == CARRYALL) {
            dest = {ux, uy+24, roundedScaledWidth, roundedScaledHeight};
        }
        global_renderDrawer->renderStrechSprite(shadow,src, dest, ShadowTrans);
    }

    // Draw BODY
    Texture *bitmap = cPlayer.getUnitBitmap(iType);
    if (bitmap) {
        cRectangle src = {start_x, start_y, bmp_width, bmp_height};
        cRectangle dest = {ux, uy, roundedScaledWidth, roundedScaledHeight};
        global_renderDrawer->renderStrechSprite(bitmap,src, dest);
    }
    else {
        log(std::format("unit of iType [{}] did not have a bitmap!?", iType));
    }


    // Draw TOP
    Texture *top = cPlayer.getUnitTopBitmap(iType);
    if (top && iHitPoints > -1) {
        // recalculate start_x using head instead of body
        start_x = bmp_head * bmp_width;
        start_y = bmp_height * rendering.iFrame;
        cRectangle src = {start_x, start_y, bmp_width, bmp_height};
        cRectangle dest = {ux, uy, static_cast<int>(round(game.m_mapCamera->factorZoomLevel(bmp_width))), static_cast<int>(round(game.m_mapCamera->factorZoomLevel(bmp_height)))};
        global_renderDrawer->renderStrechSprite(top,src, dest);
    }

    // when we want to be picked up..
    if (bCarryMe) {
        global_renderDrawer->renderSprite(gfxdata->getTexture(SYMB_PICKMEUP), ux, uy - 7);
    }

    if (m_bSelected) {
        SDL_Surface *focusBitmap = gfxdata->getSurface(FOCUS);
        int bmp_width = focusBitmap->w;
        int bmp_height = focusBitmap->h;

        int x = draw_x(bmp_width);
        int y = draw_y(bmp_height);

        cRectangle dest = {x,y, static_cast<int>(round(game.m_mapCamera->factorZoomLevel(bmp_width))),static_cast<int>(round(game.m_mapCamera->factorZoomLevel(bmp_height)))};
        global_renderDrawer->renderStrechFullSprite(gfxdata->getTexture(FOCUS), dest);
    }

    if (game.m_drawUnitDebug) {
        // render pixel at the very center
        global_renderDrawer->renderDot(center_draw_x(), center_draw_y(), Color{255, 255, 0,255},2);

        // render from the units top-left to center pixel
        global_renderDrawer->renderLine( draw_x(), draw_y(), center_draw_x(), center_draw_y(), Color{255, 255, 0,255});
    }
}

// TODO: only do this when iCell is updated
void cUnit::updateCellXAndY()
{
    iCellX = game.m_map.getCellX(iCell);
    iCellY = game.m_map.getCellY(iCell);
}

/**
 * Attack unit, and will chase target when out of range.
 * @param targetUnit
 */
void cUnit::attackUnit(int targetUnit)
{
    attackUnit(targetUnit, true);
}

void cUnit::attackUnit(int targetUnit, bool chaseWhenOutOfRange)
{
    log(std::format("attackUnit() : targetUnit is [{}]. Chase target? [{}]", targetUnit, chaseWhenOutOfRange));
    attack(game.getUnit(targetUnit).iCell, targetUnit, -1, -1, chaseWhenOutOfRange);
}

void cUnit::attackStructure(int targetStructure)
{
    log(std::format("attackStructure() : target is [{}]", targetStructure));
    attack(game.m_pStructures[targetStructure]->getCell(), -1, targetStructure, -1, true);
}

void cUnit::attackCell(int cell)
{
    log(std::format("attackCell() : cell target is [{}]", cell));
    attack(cell, -1, -1, cell, true);
}

void cUnit::attack(int goalCell, int unitId, int structureId, int attackCell, bool chaseWhenOutOfRange)
{
    // basically the same as move, but since we use m_action as ATTACK
    // it will think first in attack mode, determining if it will be CHASE now or not.
    // if not, it will just fire.

    log(std::format("Attacking UNIT ID [{}], STRUCTURE ID [{}], ATTACKCLL [{}], GoalCell [{}]",
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

    setAction(chaseWhenOutOfRange ? eActionType::ATTACK_CHASE : eActionType::ATTACK);
    setGoalCell(goalCell);
    iAttackStructure = structureId;
    iAttackUnit = unitId;
    this->iAttackCell = attackCell;
    forgetAboutCurrentPathAndPrepareToCreateNewOne(RNG::rnd(5));
}

void cUnit::attackAt(int cell)
{
    log(std::format("attackAt() : cell target is [{}]", cell));

    if (!game.m_map.isWithinBoundaries(cell)) {
        log("attackAt() : Invalid cell, aborting");
        return;
    }

    int unitId = game.m_map.getCellIdUnitLayer(cell);
    int structureId = game.m_map.getCellIdStructuresLayer(cell);
    int wormId = game.m_map.getCellIdWormsLayer(cell);
    log(std::format("attackAt() : cell target is [{}], structureId [{}], unitId [{}], wormId [{}]", cell, structureId, unitId, wormId));

    if (structureId > -1) {
        attackStructure(structureId);
        return;
    }

    if (unitId > -1) {
        attackUnit(unitId);
        return;
    }

    if (wormId > -1) {
        attackUnit(wormId);
        return;
    }

    attackCell(cell);
}

void cUnit::move_to(int iCll, int iStructureIdToEnter, int iUnitIdToPickup)
{
    move_to(iCll, iStructureIdToEnter, iUnitIdToPickup, eUnitActionIntent::INTENT_MOVE);
}

void cUnit::move_to(int iCll, int iStructureIdToEnter, int iUnitIdToPickup, eUnitActionIntent intent)
{
    if (isSandworm() && game.m_disableWormAi) {
        // don't do anything
        return;
    }

    log(std::format("(move_to - START) : to cell [{}], iStructureIdToEnter[{}], iUnitIdToPickup[{}] (to attack, if > -1), intent[{}]",
                    iCll, iStructureIdToEnter, iUnitIdToPickup, eUnitActionIntentString(intent)));

    setGoalCell(iCll);
    if (iStructureID > -1) {
        unitWillNoLongerBeInteractingWithStructure();
    }

    tellCarryAllThatWouldPickMeUpToForgetAboutMe();

    iStructureID = iStructureIdToEnter;

    if (iStructureIdToEnter > -1) {
        cAbstractStructure *pStructure = game.m_pStructures[iStructureIdToEnter];
        if (!pStructure->hasUnitHeadingTowards() && !pStructure->hasUnitWithin()) {
            pStructure->unitHeadsTowardsStructure(iID);
        }
    }

    iUnitID = iUnitIdToPickup;

    iAttackStructure = -1;
    iAttackCell = -1;

    // only when not moving (half on tile) reset nextcell
    if (!isMovingBetweenCells()) {
        movement.iNextCell = -1;
    }

    setAction(eActionType::MOVE);
    this->intent = intent;

    forgetAboutCurrentPathAndPrepareToCreateNewOne();

    log("(move_to - FINISHED)");
}

/**
 * If this unit would be picked up by a carry-all (identifiedby willBePickedUpBy ID), then let it know we no
 * longer want to be picked up. Ie, we died, or we wanted to move away ourselves and the carry-all should not
 * interfere.
 */
void cUnit::tellCarryAllThatWouldPickMeUpToForgetAboutMe() const
{
    if (willBePickedUpBy > -1) {
        cUnit &pUnit = game.getUnit(willBePickedUpBy);
        if (pUnit.isValid()) {
            pUnit.forgetAboutUnitToPickUp();
        }
    }
}


/**
 * Called every 5 ms, when action = ACTION_GUARD
 */
void cUnit::thinkFast_guard()
{
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
        rendering.iBodyShouldFace = RNG::rnd(8);
        rendering.iHeadShouldFace = RNG::rnd(8);
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
    TIMER_guard = 20 + RNG::rnd(35); // do not scan all at the same time

    updateCellXAndY();

    if (isSandworm() && !game.m_disableWormAi) {
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
        cUnit &unitToAttack = game.getUnit(unitIdToAttack);

        if (unitToAttack.isValid()) {
//            s_GameEvent event {
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
            }
            else {
                attackUnit(unitIdToAttack, false);
            }
        }
        else {
            attackUnit(unitIdToAttack, false);
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
    }
    else {
        // human units do auto-attack structures that can attack things
        int id = findNearbyStructureThatCanDamageUnitsToAttack(getRange());
        if (id > -1) {
            attackStructure(id);
        }
    }
}

bool cUnit::isSandworm() const
{
    return iType == SANDWORM;
}

/**
 * Called every 100ms
 */
void cUnit::think()
{
    thinkActionAgnostic();

    // Think attack style
    if (m_action == eActionType::ATTACK_CHASE || m_action == eActionType::ATTACK) {
        think_attack();
    }
}

void cUnit::thinkActionAgnostic()
{
    // if (TIMER_blink > 0) {
    //     TIMER_blink--;
    // }

    // if (iType == MCV) {
    //     think_MVC();
    // }

    if (isSandworm()) {
        // add a worm trail behind worm randomly for now (just not every frame, or else this spams a great
        // deal of particles overlapping eachother.
        TIMER_wormtrail++;
        if (TIMER_wormtrail > 2) {
            long x = pos_x_centered();
            long y = pos_y_centered();
            cParticle::create(x, y, D2TM_PARTICLE_WORMTRAIL, -1, -1);
            TIMER_wormtrail = 0;
        }
    }

    // HEAD is not facing correctly
    if (!isAirbornUnit()) {
        if (rendering.iBodyFacing == rendering.iBodyShouldFace) {
            if (rendering.iHeadFacing != rendering.iHeadShouldFace) {
                TIMER_turn++;
                if (TIMER_turn > getTurnSpeed()) {
                    TIMER_turn = 0;
                    rendering.iHeadFacing = determineNewFacing(rendering.iHeadFacing, rendering.iHeadShouldFace);
                } // turn
            } // head facing

        }
        else {
            think_turn_to_desired_body_facing();
        }

    }
    else {
        // air units, have only 'body' facing
        if (rendering.iBodyFacing != rendering.iBodyShouldFace) {
            think_turn_to_desired_body_facing();
        }
    }

    // when waiting.. wait
    if (TIMER_thinkwait > 0) {
        TIMER_thinkwait--;
        return;
    }

    if (isHidden())
        return;

    // when any non-airborn, non-sandworm unit is on a spice bloom, it dies
    int cellType = game.m_map.getCellType(iCell);
    if (!isAirbornUnit() && !isSandworm() && cellType == TERRAIN_BLOOM) {
        game.m_map.detonateSpiceBloom(iCell);
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

cAbstractStructure *cUnit::findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(int structureType)
{
    return game.m_map.findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(iCell, structureType, getPlayer());
}

cAbstractStructure *cUnit::findClosestAvailableStructureType(int structureType)
{
    return game.m_map.findClosestAvailableStructureType(iCell, structureType, getPlayer());
}

cAbstractStructure *cUnit::findClosestStructureType(int structureType)
{
    return game.m_map.findClosestStructureType(iCell, structureType, getPlayer());
}

void cUnit::think_carryAll()  // A carry-all has something when:
{
// - it carries a unit (iUnitIDWithinStructure > -1)
// - it has the flag eTransferType::NEW_

    if ((m_transferType == eTransferType::NEW_STAY ||
            m_transferType == eTransferType::NEW_LEAVE ||
            m_transferType == eTransferType::PICKUP) || iUnitID > -1) {

        // when picking up a unit.. only draw when picked up
        if (m_transferType == eTransferType::PICKUP && bPickedUp)
            rendering.iFrame = 1;

        // any other transfer, means it is filled from start...
        if (m_transferType != eTransferType::PICKUP)
            rendering.iFrame = 1;
    }
    else {
        rendering.iFrame = 0;
    }
}

void cUnit::think_ornithopter()
{
    cPlayer *pPlayer = getPlayer();
    rendering.iFrame++;

    if (rendering.iFrame > 3) {
        rendering.iFrame = 0;
    }

    if (iAttackUnit < 0 && iAttackStructure < 0) {
        selectTargetForOrnithopter(pPlayer);
    }
    else {
        TIMER_attack++;
    }
}

void cUnit::selectTargetForOrnithopter(cPlayer *pPlayer)
{
    // find enemy unit or structure to attack
    // units within range are taking priority, else we select a random structure.
    int iDistance = 9999;
    int iTarget = -1;

    for (int i = 0; i < game.m_Units.size(); i++) {
        cUnit &target = game.getUnit(i);
        if (target.isValid() && i != iID) {
            if (pPlayer->isSameTeamAs(target.getPlayer()))
                continue;

            // not ours and its visible
            if (target.iPlayer != iPlayer &&
                    game.m_map.isVisible(target.iCell, iPlayer) &&
                    !target.isAirbornUnit()) { // for now, to prevent orni's taking down carry-alls?
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
    }
    else {
        // no unit found, attack structure
        // scan for enemy activities.
        int iDistance = 9999;
        int iTarget = -1;

        for (int i = 0; i < MAX_STRUCTURES; i++) {
            cAbstractStructure *pStructure = game.m_pStructures[i];
            if (!pStructure) continue;
            if (!pStructure->isValid()) continue;

            // skip same team
            if (pPlayer->isSameTeamAs(pStructure->getPlayer()))
                continue;

            // not ours and its visible
            if (pStructure->getPlayerId() != iPlayer && // enemy
                    game.m_map.isStructureVisible(pStructure, iPlayer)) {
                int c = pStructure->getCell();
                int distance = game.m_map.distance(iCell, c);

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

void cUnit::think_turn_to_desired_body_facing()
{
    // BODY is not facing correctly
    TIMER_turn++;

    float turnspeed = game.unitInfos[iType].turnspeed;
    if (isAirbornUnit()) {
        // when closer to goal, turnspeed decreases.
        double distance = game.m_map.distance(iCell, movement.iGoalCell);
        int distanceInCells = 8;
        if (distance < distanceInCells) {
            turnspeed = (turnspeed/distanceInCells) * distance;
        }
        else {
            // when close to a border, then reduce turnspeed so that orni's wont crash over the map borders
            if ((iCellX < 4 || iCellX >= (game.m_map.getWidth()-4)) || (iCellY < 4 || iCellY >= (game.m_map.getHeight()-4))) {
                turnspeed = 0;
            }
        }
    }

    if (TIMER_turn > turnspeed) {
        TIMER_turn = 0;

        rendering.iBodyFacing = determineNewFacing(rendering.iBodyFacing, rendering.iBodyShouldFace);
    } // turn body
}

int cUnit::determineNewFacing(int currentFacing, int desiredFacing)
{
    int newFacing = currentFacing;

    int d = 1;

    int toleft = (newFacing + 8) - desiredFacing;
    if (toleft > 7) toleft -= 8;

    int toright = abs(toleft - 8);

    if (toright == toleft) d = -1 + (RNG::rnd(2));
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
void cUnit::thinkFast_move_airUnit()
{
    if (isHidden()) {
        return;
    }

    if (TIMER_movewait > 0) {
        TIMER_movewait--;
        return;
    }

    movement.iNextCell = getNextCellToMoveTo();

    if (!game.m_map.isValidCell(iCell)) {
        die(true, false);

        // KILL UNITS WHO SOMEHOW GET INVALID
        cLogger::getInstance()->log(LOG_DEBUG, COMP_UNITS, "Air unit got out of bounds",
                         std::format("Air unit [{}] got invalid cell [{}], and thus got killed.", iID, iCell) );
        return;
    }

    if (game.m_map.isAtMapBoundaries(iCell)) {
        if (!isReinforcement) {
            // let unit face directly to ideal angle, so it won't fly into its doom (out of map)
            rendering.iBodyFacing = rendering.iBodyShouldFace;
            rendering.iHeadFacing = rendering.iHeadShouldFace;
        }
    }

    if (!game.m_map.isValidCell(movement.iNextCell))
        movement.iNextCell = iCell;

    if (!game.m_map.isValidCell(movement.iGoalCell)) {
        setGoalCell(iCell);
    }

    // same cell (no goal specified or something)
    if (movement.iNextCell == iCell) {
        bool isWithinMapBoundaries = game.m_map.isWithinBoundaries(iCellX, iCellY);

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
                                game.m_map.cellResetIdFromLayer(iCell, MAPID_UNITS);

                                // now move air unit to the 'bring target'
                                setGoalCell(iBringTarget);

                                // smoke puffs when picking up unit
                                int pufX = (pos_x() + getBmpWidth() / 2);
                                int pufY = (pos_y() + getBmpHeight() / 2);

                                cParticle::create(pufX, pufY, D2TM_PARTICLE_CARRYPUFF, -1, -1);

                                log("Pick up unit");
                                return;
                            }
                        }
                        else {
                            // goal/unit not yet reached

                            if (!unitToPickupOrDrop.bPickedUp) {
                                // keep updating goal as long as unit has not been picked up yet.
                                setGoalCell(unitToPickupOrDrop.iCell);
                            }
                            else {
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
                        if (!game.m_map.occupied(iCell, iUnitID) && isWithinMapBoundaries) {
                            // valid structure
                            cAbstractStructure *structureUnitWantsToEnter = unitToPickupOrDrop.getStructureUnitWantsToEnter();

                            if (structureUnitWantsToEnter) {
                                bool isAttemptingDeployingAtStructure = game.m_map.getCellIdStructuresLayer(iCell) == structureUnitWantsToEnter->getStructureId();

                                if (isAttemptingDeployingAtStructure) {
                                    if (structureUnitWantsToEnter->isInProcessOfBeingEnteredOrOccupiedByUnit(unitToPickupOrDrop.iID)) {
                                        // TODO: Do this with events
                                        // already became occupied, so try to find a different kind of structure
                                        int type = structureUnitWantsToEnter->getType();
                                        cAbstractStructure *alternative = findClosestAvailableStructureType(type);

                                        if (alternative) {
                                            setGoalCell(alternative->getRandomStructureCell());
                                            iBringTarget = movement.iGoalCell;
                                            unitToPickupOrDrop.awaitBeingPickedUpToBeTransferedByCarryAllToStructure(alternative);
                                            return;
                                        }
                                        else {
                                            // !?
                                            int dropLocation = game.m_map.findNearByValidDropLocation(iCell, 3, unitToPickupOrDrop.iType);
//                                            carryAll_transferUnitTo(iUnitID, dropLocation);
                                            setGoalCell(dropLocation);
                                            iBringTarget = dropLocation;
                                            return;
                                        }
                                    } // structure is being occupied or just about to be occupied
                                } // attempts to unload above structure
                            }

                            // dump it here (unload from carry-all)
                            unitToPickupOrDrop.setCell(iCell);
                            unitToPickupOrDrop.setGoalCell(iCell);
                            unitToPickupOrDrop.updateCellXAndY(); // update cellx and celly
                            game.m_map.cellSetIdForLayer(iCell, MAPID_UNITS, iUnitID);

                            unitToPickupOrDrop.iHitPoints = unitToPickupOrDrop.iTempHitPoints;
                            unitToPickupOrDrop.iTempHitPoints = -1;
                            unitToPickupOrDrop.TIMER_movewait = 0;
                            unitToPickupOrDrop.TIMER_thinkwait = 0;
                            unitToPickupOrDrop.iCarryAll = -1;

                            // match facing of carryall
                            unitToPickupOrDrop.rendering.iHeadFacing = rendering.iHeadFacing;
                            unitToPickupOrDrop.rendering.iHeadShouldFace = rendering.iHeadShouldFace;
                            unitToPickupOrDrop.rendering.iBodyFacing = rendering.iBodyFacing;
                            unitToPickupOrDrop.rendering.iBodyShouldFace = rendering.iBodyShouldFace;

                            // clear spot
                            game.m_map.clearShroud(iCell, unitToPickupOrDrop.getUnitInfo().sight, iPlayer);

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
                        }
                        else {
                            // find a new spot
                            updateCellXAndY();
                            setGoalCell(findNewDropLocation(game.getUnit(iUnitID).iType, iCell));
                            iBringTarget = movement.iGoalCell;
                            return;
                        }
                    }
                }
            }
            else {
                m_transferType = eTransferType::NONE; // unit is not valid?
                return;
            }
        }

        // transfer is to create a new unit
        if (m_transferType == eTransferType::NEW_LEAVE ||
                m_transferType == eTransferType::NEW_STAY) {
            // bring a new unit

            if (iType == FRIGATE) {
                int iStrucId = game.m_map.getCellIdStructuresLayer(iCell);

                if (iStrucId > -1) {
                    setGoalCell(iFindCloseBorderCell(iCell));
                    m_transferType = eTransferType::DIE;

                    game.m_pStructures[iStrucId]->setFrame(4); // show package on this structure
                    game.m_pStructures[iStrucId]->setAnimating(true); // keep animating
                    dynamic_cast<cStarPort *>(game.m_pStructures[iStrucId])->setFrigateDroppedPackage(true);
                }
                else {
                    // we don't expect this to go wrong :/
                    cAbstractStructure *starport = findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(STARPORT);

                    // find closest Starport to deliver next (Starport got mid-way destroyed)
                    if (!starport) {
                        // not found, die
                        die(true, false);
                    }
                    else {
                        setGoalCell(starport->getCell());
                        starport->setUnitIdHeadingTowards(iID);
                    }
                }

                return; // override for frigates
            }

            bool canDeployAtCell = game.m_map.occupied(iCell, iID) == false;

            if (iNewUnitType > -1) {
                canDeployAtCell = game.m_map.canDeployUnitAtCell(iCell, iID);
            }

            // first check if this cell is clear
            if (canDeployAtCell && isWithinMapBoundaries) {
                // drop unit
                if (iNewUnitType > -1) {
                    int id = cUnits::unitCreate(iCell, iNewUnitType, iPlayer, true, isReinforcement);

                    if (id > -1) {
                        game.m_map.cellSetIdForLayer(iCell, MAPID_UNITS, id);
                    }
                }

                // now make sure this carry-all will not be drawn as having a unit:
                iNewUnitType = -1;

                // depending on transfertype...
                if (m_transferType == eTransferType::NEW_LEAVE) {
                    m_transferType = eTransferType::DIE;

                    // find a new border cell close to us... to die
                    setGoalCell(iFindCloseBorderCell(iCell));
                    return;
                }
                else if (m_transferType == eTransferType::NEW_STAY) {
                    // reset transfertype:
                    m_transferType = eTransferType::NONE;
                    return;
                }

            }
            else {
                // find a new spot for delivery
                updateCellXAndY();
                setGoalCell(findNewDropLocation(iNewUnitType, iCell));
                return;
            }
        }

        // move randomly
        int cell = lastDroppedOffCell;
        if (cell < 0) {
            cell = getPlayer()->getFocusCell();
        }
        setGoalCell(game.m_map.getRandomCellFromWithRandomDistance(cell, 12));
        return;
    }

    // goal cell == next cell (move straight to it)
    TIMER_move++;

    // now move
    int goalCellX = game.m_map.getCellX(movement.iGoalCell);
    int goalCellY = game.m_map.getCellY(movement.iGoalCell);

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
                if (RNG::rnd(100) < 5) {
                    int cellType = game.m_map.getCellType(iCell);
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
        }
        else {
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
    int idealAngle = faceAngle(d);
    int f = faceAngle(d); // get the angle
    float angle = 0;

    rendering.iBodyShouldFace = idealAngle;
    if (rendering.iBodyFacing != rendering.iBodyShouldFace) {
        // not ideal angle, the aircraft flies straight ahead which it is facing.
        // since we don't have a velocity vector, we do it like this for now:
        int nextX = iCellX;
        int nextY = iCellY;

        switch (rendering.iBodyFacing) {
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
    }
    else {
        // using ideal angle, just fly straight towards goal
        angle = fRadians(iCellX, iCellY, goalCellX, goalCellY);
    }

    rendering.iHeadFacing = f;

    game.m_map.cellResetIdFromLayer(iCell, MAPID_AIR);

//    int movespeed = getUnitInfo().speed;
    int movespeed = 2; // 2 pixels, the actual 'speed' is done by the delay above using TIMER_move! :/
    float newPosX = posX + cos(angle) * movespeed;
    float newPosY = posY + sin(angle) * movespeed;
    setPosX(newPosX);
    setPosY(newPosY);

    // Cell of unit is determined by its center
    iCell = game.m_mapCamera->getCellFromAbsolutePosition(pos_x_centered(), pos_y_centered());

    updateCellXAndY();
    game.m_map.cellSetIdForLayer(iCell, MAPID_AIR, iID);
}

void cUnit::setPosX(float newVal)
{
    float diff = newVal - posX;
    posX = newVal;
    if (boundParticleId > -1) {
        cParticle &pParticle = game.m_particles[boundParticleId];
        pParticle.addPosX(diff);
    }
}

void cUnit::setPosY(float newVal)
{
    float diff = newVal - posY;
    posY = newVal;
    if (boundParticleId > -1) {
        cParticle &pParticle = game.m_particles[boundParticleId];
        pParticle.addPosY(diff);
    }
}

void cUnit::forgetAboutUnitToPickUp()  // forget about this
{
    setGoalCell(iCell);
    m_transferType = eTransferType::NONE;
    iUnitID = -1;
}

cUnit &cUnit::getUnitToPickupOrDrop() const
{
    assert(iUnitID > -1 && "cUnit::getUnitToPickupOrDrop() called for invalid iUnitIDWithinStructure!");
    return game.getUnit(iUnitID);
}

cAbstractStructure *cUnit::getStructureUnitWantsToEnter() const
{
    cAbstractStructure *structureUnitWantsToEnter = nullptr;
    if (iStructureID > -1) {
        structureUnitWantsToEnter = game.m_pStructures[iStructureID];
        if (structureUnitWantsToEnter && !structureUnitWantsToEnter->isValid()) {
            structureUnitWantsToEnter = nullptr;
        }
    }
    return structureUnitWantsToEnter;
}

int cUnit::findNewDropLocation(int unitTypeToDrop, int cell) const
{
    int dropLocation = game.m_map.findNearByValidDropLocation(cell, 4, unitTypeToDrop);
    if (dropLocation < 0) {
        dropLocation = game.m_map.findNearByValidDropLocation(cell, 8, unitTypeToDrop);
    }
    if (dropLocation < 0) {
        dropLocation = game.m_map.findNearByValidDropLocation(cell, 16, unitTypeToDrop);
    }
    return dropLocation;
}

// Carryall-order
//
// Purpose:
// Order a carryall to pickup a unit, or send a new unit (depending on transferType)
//
void cUnit::carryall_order(int iuID, eTransferType transferType, int iBring, int iTpe)
{
    if (m_transferType != eTransferType::NONE)
        return; // we cannot do multiple things at a time!!

    if (transferType == eTransferType::NEW_STAY || transferType == eTransferType::NEW_LEAVE) {
        // bring a new unit, depending on the transferType the carryall who brings this will be
        // removed after he brought the unit...

        // when iTranfer is 0 or 2, the unit is just created by REINFORCE() and this function
        // sets the target and such.

        m_transferType = transferType;

        // Go to this:
        setGoalCell(iBring);
        iBringTarget = movement.iGoalCell;

        // unit we carry is none
        iUnitID = -1;

        // the unit type...
        iNewUnitType = iTpe;

        bPickedUp = false;
        // DONE!
    }
    else if (transferType == eTransferType::PICKUP && iuID > -1) {
        // the carryall must pickup the unit, and then bring it to the iBring stuff
        cUnit &pUnit = game.getUnit(iuID);
        if (pUnit.isValid()) {
            m_transferType = transferType;

            setGoalCell(pUnit.iCell); // first go to the target to pick it up

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

void cUnit::shoot(int iTargetCell)
{
    // particles are rendered at the center, so do it here as well
    int iShootX = pos_x() + (getBmpWidth() / 2);
    int iShootY = pos_y() + (getBmpHeight() / 2);
    int bmp_head = convertAngleToDrawIndex(rendering.iHeadFacing);

    // TODO: add this in sUnitInfo
    if (iType == TANK) {
        cParticle::create(iShootX, iShootY, D2TM_PARTICLE_TANKSHOOT, -1, bmp_head);
    }
    else if (iType == SIEGETANK) {
        cParticle::create(iShootX, iShootY, D2TM_PARTICLE_SIEGESHOOT, -1, bmp_head);
    }

    s_UnitInfo &unitInfo = getUnitInfo();
    int bulletType = unitInfo.bulletType;
    // if secondary fire is configured properly
    if (unitInfo.fireSecondaryWithinRange > -1 && unitInfo.bulletTypeSecondary > -1) {
        if (game.m_map.distance(iCell, iTargetCell) <= unitInfo.fireSecondaryWithinRange) {
            bulletType = unitInfo.bulletTypeSecondary;
        }
    }


    if (bulletType < 0) return; // no bullet type to spawn

    int iBull = createBullet(bulletType, iCell, iTargetCell, iID, -1);

    cUnit *attackUnit = nullptr;
    if (iAttackUnit > -1) {
        attackUnit = &game.getUnit(iAttackUnit);
        if (attackUnit && !attackUnit->isValid()) {
            // allowing homing bullets towards air units from the ground
            if (iBull > -1 && attackUnit->isAirbornUnit()) {
                game.g_Bullets[iBull].iHoming = iAttackUnit;
                game.g_Bullets[iBull].TIMER_homing = 200;
            }
        }
    }
}

int cUnit::getNextCellToMoveTo()
{
    if (isAirbornUnit()) {
        if (movement.iGoalCell == iCell) {
            return iCell;
        }

        return movement.iGoalCell; // return the goal
    }

    if (movement.iPathIndex < 0) {
        return -1;
    }

    // not valid OR same location
    int nextCell = movement.iPath[movement.iPathIndex];
    if (nextCell < 0) {
        log("No valid movement.iPath[pathindex], nextCell is < 0");
        return -1;
    }

    // now, we are sure it will be another location
    return nextCell;
}

// ouch, who shot me?
void cUnit::think_hit(int iShotUnit, int iShotStructure)
{
    if (isDead()) {
        // I've died , so don't bother
        return;
    }

    if (isSaboteur()) {
        // ignore being shot?
        return;
    }

    if (iShotUnit > -1) {
        cUnit &unitWhoShotMe = game.getUnit(iShotUnit);
        if (unitWhoShotMe.getPlayer()->isSameTeamAs(getPlayer())) return; // ignore friendly fire

        bool unitWhoShotMeIsInfantry = unitWhoShotMe.isInfantryUnit();
        bool unitWhoShotMeIsAirborn = unitWhoShotMe.isAirbornUnit();

        // only act when we are doing 'nothing'...
        if (m_action == eActionType::GUARD) {
            // only auto attack back when it is not an airborn unit
            // note: guard state already takes care of scanning for air units and attacking them
            if (!unitWhoShotMeIsAirborn) {
                attackUnit(iShotUnit); // by default chase unit in case it is out of range
            }
        }

        if (!getPlayer()->isHuman()) {
            int unitCellWhichShotMe = game.getUnit(iShotUnit).iCell;
            if (isHarvester()) {
                if (game.getUnit(iShotUnit).isInfantryUnit() && !isMovingBetweenCells()) {
                    // this harvester will try to run over the infantry that attacks it
                    move_to(unitCellWhichShotMe);
                }
                else {
                    // under attack, retreat to base? find nearby units to help out?
                }

            }
            else {
                bool notAttacking = m_action != eActionType::ATTACK_CHASE && m_action != eActionType::ATTACK;
                if (notAttacking) {
                    if (canSquishInfantry() && unitWhoShotMeIsInfantry) {
                        if (movement.iGoalCell != unitCellWhichShotMe) {
                            double distance = 9999;
                            if (movement.iGoalCell != iCell && m_action == eActionType::MOVE) {
                                // moving towards a goal already
                                distance = game.m_map.distance(iCell, unitCellWhichShotMe);
                            }
                            // found a unit closer to squish, so move towards it
                            if (distance < game.m_map.distance(iCell, movement.iGoalCell)) {
                                // AI tries to run over infantry units that attack it
                                move_to(unitCellWhichShotMe);
                            }
                        }
                    }
                    else {
                        if (!unitWhoShotMeIsAirborn) {
                            if (isSandworm()) {
                                if (game.m_map.isCellPassableForWorm(unitWhoShotMe.iCell)) {
                                    attackUnit(iShotUnit);
                                }
                                else {
                                    // tough luck :/
                                }
                            }
                            else {
                                attackUnit(iShotUnit);
                            }
                        }
                    }
                }
                else {
                    // we are attacking, but when target is very far away (out of range?) then we should not attack that but defend
                    // ourselves
                    int iDestCell = iAttackCell;

                    if (iDestCell < 0) {
                        if (iAttackUnit > -1)
                            iDestCell = game.getUnit(iAttackUnit).iCell;

                        if (iAttackStructure > -1) {
                            cAbstractStructure *pStructure = game.m_pStructures[iAttackStructure];
                            // it can become null, so check!
                            if (pStructure && pStructure->isValid()) {
                                iDestCell = pStructure->getCell();
                            }
                        }

                        if (game.m_map.distance(iCell, iDestCell) < getRange()) {
                            // within range, don't move (just prepare retaliation fire)
                        }
                        else {
                            // out of range unit, attack it
                            if (canSquishInfantry() && unitWhoShotMeIsInfantry) {
                                // AI tries to run over infantry units that attack it
                                move_to(unitCellWhichShotMe);
                            }
                            else {
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
    else if (iShotStructure > -1) {
        // TODO?
    }

    // for infantry , infantry turns into soldier and troopers into trooper when on 50% damage.
    if (isInfantryUnit()) {
        if (iType == INFANTRY || iType == TROOPERS) {
            // turn into soldier or trooper when on 50% health
            if (iHitPoints <= (game.unitInfos[iType].hp / 3)) {
                // leave 2 dead bodies (of 3 ;))

                // turn into single one
                if (iType == INFANTRY)
                    iType = SOLDIER;

                if (iType == TROOPERS)
                    iType = TROOPER;

                iHitPoints = game.unitInfos[iType].hp;

                int half = 16;
                int iDieX = pos_x() + half;
                int iDieY = pos_y() + half;

                cParticle::create(iDieX, iDieY, D2TM_PARTICLE_DEADINF01, iPlayer, -1);

                game.playSoundWithDistance(SOUND_DIE01 + RNG::rnd(5),
                                           distanceBetweenCellAndCenterOfScreen(iCell));

            }
        }
    }
}

void cUnit::log(const std::string &txt) const
{
    // logs unit stuff, but gives unit information
    game.getPlayer(iPlayer).log(std::format("[UNIT[{}]: type = {}(={}), iCell = {}, movement.iGoalCell = {}] '{}'",
                                     iID, iType, game.unitInfos[iType].name, iCell, movement.iGoalCell, txt));
}

/**
 * Thinking, called every 100ms
 */
void cUnit::think_attack()
{
    updateCellXAndY();

    if (isSandworm()) {
        think_attack_sandworm();
        return;
    }

    cUnit *attackUnit = nullptr;
    if (iAttackUnit > -1) {
        attackUnit = &game.getUnit(iAttackUnit);

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

        setGoalCell(attackUnit->iCell);
    }

    cAbstractStructure *pStructure = nullptr;
    if (iAttackStructure > -1) {
        pStructure = game.m_pStructures[iAttackStructure];
        if (pStructure && pStructure->isValid()) {
            setGoalCell(pStructure->getCell());
        }
        else {
            iAttackUnit = -1;
            iAttackStructure = -1;
            setGoalCell(iCell);
            setAction(eActionType::GUARD);
            return;
        }

        if (pStructure->isDead()) {
            actionGuard();
            log("Destroyed structure target");
            return;
        }
    }

    if (iAttackCell > -1) {
        setGoalCell(iAttackCell);

        bool isBloomOrWallTerrain =
            game.m_map.getCellType(iAttackCell) == TERRAIN_BLOOM || game.m_map.getCellType(iAttackCell) == TERRAIN_WALL;

        if (isBloomOrWallTerrain) {
            // stop attacking a spice bloom or a wall when it got destroyed
            if (game.m_map.getCellHealth(iAttackCell) < 0) {
                actionGuard();
                return;
            }
        }
        else {
            if (!getPlayer()->isHuman()) {
                // non human units should stop attacking the ground
                actionGuard();
                return;
            }
        }
    }

    // Distance check
    int distance = game.m_map.distance(iCell, movement.iGoalCell);

    if (isAirbornUnit()) {
        // AIRBORN UNITS ATTACK THINKING
        int minDistance = 2;

        if (distance > minDistance && distance <= getRange()) {
            // when this function returns true, it is done firing bullets
            if (setAngleTowardsTargetAndFireBullets(distance)) {
                int randomCellFrom = game.m_map.getRandomCellFrom(movement.iGoalCell, 16);
                int rx = game.m_map.getCellX(randomCellFrom);
                int ry = game.m_map.getCellY(randomCellFrom);

                iAttackUnit = -1;
                iAttackStructure = -1;
                setAction(eActionType::MOVE);
                setGoalCell(game.m_map.getGeometry().getCellWithMapDimensions(rx, ry));
            }
        }
        else {
            // stop attacking, move instead?
            setAction(eActionType::MOVE);
            iAttackUnit = -1;
            iAttackStructure = -1;
        }

        return; // bail, air-unit attack thinking finished
    }

    if (!isMovingBetweenCells()) {
        if (distance <= getRange()) {
            setAngleTowardsTargetAndFireBullets(distance);
        }
        else {   // not within distance
            if (m_action == eActionType::ATTACK_CHASE) {
                startChasingTarget();
            }
            else {
                actionGuard();
            }
        }
    }
    else {
        if (distance > getRange()) {
            // atleast don't try to attack it
            if (m_action == eActionType::ATTACK_CHASE) {
                startChasingTarget();
            }
            else {
                actionGuard();
            }
        }
    }
}

void cUnit::think_attack_sandworm()
{
    if (iAttackUnit < 0) {
        // no attack unit
        actionGuard();
        return;
    }

    cUnit *attackUnit = &game.getUnit(iAttackUnit);

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

    // update movement.iGoalCell with where the attacking unit is (chase)
    setGoalCell(attackUnit->iCell);
    if (movement.iGoalCell == iCell) {
        attackUnit->die(false, false);
        unitsEaten++;
        long x = pos_x_centered();
        long y = pos_y_centered();
        cParticle::create(x, y, D2TM_PARTICLE_WORMEAT, -1, -1);
        game.playSoundWithDistance(SOUND_WORM, distanceBetweenCellAndCenterOfScreen(iCell));
        actionGuard();
        TIMER_movewait = (1000/5) * 4; // wait for 4 seconds before moving again
        TIMER_guard = (1000/5) * 4; // timer guard works other way around..

        if (unitsEaten >= getUnitInfo().appetite) {
            // let worm die (and respawn later)
            iHitPoints = getUnitInfo().dieWhenLowerThanHP;
            takeDamage(1); // get below the thresh-hold to die/vanish
        }
        else {
            TIMER_guard = (1000/5) * ((5*unitsEaten) + RNG::rnd((20*unitsEaten)));
        }

        cLogger::getInstance()->log(LOG_DEBUG, COMP_UNITS, "think_attack_sandworm() -> eaten unit", 
                std::format("think_attack_sandworm() -> eaten unit. Units eaten {}, TIMER_guard {}",
                unitsEaten, TIMER_guard));
        return;
    }

    if (attackUnit->isIdle() && !game.m_map.isCellPassableForWorm(attackUnit->iCell)) {
        // forget about unit that is not reachable
        actionGuard();
        return;
    }

    startChasingTarget();
    return;
}

void cUnit::actionGuard()
{
    setAction(eActionType::GUARD);
    iAttackUnit = -1;
    iAttackCell = -1;
    iAttackStructure =-1;
}

int cUnit::getFaceAngleToCell(int cell) const
{
    int d = fDegrees(iCellX, iCellY, game.m_map.getCellX(cell), game.m_map.getCellY(cell));
    return faceAngle(d); // get the angle
}

void cUnit::startChasingTarget()
{
    if (iAttackStructure > -1) {
        setAction(eActionType::CHASE);
        // a structure does not move, so don't need to re-calculate path?
//        forgetAboutCurrentPathAndPrepareToCreateNewOne();
    }
    else if (iAttackUnit > -1) {
        cUnit *attackUnit = &game.getUnit(iAttackUnit);
        // chase unit, but only when ground unit
        if (!attackUnit->isAirbornUnit()) {
            setAction(eActionType::CHASE);
            // only think of new path when our target moved
            if (attackUnit->getCell() != movement.iGoalCell) {
                forgetAboutCurrentPathAndPrepareToCreateNewOne();
            }
        }
        else {
            // do not chase air units, very ... inconvenient
            setAction(eActionType::GUARD);
            setGoalCell(iCell);
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }
    }
    else if (iAttackCell > -1) {
        setAction(eActionType::CHASE);
    }
}

bool cUnit::setAngleTowardsTargetAndFireBullets(int distance)
{
    s_UnitInfo &unitType = getUnitInfo();
    // in range , fire and such

    // Facing
    int angle = getFaceAngleToCell(movement.iGoalCell);

    rendering.iBodyShouldFace = angle;
    rendering.iHeadShouldFace = angle;

    if (rendering.iBodyShouldFace == rendering.iBodyFacing && rendering.iHeadShouldFace == rendering.iHeadFacing) {

        TIMER_attack++;
        if (TIMER_attack >= unitType.attack_frequency) {
            int shootCell = movement.iGoalCell;

            if (iType == LAUNCHER || iType == DEVIATOR) {
                if (distance < unitType.range) {
                    int dx = game.m_map.getCellX(shootCell);
                    int dy = game.m_map.getCellY(shootCell);

                    int inaccuracy = ((unitType.range - distance) / 3) + 1; // at 'perfect' range, we always have a inaccuracy of 1 at minimum

                    dx -= inaccuracy;
                    dx += RNG::rnd((inaccuracy * 2)+1); // we need + 1, because it is 'until'

                    dy -= inaccuracy;
                    dy += RNG::rnd((inaccuracy * 2)+1); // we need + 1, because it is 'until'

                    shootCell = game.m_map.getGeometry().getCellWithMapDimensions(dx, dy);
                }
            }

            // first bullet
            if (TIMER_attack == unitType.attack_frequency) {
                shoot(shootCell);
            }

            bool canFireTwice = unitType.fireTwice && getHealthNormalized() > unitType.fireTwiceHpThresholdFactor;
            if (!canFireTwice) {
                TIMER_attack = 0;
                return true;
            }
            else {
                int secondShotTimeLimit = unitType.next_attack_frequency < 0 ?
                                          unitType.attack_frequency + (unitType.attack_frequency / 4)
                                          :
                                          unitType.attack_frequency + unitType.next_attack_frequency;

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

int cUnit::getRange() const
{
    return getUnitInfo().range;
}

int cUnit::getSight() const
{
    return getUnitInfo().sight;
}

s_UnitInfo &cUnit::getUnitInfo() const
{
    return game.unitInfos[iType];
}

// thinking about movement (called every 5 ms)
// called when ACTION_CHASE or ACTION_MOVE
void cUnit::thinkFast_move()
{
    if (!isValid()) {
        assert(false && "Expected to have a valid unit calling thinkFast_move()");
    }

    if (isSandworm() && game.m_disableWormAi) {
        return; // skip
    }

    // this is about non-aircraft only, so bail for aircraft units
    if (isAirbornUnit()) {
        return;
    }

    // it is a unit that is temporarily not available on the map (picked up by carry-all for instance, or
    // within a structure)
    if (isHidden()) {
        return;
    }

    if (TIMER_movewait > 0) {
        TIMER_movewait--;
        return;
    }

    // when there is a valid goal cell (differs), then we go further
    if (movement.iGoalCell == iCell) {
        setAction(eActionType::GUARD); // do nothing
        forgetAboutCurrentPathAndPrepareToCreateNewOne();
        return;
    }

    // not moving between cells, check if there is a new cell to move to
    if (!isMovingBetweenCells()) {
        movement.iNextCell = getNextCellToMoveTo();

        // no next cell determined
        if (movement.iNextCell < 0) {
            forgetAboutCurrentPathAndPrepareToCreateNewOne(0);

            // when we do have a different goal, we should get a path:
            if (movement.iGoalCell != iCell) {
                int iResult = cPathFinder::createPath(iID, 0); // do not take units into account yet
                log(std::format("Create path ... result = {}", iResult));

                // On fail:
                if (iResult < 0) {
                    // simply failed
                    if (iResult == -1) {
                        // Check why, is our goal cell occupied?
                        int uID = game.m_map.getCellIdUnitLayer(movement.iGoalCell);
                        int sID = game.m_map.getCellIdStructuresLayer(movement.iGoalCell);

                        // Other unit is on goal cell, do something about it.

                        // uh oh
                        if (uID > -1 && uID != iID) {
                            // occupied, not by self
                            // find a goal cell near to it
                            int iNewGoal = cPathFinder::returnCloseGoal(movement.iGoalCell, iCell, iID);

                            if (iNewGoal == movement.iGoalCell) {
                                // same goal, cant find new, stop
                                setGoalCell(iCell);
                                log("Could not find alternative goal");
                                movement.iPathIndex = -1;
                                movement.iPathFails = 0;
                                return;

                            }
                            else {
                                setGoalCell(iNewGoal);
                                TIMER_movewait = RNG::rnd(20);
                                log("Found alternative goal");
                                return;
                            }
                        }
                        else if (sID > -1 && iStructureID > -1 && iStructureID != sID) {
                            log("Want to enter structure, yet ID's do not match");
                            log("Resetting structure id and such to redo what i was doing?");
                            iStructureID = -1;
                            setGoalCell(iCell);
                            movement.iPathIndex = -1;
                            movement.iPathFails = 0;
                        }
                        else {
                            log("Something else blocks path, but goal itself is not occupied.");
                            setGoalCell(iCell);
                            movement.iPathIndex = -1;
                            movement.iPathFails = 0;
                            iStructureID = -1;

                            // random move around
                            int iF = freeAroundMove(iID);

                            if (iF > -1) {
                                move_to(iF);
                            }
                        }
                    }
                    else if (iResult == -2) {
                        // we have a problem here, units/stuff around us blocks the unit
                        log("Units surround me, what to do?");
                    }
                    else if (iResult == -3) {
                        log("I just failed, ugh");
                    }

                    // We failed
                    movement.iPathFails++;

                    if (movement.iPathFails > 2) {
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
                        setGoalCell(iCell);
                        movement.iPathFails = 0;
                        movement.iPathIndex = -1;
                        if (TIMER_movewait <= 0) {
                            TIMER_movewait = 100;
                        }
                    }
                }
                else {
                    //log("SUCCES");
                }
            } // must go somewhere (has a goal)
            return;
        } // didn't get valid nextCell
    }

    // Update the 'should' facing (ideal facing) of body and head.
    rendering.iBodyShouldFace = getFaceAngleToCell(movement.iNextCell);
    rendering.iHeadShouldFace = getFaceAngleToCell(movement.iGoalCell);

    // check
    bool bOccupied = false;

    int idOfUnitAtNextCell = game.m_map.getCellIdUnitLayer(movement.iNextCell);

    // Cell is occupied, not by self, so it is occupied...
    if (idOfUnitAtNextCell > -1 &&
            idOfUnitAtNextCell != iID) {
        // get it
        int iUID = idOfUnitAtNextCell;

        bOccupied = true;
        // unless... it is an enemy infantry unit and I can squish stuff
        if (canSquishInfantry()) {
            cUnit &unitAtCell = game.getUnit(iUID);
            if (unitAtCell.isValid() &&
                    unitAtCell.iPlayer != iPlayer && // enemy player?
                    unitAtCell.isInfantryUnit() // squishable?
               ) {
                bOccupied = false;
            }
        }
    }

    // structure is NOT matching our structure ID, then its blocking us
    int idOfStructureAtNextCell = game.m_map.getCellIdStructuresLayer(movement.iNextCell);

    if (idOfStructureAtNextCell > -1) {
        if (iStructureID < 0) {
            // not intended to go into a structure, so it blocks
            bOccupied = true;
        }
        else if (iStructureID > -1) {
            if (iStructureID == idOfStructureAtNextCell) {
                // it is the structure this unit intents to go to...
                cAbstractStructure *pStructure = game.m_pStructures[iStructureID];
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
                        }
                        else {
                            // start entering the structure at next cell
                            pStructure->startEnteringStructure(iID);
                            deselect();
                        }

                        // TODO: this should be done via events (EVENT_CAPTURED? and then make unit choose different structure)
                        if (!pStructure->getPlayer()->isSameTeamAs(getPlayer())) { // no longer of my team
                            findAlternativeStructure = true;
                            bOccupied = true;
                        }
                        else {    // structure is occupied by unit
                            // not occupied, continue!
                        }

                        if (findAlternativeStructure) {
                            // find alternative structure type nearby
                            const sFindBestStructureResult &result = findBestStructureCandidateToHeadTo(
                                        pStructure->getType());

                            cAbstractStructure *candidate = result.structure;
                            if (candidate && candidate->getStructureId() != iStructureID) {
                                // founder an alternative
                                move_to_enter_structure(candidate, intent);
                                return; // bail, else the logic below will kick of (ugh, bad code)
                            }
                            else {
                                // wait, maybe we can enter later!
                                TIMER_movewait = 100; // we wait
                                return; // bail, else the logic below will kick of (ugh, bad code)
                            }
                        }
                    }
                    else if (intent == eUnitActionIntent::INTENT_CAPTURE) {
                        if (pStructure->getPlayer()->isSameTeamAs(getPlayer())) {
                            iStructureID = -1;
                            setGoalCell(iCell);
                            movement.iNextCell = iCell;
                            TIMER_movewait = 100; // we wait
                            bOccupied = true; // obviously - but will do nothing :S
                            return; // bail, else the logic below will kick of (ugh, bad code)
                        }
                    }
                    else {
                        bOccupied = true;
                    }
                }
                else {
                    // structure is no longer valid, what now?
                    // .. not occupied?
                    bOccupied = true;
                }
            }
            else {
                // wrong structure, so blocks
                bOccupied = true;
            }
        }
    }

    int cellTypeAtNextCell = game.m_map.getCellType(movement.iNextCell);
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
        if (movement.iNextCell == movement.iGoalCell) {
            // it is our goal cell, close enough
            setGoalCell(iCell);
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
            return;
        }
        else if (idOfStructureAtNextCell > -1) {
            // blocked by structure
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }
        else if (idOfUnitAtNextCell > -1) {
            // From here, assume a unit is standing in our way. First check if this unit will
            // move. If so, we wait until it has moved.
            int uID = idOfUnitAtNextCell;

            // Wait when the obstacle is moving, perhaps it will clear our way
            cUnit &unitOccupyingNextCell = game.getUnit(uID);
            if (unitOccupyingNextCell.isValid() &&
                    unitOccupyingNextCell.TIMER_movewait <= 0 &&
                    unitOccupyingNextCell.movement.iGoalCell != unitOccupyingNextCell.iCell) {
                // this unit is also moving, so wait for it to move
                TIMER_movewait = 50;
                return;
            }
            else {
                // this unit has no intention to move away, so create path around it.
                forgetAboutCurrentPathAndPrepareToCreateNewOne();
            }
        }
        else {
            // all other cases (ie terrain blocking, etc)
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }
        return;
    }

    updateCellXAndY();

    if (rendering.iBodyShouldFace == rendering.iBodyFacing) {
        eUnitMoveToCellResult result = moveToNextCellLogic();

        if (result == eUnitMoveToCellResult::MOVERESULT_AT_GOALCELL ||
                result == eUnitMoveToCellResult::MOVERESULT_AT_CELL) {
            // not occupied cell;
            int idOfStructureAtCurrentCell = game.m_map.getCellIdStructuresLayer(iCell);

            // we wanted to enter this structure
            if (iStructureID > -1 &&
                    idOfStructureAtCurrentCell > -1 && idOfStructureAtCurrentCell == iStructureID) {
                cAbstractStructure *pStructure = game.m_pStructures[iStructureID];
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
                            }
                            else if (intent == eUnitActionIntent::INTENT_UNLOAD_SPICE) {
                                // find alternative
                            }
                        }
                    }
                    else if (intent == eUnitActionIntent::INTENT_CAPTURE || intent == eUnitActionIntent::INTENT_MOVE) {
                        if (isSaboteur()) {
                            // the unit will die and inflict damage
                            pStructure->damage(getUnitInfo().damageOnEnterStructure, -1); // no need to pass ID of unit, as it is dead
                            die(true, false);
                        }
                        else {
                            if (pStructure->getHitPoints() < pStructure->getCaptureHP()) {
                                // make structure switch sides
                                pStructure->getsCapturedBy(getPlayer());
                                die(false, false);
                            }
                            else {
                                // the unit will die and inflict damage
                                die(true, false);
                                pStructure->damage(getUnitInfo().damageOnEnterStructure, -1); // no need to pass ID of unit, as it is dead
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
eUnitMoveToCellResult cUnit::moveToNextCellLogic()
{
    // When we should move:
    int bToLeft = -1;         // 0 = go left, 1 = go right
    int bToDown = -1;         // 0 = go down, 1 = go up

    int iNextX = game.m_map.getCellX(movement.iNextCell);
    int iNextY = game.m_map.getCellY(movement.iNextCell);

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
    int cellType = game.m_map.getCellType(iCell);
    int iSlowDown = game.m_map.getCellSlowDown(iCell);

    cPlayerDifficultySettings *difficultySettings = game.getPlayer(iPlayer).getDifficultySettings();
    if (TIMER_move < ((difficultySettings->getMoveSpeed(iType, iSlowDown)))) {
        return eUnitMoveToCellResult::MOVERESULT_SLOWDOWN; // get out
    }

    TIMER_move = 0; // reset to 0

    // from here on, set the map id, so no other unit can take its place
    if (!isSandworm()) {
        // note, no AIRBORN here (27/03/2021 - I guess this is because this method is not called by thinkFast_move_airUnit())
        game.m_map.cellSetIdForLayer(movement.iNextCell, MAPID_UNITS, iID);
    }
    else {
        game.m_map.cellSetIdForLayer(movement.iNextCell, MAPID_WORMS, iID);
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

                }
                else {
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

            rendering.iFrame++;
            if (rendering.iFrame > 3)
                rendering.iFrame = 0;

            TIMER_frame = 0;
        }
    }

    // movement is done, determine what to do with new state.
    if (!isMovingBetweenCells()) {
        // when we are chasing, we now set on attack...
        if (m_action == eActionType::CHASE) {
            // next time we think, will be checking for distance, etc
            cUnit *attackUnit = &game.getUnit(iAttackUnit);
            if (attackUnit && attackUnit->isValid()) {
                setAction(eActionType::ATTACK_CHASE);
                if (attackUnit->getCell() != movement.iGoalCell) {
                    forgetAboutCurrentPathAndPrepareToCreateNewOne(0);
                }
            }
        }

        // movement to cell complete
        if (isSandworm()) {
            game.m_map.cellResetIdFromLayer(iCell, MAPID_WORMS);
        }
        else {
            game.m_map.cellResetIdFromLayer(iCell, MAPID_UNITS);
        }

        setCell(movement.iNextCell);
        movement.iPathIndex++;
        movement.iPathFails = 0; // we change this to 0... every cell

        updateCellXAndY();

        // check any units that are on our cell as well.
        // perhaps this could be sped up if we had more than 1 unit on a cell possible (for now we don't have
        // this). An alternative is to have a function return per 'segment' the units within it (so we don't need to
        // iterate over all units)
        if (canSquishInfantry()) {
            for (int iq = 0; iq < game.m_Units.size(); iq++) {
                cUnit &potentialDeadUnit = game.getUnit(iq);
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

        if (iPlayer == AI_CPU5 && game.getPlayer(HUMAN).isHouse(ATREIDES)) {
            // TODO: make this work for all allied forces
            // hackish way to get Fog of war clearance by allied fremen units (super weapon).
            game.m_map.clearShroud(iCell, game.unitInfos[iType].sight, HUMAN);
        }

        game.m_map.clearShroud(iCell, game.unitInfos[iType].sight, iPlayer);

        // The goal did change probably, or something else forces us to reconsider
        if (movement.bCalculateNewPath) {
            forgetAboutCurrentPathAndPrepareToCreateNewOne();
        }

        if (iCarryAll > -1) {
            log("A carry all is after me...");
            // wait longer for the carry-all to arive before thinking of a new path
            forgetAboutCurrentPathAndPrepareToCreateNewOne(10000);
            return eUnitMoveToCellResult::MOVERESULT_WAIT_FOR_CARRYALL; // wait a second will ya!
        }

        // Just arrived at goal cell
        if (iCell == movement.iGoalCell) {
            return eUnitMoveToCellResult::MOVERESULT_AT_GOALCELL;
        }

        TIMER_movewait = 2 + ((getUnitInfo().speed + iSlowDown) * 3);
        return eUnitMoveToCellResult::MOVERESULT_AT_CELL;
    }
    return eUnitMoveToCellResult::MOVERESULT_BUSY_MOVING;
}

int cUnit::pos_y_centered()
{
    return pos_y() + 16;
}

int cUnit::pos_x_centered()
{
    return pos_x() + 16;
}

/**
 * Clears the created path, resets next-cell to current cell. Sets timer to wait to 100. So that
 * after that the unit will think of a new path to create.
 */
void cUnit::forgetAboutCurrentPathAndPrepareToCreateNewOne()
{
    forgetAboutCurrentPathAndPrepareToCreateNewOne(35);
}

/**
 * Clears the created path, resets next-cell to current cell. Sets timer to wait to <timeToWait>. So that
 * after that the unit will think of a new path to create.
 */
void cUnit::forgetAboutCurrentPathAndPrepareToCreateNewOne(int timeToWait)
{
    memset(movement.iPath, -1, sizeof(movement.iPath));
    movement.iPathIndex = -1;
    TIMER_movewait = timeToWait;
}

bool cUnit::isInfantryUnit() const
{
    return game.unitInfos[iType].infantry;
}

cUnit::cUnit()
{
    mission = -1;
    init(-1);
}

cUnit::~cUnit()
{
}

/**
 * Poor man solution to frequently update the dimensions of unit, better would be using events?
 * (onMove, onViewportMove, onViewportZoom?)
 */
void cUnit::thinkFast_position()
{
    // keep updating dimensions
    dimensions.move(draw_x(), draw_y());
    if (game.m_mapCamera) {
        dimensions.resize(game.m_mapCamera->factorZoomLevel(getBmpWidth()),
                          game.m_mapCamera->factorZoomLevel(getBmpHeight()));
    }
}

bool cUnit::isMovingBetweenCells()
{
    // when not perfectly divisible then it is 'between' cells.
    return ((int)posX) % TILESIZE_WIDTH_PIXELS != 0 || ((int)posY) % TILESIZE_HEIGHT_PIXELS != 0;
}

bool cUnit::isDamaged()
{
    return iHitPoints < getUnitInfo().hp;
}

void cUnit::restoreFromTempHitPoints()
{
    iHitPoints = iTempHitPoints; // restore true hitpoints
    int maxHp = getUnitInfo().hp;
    if (iHitPoints > maxHp) {
        iHitPoints = maxHp;
    }
    iTempHitPoints = -1; // TODO: get rid of this hack
}

void cUnit::setMaxHitPoints()
{
    iHitPoints = getUnitInfo().hp;
}

bool cUnit::canSquishInfantry()
{
    return getUnitInfo().squish;
}

bool cUnit::canBeSquished()
{
    return getUnitInfo().canBeSquished;
}

cPlayer *cUnit::getPlayer()
{
    return &game.getPlayer(iPlayer);
}

bool cUnit::isSaboteur()
{
    return iType == SABOTEUR;
}

void cUnit::move_to(int iGoalCell)
{
    eUnitActionIntent intent = eUnitActionIntent::INTENT_MOVE;

    int structureID = -1;
    int unitID = -1;
    if (iGoalCell > -1) {
        structureID = game.m_map.getCellIdStructuresLayer(iGoalCell);
        unitID = game.m_map.getCellIdUnitLayer(iGoalCell);
    }

    if (structureID > -1) {
        cAbstractStructure *pStructure = game.m_pStructures[structureID];
        if (pStructure && pStructure->isValid()) {
            bool friendlyStructure = getPlayer()->isSameTeamAs(pStructure->getPlayer());
            if (friendlyStructure) {
                if (isInfantryUnit()) {
                    structureID = -1; // reset back, we don't allow capturing own/allied buildings
                }
                else if (isHarvester()) {
                    if (pStructure->getType() == REFINERY) {
                        intent = eUnitActionIntent::INTENT_UNLOAD_SPICE;
                    }
                    else if (pStructure->getType() == REPAIR) {
                        intent = eUnitActionIntent::INTENT_REPAIR;
                    }
                }
                else {
                    if (pStructure->getType() == REPAIR) {
                        intent = eUnitActionIntent::INTENT_REPAIR;
                    }
                }
            }
            else {
                // if capturable... (TODO)
                if (isInfantryUnit() || isSaboteur()) {
                    intent = eUnitActionIntent::INTENT_CAPTURE;
                }
                else {
                    //
                    intent = eUnitActionIntent::INTENT_MOVE;
                }
            }
        }
    }

    move_to(iGoalCell, structureID, unitID, intent);
}

void cUnit::setCell(int cll)
{
    this->iCell = cll;
    setPosX(game.m_map.getAbsoluteXPositionFromCell(cll));
    setPosY(game.m_map.getAbsoluteYPositionFromCell(cll));
}

void cUnit::assignMission(int aMission)
{
    log(std::format("I'm being assigned to mission {} (prev mission was {})", aMission, mission));
    mission = aMission;
}

bool cUnit::isAssignedAnyMission()
{
    return mission > -1;
}

bool cUnit::isAssignedMission(int aMission)
{
    return mission == aMission;
}

void cUnit::unAssignMission()
{
    mission = -1;
}

int cUnit::getPlayerId() const
{
    return iPlayer;
}

int cUnit::getType() const
{
    return iType;
}

std::string cUnit::eUnitActionIntentString(eUnitActionIntent intent)
{
    switch (intent) {
        case eUnitActionIntent::INTENT_CAPTURE:
            return "INTENT_CAPTURE";
        case eUnitActionIntent::INTENT_MOVE:
            return "INTENT_MOVE";
        case eUnitActionIntent::INTENT_ATTACK:
            return "INTENT_ATTACK";
        case eUnitActionIntent::INTENT_NONE:
            return "INTENT_NONE";
        case eUnitActionIntent::INTENT_REPAIR:
            return "INTENT_REPAIR";
        case eUnitActionIntent::INTENT_UNLOAD_SPICE:
            return "INTENT_UNLOAD_SPICE";
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
bool cUnit::isUnableToMove()
{
    if (game.m_map.occupied(game.m_map.getCellLeft(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellRight(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellAbove(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellBelow(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellLowerLeft(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellLowerRight(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellUpperRight(iCell), iID) &&
            game.m_map.occupied(game.m_map.getCellUpperLeft(iCell), iID)) {
        return true;
    }

    return false;
}

void cUnit::move_to_enter_structure(cAbstractStructure *pStructure, eUnitActionIntent intent)
{
    if (!pStructure) return;
    if (!pStructure->isValid()) return;

    // found a candidate, lets check if we can find a carry-all that will bring us
    if (!pStructure->hasUnitHeadingTowards() && !pStructure->hasUnitWithin()) {
        pStructure->unitHeadsTowardsStructure(iID);
        iStructureID = pStructure->getStructureId();
    }
    else {
        // we chose to head towards this structure, but we can't claim it; this is
        // the situation that more units than available structures are heading towards the same structure
        // in this case we only remember the structureId, as we still have the intent to head towards it.
        // we let the other code that with "entering a structure" deal with any possibly already occupied
        // structure stuff.
        iStructureID = pStructure->getStructureId();
    }
    move_to(pStructure->getRandomStructureCell(), pStructure->getStructureId(), -1, intent);
}

sFindBestStructureResult cUnit::findBestStructureCandidateToHeadTo(int structureType)
{
    cAbstractStructure *candidate = findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(structureType);
    // TODO: we could do this even better, by scanning the structures once. Remembering distance and a 'state'
    // so to speak, ie "AVAILABLE, NOT AVAILABLE, ALREADY_HEADING", etc. That way we only need to scan once, and don't
    // need to call these functions multiple times. Also, the logic could be more smart then which (distance) to pick.
    // but for now settle with this - as I want 0.6.0 get done at some point. So deliberately deferring the optimisation
    // in favor of getting things done.
    if (candidate) {
        return sFindBestStructureResult {
            .structure = candidate,
            .reason = eFindBestStructureResultReason::FREE_STRUCTURE
        };
    }

    candidate = findClosestAvailableStructureType(structureType);
    if (candidate) {
        // a structure is found that is free, but another unit is heading towards it.
        return sFindBestStructureResult {
            .structure = candidate,
            .reason = eFindBestStructureResultReason::OTHER_UNIT_ALSO_HEADS_TOWARDS_STRUCTURE
        };
    }

    candidate = findClosestStructureType(structureType);

    // a structure is found, but it is occupied
    if (candidate) {
        return sFindBestStructureResult {
            .structure = candidate,
            .reason = eFindBestStructureResultReason::OCCUPIED
        };
    }

    return sFindBestStructureResult {
        .structure = nullptr,
        .reason = eFindBestStructureResultReason::NO_RESULT
    };
}

eHeadTowardsStructureResult cUnit::findBestStructureCandidateAndHeadTowardsItOrWait(
    int structureType,
    bool allowCarryallTransfer,
    eUnitActionIntent actionIntent)
{
    rendering.iFrame = 0; // stop animating
    assert(structureType > -1);

    if (intent == actionIntent) {
        // same intent, so ignore?
        return eHeadTowardsStructureResult::NOOP_ALREADY_BUSY;
    }

    log(std::format("cUnit::findBestStructureCandidateAndHeadTowardsItOrWait - Going to look for a [{}]",
                    game.structureInfos[structureType].name));

    const sFindBestStructureResult &result = findBestStructureCandidateToHeadTo(structureType);

    // No structure found, so bail
    if (result.reason == eFindBestStructureResultReason::NO_RESULT) {
        TIMER_thinkwait = 10;
        return eHeadTowardsStructureResult::FAILED_NO_STRUCTURE_AVAILABLE;
    }

    cAbstractStructure *pStructure = result.structure;
    assert(pStructure && "Expected to have a valid pStructure pointer here");

    if (allowCarryallTransfer) {
        // TODO: Don't use cell based, but abs coordinates (pixel based) and use center
        int destCell = pStructure->getCell() + 2; // + 2 because refinery 'pad' is there.

        // try to get a carry-all to help when a bit bigger distance
        int distanceWeAllowDriving = 4;
        if (game.m_map.distance(iCell, destCell) > distanceWeAllowDriving) {
            if (findAndOrderCarryAllToBringMeToStructureAtCell(pStructure, destCell)) {
                return eHeadTowardsStructureResult::SUCCESS_AWAITING_CARRYALL;
            }
        }
    }

    // no Carry-all found/required, or we are close enough to drive
    move_to_enter_structure(pStructure, actionIntent);
    TIMER_movewait = 0;
    return eHeadTowardsStructureResult::SUCCESS_RETURNING;
}

bool cUnit::findAndOrderCarryAllToBringMeToStructureAtCell(cAbstractStructure *candidate, int destCell)
{
    int r = carryallFreeForTransfer(iPlayer);
    if (r < 0) {
        return false;
    }

    cUnit &carryAll = game.getUnit(r);
    carryAll.carryAll_transferUnitTo(iID, destCell);

    // todo: "getCellWhereToBringUnit"? where to enter structure so to speak?
    awaitBeingPickedUpToBeTransferedByCarryAllToStructure(candidate);

    // TODO: somehow remember we want to do something here!?
    return true;
}

void cUnit::carryAll_transferUnitTo(int unitIdToTransfer, int destinationCell)
{
    carryall_order(unitIdToTransfer, eTransferType::PICKUP, destinationCell, -1);
}

void cUnit::awaitBeingPickedUpToBeTransferedByCarryAllToStructure(cAbstractStructure *candidate)
{
    TIMER_movewait = 650; // wait for pickup!
    TIMER_thinkwait = 650;
    if (!candidate->hasUnitHeadingTowards() && !candidate->hasUnitWithin()) {
        candidate->unitHeadsTowardsStructure(iID);
        iStructureID = candidate->getStructureId();
    }
    else {
        // we chose to head towards this structure, but we can't claim it; this is
        // the situation that more units than available structures are heading towards the same structure
        // in this case we only remember the structureId, as we still have the intent to head towards it.
        // we let the other code that with "entering a structure" deal with any possibly already occupied
        // structure stuff.
        iStructureID = candidate->getStructureId();
    }
}

void cUnit::hideUnit()
{
    iTempHitPoints = iHitPoints;
    iHitPoints = -1; // 'kill' unit
}

bool cUnit::belongsTo(const cPlayer *pPlayer) const
{
    if (pPlayer == nullptr) return false;
    return pPlayer->getId() == iPlayer;
}

bool cUnit::isMarkedForRemoval()
{
    return bRemoveMe;
}

bool cUnit::isWithinViewport(cRectangle *viewport) const
{
    if (viewport == nullptr) return false;
    return dimensions.isOverlapping(viewport);
}

void cUnit::draw_debug(cTextDrawer* textDrawer)
{
    global_renderDrawer->renderRectColor(dimensions.getX(),dimensions.getY(), dimensions.getWidth(),dimensions.getHeight(), Color{255, 0, 255,ShadowTrans});
    global_renderDrawer->renderDot(center_draw_x(), center_draw_y(), Color{255, 0, 255,ShadowTrans},1);
    textDrawer->drawText(draw_x(), draw_y(), Color{255, 255, 255,ShadowTrans}, std::format("{}", iID));
    if (isSandworm()) {
        textDrawer->drawText(draw_x(), draw_y()-16, Color{255,255,255,255}, std::format("{} / {} / {}", unitsEaten, TIMER_guard, TIMER_movewait));
    }
}

void cUnit::takeDamage(int damage, int unitWhoDealsDamage, int structureWhoDealsDamage)
{
    iHitPoints -= damage;
    if (isDead()) {
        die(true, false);
    }
    else {
        if (boundParticleId < 0) {
            if (getUnitInfo().renderSmokeOnUnitWhenThresholdMet) {
                if (getHealthNormalized() < getUnitInfo().smokeHpFactor) {
                    int particleId = cParticle::create(
                                         pos_x_centered(),
                                         pos_y_centered(),
                                         D2TM_PARTICLE_SMOKE, // non-shadow smoke looks better on unit
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
        else {
            auto originType = eBuildType::UNKNOWN;
            auto originId = -1;
            auto originCell = -1;
            if (unitWhoDealsDamage > -1) {
                cUnit &pUnit = game.getUnit(unitWhoDealsDamage);
                if (pUnit.isValid()) {
                    originType = eBuildType::UNIT;
                    originId = unitWhoDealsDamage;
                    originCell = pUnit.iCell;
                }
            }
            else if (structureWhoDealsDamage > -1) {
                cAbstractStructure *pStructure = game.m_pStructures[structureWhoDealsDamage];
                if (pStructure) { // can be NULL (destroyed after firing this bullet)
                    originId = structureWhoDealsDamage;
                    originType = eBuildType::STRUCTURE;
                    originCell = pStructure->getCell();
                }
            }
            s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_DAMAGED,
                .entityType = eBuildType::UNIT,
                .entityID = iID,
                .player = getPlayer(),
                .entitySpecificType = getType(),
                .atCell = originCell,
                .originId = originId,
                .originType = originType,
            };

            game.onNotifyGameEvent(event);
        }
    }
}

bool cUnit::isEligibleForRepair()
{
    return isDamaged() && !isInfantryUnit() && !isAirbornUnit();
}

bool cUnit::isAbleToGuard()
{
    return getUnitInfo().canGuard;
}

// called every 5 ms
// used for movement logic
void cUnit::thinkFast()
{
    thinkFast_position();

    // aircraft
    if (isAirbornUnit()) {
        thinkFast_move_airUnit();
    }
    else {
        // move
        if (m_action == eActionType::MOVE || m_action == eActionType::CHASE || isMovingBetweenCells()) {
            thinkFast_move();
        }
    }

    // guard
    if (m_action == eActionType::GUARD) {
        thinkFast_guard();
    }
}

void cUnit::thinkFast_guard_sandworm()
{
    int iDistance = 9999;
    int unitIdToAttack = -1;

    for (int i = 0; i < game.m_Units.size(); i++) {
        cUnit &potentialDinner = game.getUnit(i);
        if (i == iID) continue;
        if (!potentialDinner.isValid()) continue;
        if (potentialDinner.getPlayer()->isSameTeamAs(getPlayer())) continue;
        if (potentialDinner.isAirbornUnit()) continue;
        if (potentialDinner.isSandworm()) continue; // don't eat other worms

        double distance = game.m_map.distance(iCell, potentialDinner.iCell);

        if (distance <= getSight() && distance < iDistance) {
            iDistance = distance;
            unitIdToAttack = i;
        }
    }

    if (unitIdToAttack > -1) {
        cUnit &unitToAttack = game.getUnit(unitIdToAttack);

        if (unitToAttack.isValid()) {
//            s_GameEvent event {
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
int cUnit::findNearbyGroundUnitToAttack(int range)
{
    // TODO: it would be nice if we could have a 'predicate' lambda here, so we can use that in the for loop
    // and have 1 function to do some of the logic. For now it is very explicit and duplicated.
    // See also findNearbyAirUnitToAttack, or the thinkFast_guard_sandworm logic.

    // TODO: another improvement would be to return list of ids in range, so we can order them
    // later by priority?

    int iDistance = 9999;
    int unitIdToAttack = -1;

    for (int i = 0; i < game.m_Units.size(); i++) {
        if (i == iID) continue; // skip self
        cUnit &potentialThreat = game.getUnit(i);
        if (!potentialThreat.isValid()) continue;
        if (potentialThreat.belongsTo(getPlayer())) continue; // skip own units
        if (potentialThreat.isAirbornUnit()) continue; // skip all airborn units (only focus on ground units)
        if (getPlayer()->isSameTeamAs(potentialThreat.getPlayer())) continue; // skip same team players / allies
        if (!game.m_map.isVisible(potentialThreat.iCell, iPlayer)) continue; // skip non-visible potential enemy units

        int distance = game.m_map.distance(iCell, potentialThreat.iCell);

        if (distance <= range && distance < iDistance) {
            iDistance = distance;
            unitIdToAttack = i;
        }
    }

    return unitIdToAttack;
}

int cUnit::findNearbyAirUnitToAttack(int range)
{
    int iDistance = 9999;
    int airUnitToAttack = -1;

    for (int i = 0; i < game.m_Units.size(); i++) {
        if (i == iID) continue; // skip self
        cUnit &potentialThreat = game.getUnit(i);
        if (!potentialThreat.isValid()) continue;
        if (!potentialThreat.isAirbornUnit()) continue; // skip all non-airborn units right away
        if (potentialThreat.getPlayerId() == getPlayerId()) continue; // skip own units
        if (getPlayer()->isSameTeamAs(potentialThreat.getPlayer())) continue; // skip same team players / allies
        if (!potentialThreat.isAttackableAirUnit()) continue;
        if (!game.m_map.isVisible(potentialThreat.iCell, iPlayer)) continue; // skip non-visible potential enemy units

        int distance = game.m_map.distance(iCell, potentialThreat.iCell);

        if (distance <= range &&
                distance < iDistance) { // closer than found thus far
            iDistance = distance;
            airUnitToAttack = i;
        }
    }

    return airUnitToAttack;
}

int cUnit::findNearbyStructureThatCanDamageUnitsToAttack(int range)
{
    // TODO: have some kind of lambda here as well

    // TODO: another improvement would be to return list of ids in range, so we can order them
    // later by priority?
    int structureIdToAttack = -1;
    int iDistance = 9999;

    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = game.m_pStructures[i];
        if (!pStructure) continue;
        if (!pStructure->isValid()) continue;
        if (getPlayer()->isSameTeamAs(pStructure->getPlayer())) continue;
        if (!game.m_map.isStructureVisible(pStructure, iPlayer)) continue; // not visible

        // ignore structures which cannot attack air or ground units
        if (!pStructure->canAttackAirUnits()) continue;
        if (!pStructure->canAttackGroundUnits()) continue;

        // see big comment about this in findNearbyStructureToAttack
        int distance = game.m_map.distance(iCell, pStructure->getRandomStructureCell());

        if (distance <= range && distance < iDistance) {
            iDistance = distance;
            structureIdToAttack = i;
        }
    }

    return structureIdToAttack;
}

int cUnit::findNearbyStructureToAttack(int range)
{
    // TODO: have some kind of lambda here as well

    // TODO: another improvement would be to return list of ids in range, so we can order them
    // later by priority?
    int structureIdToAttack = -1;
    int iDistance = 9999;

    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = game.m_pStructures[i];
        if (!pStructure) continue;
        if (!pStructure->isValid()) continue;
        if (getPlayer()->isSameTeamAs(pStructure->getPlayer())) continue;
        if (!game.m_map.isStructureVisible(pStructure, iPlayer)) continue; // not visible

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
        int distance = game.m_map.distance(iCell, pStructure->getRandomStructureCell());

        if (distance <= range && distance < iDistance) {
            iDistance = distance;
            structureIdToAttack = i;
        }
    }

    return structureIdToAttack;
}

// void cUnit::think_MVC() {
//     cPlayer *pPlayer = getPlayer();
//     if (pPlayer->isHuman()) {
//         // TODO: React upon keypress and then issue a command to deploy MCV instead of using this hacky via think function
//         if (bSelected) {
//             if (key[SDL_SCANCODE_D]) {
//                 bool result = pPlayer->canPlaceStructureAt(iCell, CONSTYARD, iID).success;

//                 if (result) {
//                     int iLocation = iCell;

//                     die(false, false);

//                     // place const yard
//                     pPlayer->placeStructure(iLocation, CONSTYARD, 100);
//                     return;
//                 }
//             }
//         }
//     }
// }

int cUnit::getTurnSpeed()
{
    return game.unitInfos[iType].turnspeed;
}

void cUnit::think_harvester()
{
    bool bFindRefinery = false;

    // cell = goal cell (doing nothing)
    if (iCell == movement.iGoalCell) {
        int cellType = game.m_map.getCellType(iCell);
        // when on spice, harvest
        if (cellType == TERRAIN_SPICE ||
                cellType == TERRAIN_SPICEHILL) {
            // do timer stuff
            if (iCredits < getUnitInfo().credit_capacity)
                TIMER_harvest++;
        }
        else {
            // not on spice, find a new location
            if (iCredits < getUnitInfo().credit_capacity) {
                // find harvest cell
                move_to(findHarvestSpot(iID), -1, -1);
            }
            else {
                rendering.iFrame = 0;
                bFindRefinery = true;
                // find a refinery
            }
        }

        if (iCredits >= getUnitInfo().credit_capacity)
            bFindRefinery = true;

        // when we should harvest...
        cPlayerDifficultySettings *difficultySettings = game.getPlayer(iPlayer).getDifficultySettings();
        if (TIMER_harvest > (difficultySettings->getHarvestSpeed(game.unitInfos[iType].harvesting_speed)) &&
                iCredits < getUnitInfo().credit_capacity) {
            TIMER_harvest = 1;

            rendering.iFrame++;

            if (rendering.iFrame > 3)
                rendering.iFrame = 1;

            iCredits += game.unitInfos[iType].harvesting_amount;
            game.m_map.cellTakeCredits(iCell, game.unitInfos[iType].harvesting_amount);

            // turn into sand/spice (when spicehill)
            if (game.m_map.getCellCredits(iCell) <= 0) {
                if (cellType == TERRAIN_SPICEHILL) {
                    game.m_map.cellChangeType(iCell, TERRAIN_SPICE);
                    game.m_map.cellGiveCredits(iCell, RNG::rnd(100));
                }
                else {
                    game.m_map.cellChangeType(iCell, TERRAIN_SAND);
                    game.m_map.cellChangeTile(iCell, 0);
                }

                move_to(findHarvestSpot(iID), -1, -1);

                cMapEditor(game.m_map).smoothAroundCell(iCell);
            }
        }

        // refinery required, go find one that is available
        if (bFindRefinery) {
            findBestStructureCandidateAndHeadTowardsItOrWait(REFINERY, true, INTENT_UNLOAD_SPICE);
            return;
        }
    }
    else {
        // ??
        rendering.iFrame = 0;
    }
}

void cUnit::setGoalCell(int goalCell)
{
    log(std::format("setGoalCell() from {} to {}", movement.iGoalCell, goalCell));
    movement.iGoalCell = goalCell;
}

void cUnit::setAction(eActionType action)
{
    log(std::format("setAction() from {} to {}", eActionTypeString(m_action), eActionTypeString(action)));
    m_action = action;
}

void cUnit::retreatToNearbyBase()
{
    const std::vector<sEntityForDistance> &result = getPlayer()->getAllMyStructuresOrderClosestToCell(iCell);
    if (result.empty()) {
        // don't know where to retreat to :/
        return;
    }
    const sEntityForDistance &closest = result[0];
    cAbstractStructure *pStructure = game.m_pStructures[closest.entityId];
    // use the 'drop location' function, as it will circle around a given cell until a valid cell is found
    int cellToRetreatTo = findNewDropLocation(iType, pStructure->getCell());

    move_to(cellToRetreatTo); // intent retreat?
}

void cUnit::deselect() {
    this->m_bSelected = false;
}

void cUnit::select() {
    this->m_bSelected = true;
}

bool cUnit::isSelected() const {
    return m_bSelected;
}


std::string cUnit::getHarvesterStatusForMessageBar()
{
    s_UnitInfo &info = getUnitInfo();
    int harvested = ((float)iCredits/info.credit_capacity)*100;
    return std::format("{} at {} percent health and {} percent harvested", info.name, getHealthNormalized()*100, harvested);
}

std::string cUnit::getUnitStatusForMessageBar()
{
    s_UnitInfo &info = getUnitInfo();
    switch (getType()) {
        case HARVESTER:
            return getHarvesterStatusForMessageBar();
        default:
            return std::format("{} at {} percent health", info.name, getHealthNormalized()*100);
    }

}

bool cUnit::isAttackingUnit()
{
    return getUnitInfo().canAttackAirUnits || getUnitInfo().canAttackUnits;
}

void cUnit::takeDamage(int damage)
{
    takeDamage(damage, -1, -1);
}

bool cUnit::isHidden()
{
    // hidden (entered structure, etc).
    return iTempHitPoints > -1;
}

// Hacky function that checks temp hitpoints (if it requires repairing)
bool cUnit::requiresRepairing()
{
    return iTempHitPoints < getUnitInfo().hp;
}

void cUnit::repair(int hp)
{
    iTempHitPoints += hp;
}

bool cUnit::canUnload()
{
    return iCredits > 0;
}

int cUnit::findHarvestSpot(int id)
{
    // finds the closest harvest spot
    cUnit &cUnit = game.getUnit(id);
    cUnit.updateCellXAndY();
    int cx = cUnit.getCellX();
    int cy = cUnit.getCellY();

    int TargetSpice = -1;
    int TargetSpiceHill = -1;

    // distances
    int TargetSpiceDistance = 40;
    int TargetSpiceHillDistance = 40;


    for (int i = 0; i < game.m_map.getMaxCells(); i++)
        if (game.m_map.getCellCredits(i) > 0 && i != cUnit.getCell()) {
            // check if its not out of reach
            int dx = game.m_map.getCellX(i);
            int dy = game.m_map.getCellY(i);

            // skip bordered ones
            if (game.m_map.isWithinBoundaries(dx, dy) == false)
                continue;

            /*
            if (dx <= 1) continue;
            if (dy <= 1) continue;

            if (dx >= (game.map_width-1))
              continue;

            if (dy >= (game.map_height-1))
              continue;*/

            int idOfUnitAtCell = game.m_map.getCellIdUnitLayer(i);
            if (idOfUnitAtCell > -1)
                continue;

            if (game.m_map.occupied(i, id))
                continue; // occupied

            int d = ABS_length(cx, cy, game.m_map.getCellX(i), game.m_map.getCellY(i));

            int cellType = game.m_map.getCellType(i);
            if (cellType == TERRAIN_SPICE) {
                if (d < TargetSpiceDistance) {
                    TargetSpice = i;
                    TargetSpiceDistance = d; // update distance
                }
            }
            else if (cellType == TERRAIN_SPICEHILL) {
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
    }
    else {
        // when both are close, prefer spice hill
        if (TargetSpiceHill > -1 && (TargetSpiceHillDistance <= TargetSpiceDistance))
            return TargetSpiceHill;
    }

    return TargetSpice;
}

int cUnit::carryallFreeForTransfer(int iPlayer)
{
    // find a free carry all
    for (int i = 0; i < game.m_Units.size(); i++) {
        cUnit &cUnit = game.getUnit(i);
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
int cUnit::carryallTransfer(int iuID, int iGoal)
{
    int carryAllUnitId = carryallFreeForTransfer(game.getUnit(iuID).iPlayer);
    if (carryAllUnitId > -1) {
        cUnit &cUnit = game.getUnit(carryAllUnitId);
        cUnit.carryall_order(iuID, eTransferType::PICKUP, iGoal, -1);
    }
    return carryAllUnitId;
}

int cUnit::freeAroundMove(int iUnit)
{
    if (iUnit < 0) {
        logbook("Invalid unit");
        return -1;
    }

    cUnit &cUnit = game.getUnit(iUnit);

    cUnit.updateCellXAndY();
    int iStartX = cUnit.getCellX();
    int iStartY = cUnit.getCellY();

    int iWidth = RNG::rnd(4);

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
            int cll = game.m_map.getGeometry().getCellWithMapBorders(x, y);

            if (cll > -1 && !game.m_map.occupied(cll)) {
                iClls[foundCoordinates] = cll;
                foundCoordinates++;
            }

        }
    }

    if (foundCoordinates < 1) return -1;

    return iClls[RNG::rnd(foundCoordinates)]; // random cell
}