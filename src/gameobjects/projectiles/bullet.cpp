/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "bullet.h"

#include "data/gfxdata.h"
#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "gameobjects/particles/cParticle.h"
#include "map/cMapCamera.h"
#include "map/cMapEditor.h"
#include "player/cPlayer.h"
#include "utils/cSoundPlayer.h"

#include <fmt/core.h>
#include <cmath>

static constexpr auto ANIMATION_SPEED = 12;

void cBullet::init()
{
    bAlive = false;
    bStartedFromMountain = false;

    iCell = -1;          // cell of bullet
    iType = -1;          // type of bullet

    // Movement
    posX = -1;
    posY = -1;
    targetX = -1;
    targetY = -1;

    iOwnerUnit = -1;     // unit who shoots
    iOwnerStructure = -1;// structure who shoots (rocket turret?)
    iPlayer = -1;

    iFrame = 0;           // frame (for rockets)

    TIMER_frame = 0;      // timer for switching frames

    iHoming = -1;        // homing to unit...
    TIMER_homing = 0;   // when timer set, > 0 means homing
}

int cBullet::pos_x() const
{
    return posX;
}

int cBullet::pos_y() const
{
    return posY;
}

int cBullet::draw_x()
{
    int bmpOffset = (getBulletBmpWidth()/2) * -1;
    return mapCamera->getWindowXPositionWithOffset(pos_x(), bmpOffset);
}

int cBullet::draw_y()
{
    int bmpOffset = (getBulletBmpHeight()/2) * -1;
    return mapCamera->getWindowYPositionWithOffset(pos_y(), bmpOffset);
}

// draw the bullet
void cBullet::draw()
{
    int x = draw_x();
    int y = draw_y();

    if (x < -getBulletBmpWidth() || x > (game.m_screenW + getBulletBmpWidth()))
        return;

    if (y < getBulletBmpHeight() || y > game.m_screenH + getBulletBmpHeight())
        return;

    int ba = bullet_face_angle(fDegrees(posX, posY, targetX, targetY));

    int h = 32;
    int bmp_width = getBulletBmpWidth();


    // source X,Y image coordinates?
    int sx, sy;

    sy = iFrame * h;

    if (iType == ROCKET_BIG) {
        sy = (iFrame * 48);
    }

    if (iType == ROCKET_SMALL ||
            iType == ROCKET_SMALL_FREMEN ||
            iType == ROCKET_SMALL_ORNI) {
        sy = (iFrame * 16);
    }

    if (iType != BULLET_SMALL &&
            iType != BULLET_TRIKE &&
            iType != BULLET_QUAD &&
            iType != BULLET_TANK &&
            iType != BULLET_SIEGE &&
            iType != BULLET_DEVASTATOR &&
            iType != BULLET_TURRET) {
        sx = ba * getBulletBmpWidth();
    }
    else {
        sx = 0;
    }

    // Whenever this bullet is a shimmer, draw a shimmer and leave
    if (iType == BULLET_SHIMMER) {
        renderDrawer->shimmer(mapCamera->factorZoomLevel(16), x, y, mapCamera->factorZoomLevel(4));
        return;
    }

    if (sBulletInfo[iType].bmp != nullptr) {
        renderDrawer->maskedStretchBlit(sBulletInfo[iType].bmp,
                                        bmp_screen,
                                        sx, sy,
                                        bmp_width, bmp_width,
                                        x, y,
                                        mapCamera->factorZoomLevel(bmp_width), mapCamera->factorZoomLevel(bmp_width));
    }
}

int cBullet::getBulletBmpWidth() const
{
    return sBulletInfo[iType].bmp_width;
}

int cBullet::getBulletBmpHeight() const
{
    return getBulletBmpWidth(); // the bullets are always assumed to be a square (height==width)
}


// called every 5 ms
void cBullet::thinkFast()
{
    int maxFrames = gets_Bullet().max_frames;
    if (maxFrames > 0) {
        // frame animation first
        TIMER_frame++;

        if (TIMER_frame > ANIMATION_SPEED) {
            iFrame++;
            if (iFrame > maxFrames) { // fire animation of rocket
                iFrame = 0;
            }

            int smokeParticle = gets_Bullet().smokeParticle;
            if (smokeParticle > -1) {
                long x = pos_x();
                long y = pos_y();
                cParticle::create(x, y, smokeParticle, -1, -1);
            }

            TIMER_frame = 0;
        }
    }

    // when this bastard is homing... set goal
    if (TIMER_homing > 0) {
        TIMER_homing--;

        if (iHoming > -1) {
            if (unit[iHoming].isValid()) {
                int cll = unit[iHoming].getCell();
                targetX = map.getAbsoluteXPositionFromCell(cll);
                targetY = map.getAbsoluteYPositionFromCell(cll);
            }
        }
    }

    think_move();
}


void cBullet::think_move()
{
    iCell = mapCamera->getCellFromAbsolutePosition(posX, posY);

    int iCellX = map.getCellX(iCell);
    int iCellY = map.getCellY(iCell);

    // out of bounds somehow; then die
    if (!map.isWithinBoundaries(iCellX, iCellY)) {
        die();
        return;
    }

    // move bullet a bit towards its goal
    moveBulletTowardsGoal();

    if (isAtDestination()) {
        arrivedAtDestinationLogic();
        return;
    }

    int idOfStructureAtCell = map.getCellIdStructuresLayer(iCell);
    int cellTypeAtCell = map.getCellType(iCell);

    if (!isGroundBullet()) {
        return;
    }

    // from here, all bullets that are not 'flying' (ie can't get over things).
    // these bullets will be blocked by other buildings, walls, mountains

    // hit structures, walls and mountains only when being fired from 'below'
    if (!bStartedFromMountain) {
        if (cellTypeAtCell == TERRAIN_MOUNTAIN) {
            arrivedAtDestinationLogic();
            return;
        }

        // non flying bullets hit against a wall, except for bullets from turrets
        if (cellTypeAtCell == TERRAIN_WALL) {
            arrivedAtDestinationLogic();
            return;
        }
    }

    if (idOfStructureAtCell > -1) {
        // structures block non-flying bullets, except when it is from the structure
        // which spawned the bullet. It will also 'shoot over' our own buildings.
        int id = idOfStructureAtCell;
        bool bHitsEnemyBuilding = false;

        if (iOwnerStructure > -1) {
            if (id != iOwnerStructure) {
                bHitsEnemyBuilding = true;
            }
            else {
                // do not hit own or allied structures
                if (!structure[id]->getPlayer()->isSameTeamAs(getPlayer())) {
                    bHitsEnemyBuilding = true;
                }
            }
        }
        else {
            // OG Dune 2 effect: Hit structures
            bHitsEnemyBuilding = true;
        }

        if (bHitsEnemyBuilding) {
            arrivedAtDestinationLogic();
            return;
        }
    }
}

void cBullet::arrivedAtDestinationLogic()
{
    //
    // we handle different kind of places where we can inflict damage
    // and we don't bail out after doing that, allowing to inflict damage on several layers on the battle field
    // for instance: damage a wall, but also a unit (ornithopter), and so forth
    //

    const s_BulletInfo &sBullet = gets_Bullet();

    // damage is inflicted to size of explosion
    int x = map.getCellX(iCell);
    int y = map.getCellY(iCell);

    int halfExplosionSize = std::round((float)(sBullet.explosionSize / 2));
    int startX = (x - halfExplosionSize);
    int startY = (y - halfExplosionSize);
    int endX = x + (halfExplosionSize + 1);
    int endY = y + (halfExplosionSize + 1);

    float maxDistanceFromCenter = halfExplosionSize + 0.5f;
    for (int sx = startX; sx < endX; sx++) {
        for (int sy = startY; sy < endY; sy++) {
            int cellToDamage = map.getCellWithMapBorders(sx, sy);
            if (cellToDamage < 0) continue;

            float actualDistance = ABS_length(sx, sy, x, y);
            if (actualDistance > maxDistanceFromCenter) actualDistance = maxDistanceFromCenter;

            float factor = 1.0f;
            if (actualDistance > 1.0f) {
                factor = 1.0f - (actualDistance / maxDistanceFromCenter);
            }

            int half = 16;
            int randomX = -8 + rnd(half);
            int randomY = -8 + rnd(half);
            int posX = map.getAbsoluteXPositionFromCellCentered(cellToDamage) + randomX;
            int posY = map.getAbsoluteYPositionFromCellCentered(cellToDamage) + randomY;

            logbook(fmt::format(
                        "iCell {} : cellToDamage : {} : ExplosionSize is {}, maxDistanceFromCenter is {} , actualDistance = {}, x={}, y={} and factor = {}",
                        iCell, cellToDamage, sBullet.explosionSize, maxDistanceFromCenter, actualDistance, sx, sy, factor));

            // when air layer is hit, it won't damage ground things
            if (!damageAirUnit(cellToDamage, factor)) {                // inflict damage on air unit (if rocket)
                damageStructure(cellToDamage, factor);                 // damage structure at cell if applicable
                damageWall(cellToDamage, factor);                      // damage wall if applicable

                if (!damageGroundUnit(cellToDamage, factor)) {         // inflict damage on ground unit if applicable
                    // only inflict damage when nothing 'above it' (ie a ground unit) is hit first.
                    damageSandworm(cellToDamage, factor);              // inflict damage on sandworm if applicable

                    detonateSpiceBloom(cellToDamage);          // detonate spice bloom if applicable
                }
            }

            // create particle of explosion
            if (sBullet.deathParticle > -1) {
                // depending on 'explosion size'
                cParticle::create(posX, posY, sBullet.deathParticle, -1, -1);
            }

            if (iType == ROCKET_BIG) {
                // HACK HACK: produce sounds here... should be taken from bullet data structure; or via events
                // so that elsewhere this can be handled.
                if (rnd(100) < 35) {
                    game.playSoundWithDistance(SOUND_TANKDIE + rnd(2),
                                               distanceBetweenCellAndCenterOfScreen(cellToDamage));
                }
                if (rnd(100) < 20) {
                    cParticle::create(posX, posY, D2TM_PARTICLE_SMOKE_WITH_SHADOW, -1, -1);
                }
            }

            damageTerrain(cellToDamage, factor);
        }
    }

    if (iType == ROCKET_BIG) {
        game.shakeScreen(40);
    }

    die();
}

/**
 * Only when bullet can damage ground, this function will do something.
 * @param cell
 * @param factor
 */
void cBullet::damageTerrain(int cell, double factor) const
{
    if (!map.isValidCell(cell)) return;
    if (!canDamageGround()) return;

    float iDamage = getDamageToInflictToNonInfantry() * factor;

    int idOfStructureAtCell = map.getCellIdStructuresLayer(cell);
    int cellTypeAtCell = map.getCellType(cell);

    map.cellTakeDamage(cell, iDamage);

    if (cellTypeAtCell == TERRAIN_SLAB) {
        // change into rock, get destroyed. But only when we did not hit a structure.
        if (idOfStructureAtCell < 0) {
            map.cellChangeType(cell, TERRAIN_ROCK);
            cMapEditor(map).smoothAroundCell(cell);
        }
    }
}

bool cBullet::isInfantryBullet() const
{
    return iType == BULLET_SMALL;
}

bool cBullet::isSonicWave() const
{
    return iType == BULLET_SHIMMER;
}

/**
 * Returns true if air unit exists and can be damaged. Takes same team into account.
 *
 * @param unitIdOnAirLayer
 * @return
 */
bool cBullet::doesAirUnitTakeDamage(int unitIdOnAirLayer) const
{
    if (unitIdOnAirLayer < 0) return false;
    if (iOwnerUnit > 0 && unitIdOnAirLayer == iOwnerUnit) return false; // do not damage self

    cUnit &airUnit = unit[unitIdOnAirLayer];
    if (iOwnerUnit <= 0) {
        return false; // no air unit to damage
    }

    cUnit &ownerUnit = unit[iOwnerUnit];
    if (!ownerUnit.isValid()) {
        return false; // unit is not 'valid'
    }

    if (ownerUnit.getPlayer()->isSameTeamAs(airUnit.getPlayer())) {
        return false; // Do not damage same team
    }

    // yes, an air unit is present, is not of my team and can should be damaged.
    return true;
}

/**
 * Handle damaging at cell, returns true if an (non-owner) unit is damaged.
 * Returns false when cell is invalid, bullet cannot damage air units, or if unit at cell is same as owner of this bullet.
 */
bool cBullet::damageAirUnit(int cell, double factor) const
{
    if (!map.isValidCell(cell)) return false;
    if (!canDamageAirUnits()) return false;
    int unitIdOnAirLayer = map.getCellIdAirUnitLayer(cell);

    float iDamage = getDamageToInflictToNonInfantry() * factor;

    if (doesAirUnitTakeDamage(unitIdOnAirLayer)) {
        cUnit &airUnit = unit[unitIdOnAirLayer];
        airUnit.takeDamage(iDamage, iOwnerUnit, iOwnerStructure);
        return true;
    }

    return false;
}

/**
 * Inflicts damage on ground unit if it exists at this cell.
 * If cell param is invalid or no ground unit exists at (valid) cell, then this method aborts.
 * @param cell
 */
bool cBullet::damageGroundUnit(int cell, double factor) const
{
    if (!map.isValidCell(cell)) return false;
    int id = map.getCellIdUnitLayer(cell);
    if (id < 0) return false;
    if (iOwnerUnit > 0 && id == iOwnerUnit) return false; // do not damage self

    cUnit &groundUnitTakingDamage = unit[id];

    float iDamage = getDamageToInflictToUnit(groundUnitTakingDamage) * factor;
    groundUnitTakingDamage.takeDamage(iDamage, iOwnerUnit, iOwnerStructure);

    // this unit will think what to do now (he got hit ouchy!)
    groundUnitTakingDamage.think_hit(iOwnerUnit, iOwnerStructure);

    // NO HP LEFT, DIE
    if (groundUnitTakingDamage.isDead()) {
        // who is to blame for killing this unit?
        if (iOwnerUnit > -1) {
            cUnit &ownerUnit = unit[iOwnerUnit];
            if (ownerUnit.isValid()) {
                // TODO: update statistics

                if (sUnitInfo[groundUnitTakingDamage.iType].infantry) {
                    ownerUnit.fExperience += 0.25; // 4 kills = 1 star
                }
                else {
                    ownerUnit.fExperience += 0.45; // ~ 3 kills = 1 star
                }
            }
        }
    }

    if (gets_Bullet().deviateProbability > 0 && iOwnerUnit > -1) {
        // can deviate a unit upon impact

        // TODO: Stefan: is this needed?!- aren't we playing sound effects in a more generic way?
        // TODO: impact sound effect should be configured!?
        game.playSoundWithDistance(SOUND_GAS, distanceBetweenCellAndCenterOfScreen(cell));

        // take over unit
        if (rnd(100) < gets_Bullet().deviateProbability) {
            cUnit &ownerUnit = unit[iOwnerUnit];
            if (ownerUnit.isValid()) {
                groundUnitTakingDamage.iPlayer = ownerUnit.iPlayer;
                groundUnitTakingDamage.iGroup = -1;

                // send out event that this unit got deviated (and what the new owner ID is)
                s_GameEvent event {
                    .eventType = eGameEventType::GAME_EVENT_DEVIATED,
                    .entityType = eBuildType::UNIT,
                    .entityID = groundUnitTakingDamage.iID,
                    .player = groundUnitTakingDamage.getPlayer(), // <-- is now changed
                    .entitySpecificType = groundUnitTakingDamage.iType
                };

                game.onNotifyGameEvent(event);
            }
        }
    }
    return true;
}

/**
 * Given a unit that is taking damage, decide what kind and how many damage to inflict
 * @param unitTakingDamage
 * @return
 */
float cBullet::getDamageToInflictToUnit(cUnit &unitTakingDamage) const
{
    if (unitTakingDamage.isInfantryUnit()) {
        return getDamageToInflictToInfantry();
    }
    return getDamageToInflictToNonInfantry();
}

float cBullet::getDamageToInflictToInfantry() const
{
    cPlayerDifficultySettings *difficultySettings = getDifficultySettings();

    float result = difficultySettings->getInflictDamage(sBulletInfo[iType].damage_inf);

    if (iOwnerUnit > -1) {
        float fDam = unit[iOwnerUnit].fExpDamage() * result;
        result += fDam;
    }
    return result;
}

/**
 * If cell is TERRAIN_BLOOM, then it will detonate the spice bloom.
 * if cell is invalid, or terrain is not TERRAIN_BLOOM; it will abort.
 * @param cell
 */
void cBullet::detonateSpiceBloom(int cell) const
{
    map.detonateSpiceBloom(cell);
}

void cBullet::damageSandworm(int cell, double factor) const
{
    if (!map.isValidCell(cell)) return;
    int id = map.getCellIdWormsLayer(cell);
    if (id < 0) return; // bail

    cUnit &worm = unit[id];
    float damage = getDamageToInflictToNonInfantry() * factor;
    worm.takeDamage(damage, iOwnerUnit, iOwnerStructure);
}

bool cBullet::isAtDestination() const
{
    int distanceX = abs(targetX - posX);
    int distanceY = abs(targetY - posY);
    return distanceX < 2 && distanceY < 2;
}

/**
 * If provided cell is of TERRAIN_WALL, then damage it.
 * If the terrain is not TERRAIN_WALL, then it aborts.
 * If cell param provided is invalid, it aborts.
 * @param cell
 */
void cBullet::damageWall(int cell, double factor) const
{
    if (!map.isValidCell(cell)) return;
    int cellTypeAtCell = map.getCellType(cell);
    if (cellTypeAtCell != TERRAIN_WALL) return;

    float iDamage = getDamageToInflictToNonInfantry() * factor;

    map.cellTakeDamage(cell, iDamage);

    if (map.getCellHealth(cell) < 0) {
        // remove wall, turn into smudge:
        auto mapEditor = cMapEditor(map);
        mapEditor.createCell(cell, TERRAIN_ROCK, 0);
        mapEditor.smoothAroundCell(cell);
        map.smudge_increase(SMUDGE_WALL, cell);
    }
}

void cBullet::moveBulletTowardsGoal()
{
    // step 1 : look to the correct direction
    float angle = fRadians(posX, posY, targetX, targetY);

    // now do some thing to make
    // 1/8 of a cell (2 pixels) per movement
    int movespeed = 2; // this is fixed! (TODO: move this to bullet type data)
    posX += cos(angle) * movespeed;
    posY += sin(angle) * movespeed;
}

bool cBullet::isGroundBullet() const
{
    return gets_Bullet().groundBullet;
}

bool cBullet::canDamageAirUnits() const
{
    return gets_Bullet().canDamageAirUnits;
}

float cBullet::getDamageToInflictToNonInfantry() const
{
    cPlayerDifficultySettings *pDifficultySettings = getDifficultySettings();
    const s_BulletInfo &sBullet = gets_Bullet();
    float iDamage = pDifficultySettings->getInflictDamage(sBullet.damage);

    // increase damage by experience of unit
    if (iOwnerUnit > -1) {
        // extra damage by experience:
        cUnit &cUnit = unit[iOwnerUnit];
        if (cUnit.isValid()) { // in case the unit died while firing
            float iDam = (cUnit.fExpDamage() * iDamage);
            iDamage = iDamage + iDam;
        }
    }

    return iDamage;
}

cPlayerDifficultySettings *cBullet::getDifficultySettings() const
{
    const cPlayer *cPlayer = getPlayer();
    cPlayerDifficultySettings *pDifficultySettings = cPlayer->getDifficultySettings();
    return pDifficultySettings;
}

s_BulletInfo cBullet::gets_Bullet() const
{
    return sBulletInfo[iType];
}

cPlayer *cBullet::getPlayer() const
{
    return &players[iPlayer];
}

/**
 * Damage the structure; if id < 0 then this method does nothing.
 *
 * @param difficultySettings
 * @param idOfStructureAtCell
 */
void cBullet::damageStructure(int idOfStructureAtCell, double factor)
{
    if (!map.isValidCell(idOfStructureAtCell)) return;
    int id = map.getCellIdStructuresLayer(idOfStructureAtCell);
    if (id < 0) return; // bail

    cPlayerDifficultySettings *difficultySettings = getDifficultySettings();

    float iDamage = difficultySettings->getInflictDamage(sBulletInfo[iType].damage) * factor;

    cUnit *pUnit = nullptr;
    if (iOwnerUnit > -1) {
        if (unit[iOwnerUnit].isValid()) {
            pUnit = &unit[iOwnerUnit];
        }
    }

    if (pUnit) {
        int iDam = (pUnit->fExpDamage() * iDamage);
        iDamage += iDam;
    }

    cAbstractStructure *pStructure = structure[id];
    if (pStructure == nullptr) {
        return; // invalid pointer!
    }

    pStructure->damage(iDamage, iOwnerUnit);

    if (pStructure->isDead()) {
        if (pUnit) {
            // TODO: update statistics (structures destroyed ??)
        }
    }

}

/**
 * Picks a random Y position around current position. (max half tile distance)
 * @return
 */
int cBullet::getRandomY() const
{
    int half = 16;
    int randomY = -8 + rnd(half);
    return pos_y() + half + randomY;
}

/**
 * Picks a random X position around current position. (max half tile distance)
 * @return
 */
int cBullet::getRandomX() const
{
    int half = 16;
    int randomX = -8 + rnd(half);
    return pos_x() + half + randomX;
}

void cBullet::die()
{
    bAlive = false;
}

bool cBullet::canDamageGround() const
{
    return gets_Bullet().canDamageGround;
}
