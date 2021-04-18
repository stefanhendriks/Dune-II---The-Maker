/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "../../include/d2tmh.h"

#include <math.h>

void cBullet::init() {
    bAlive = false;

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

int cBullet::pos_x() const {
    return posX;
}

int cBullet::pos_y() const {
    return posY;
}

int cBullet::draw_x() {
    int bmpOffset = (getBulletBmpWidth()/2) * -1;
    return mapCamera->getWindowXPositionWithOffset(pos_x(), bmpOffset);
}

int cBullet::draw_y() {
    int bmpOffset = (getBulletBmpHeight()/2) * -1;
    return mapCamera->getWindowYPositionWithOffset(pos_y(), bmpOffset);
}

// draw the bullet
void cBullet::draw() {
    int x = draw_x();
    int y = draw_y();

    if (x < -getBulletBmpWidth() || x > (game.screen_x + getBulletBmpWidth()))
        return;

    if (y < getBulletBmpHeight() || y > game.screen_y + getBulletBmpHeight())
        return;

    int fa = bullet_face_angle(fDegrees(posX, posY, targetX, targetY));
    int ba = bullet_correct_angle(fa);

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
    } else {
        sx = 0;
    }

    // Whenever this bullet is a shimmer, draw a shimmer and leave
    if (iType == BULLET_SHIMMER) {
        Shimmer(mapCamera->factorZoomLevel(16), x, y);
        return;
    }

    if (bullets[iType].bmp != NULL) {
        BITMAP *src = (BITMAP *) bullets[iType].bmp;
        allegroDrawer->maskedStretchBlit(src,
                                         bmp_screen,
                                         sx, sy,
                                         bmp_width, bmp_width,
                                         x, y,
                                         mapCamera->factorZoomLevel(bmp_width), mapCamera->factorZoomLevel(bmp_width));
    }
}

int cBullet::getBulletBmpWidth() const {
    return bullets[iType].bmp_width;
}

int cBullet::getBulletBmpHeight() const {
    return getBulletBmpWidth(); // the bullets are always assumed to be a square (height==width)
}


void cBullet::think() {
    // frame animation first
    TIMER_frame++;

    if (TIMER_frame > 12) {
        bool bCreatePuf = false;

        // big rockets create smoke
        if (iType == ROCKET_NORMAL ||
            iType == BULLET_GAS ||
            iType == ROCKET_RTURRET ||
            iType == ROCKET_BIG) {

            iFrame++;
            if (iFrame > 1) { // fire animation of rocket
                iFrame = 0;
            }

            bCreatePuf = true;
        }

        // smaller rockets don't create smoke... except for ornithopters
        if (iType == ROCKET_SMALL || iType == ROCKET_SMALL_ORNI) {
            iFrame++;
            if (iFrame > 1) {
                iFrame = 0;
            }

            if (iType == ROCKET_SMALL_ORNI) {
                bCreatePuf = true;
            }

        }

        if (bCreatePuf) {
            PARTICLE_CREATE(pos_x(), pos_y(), BULLET_PUF, -1, -1);
        }

        TIMER_frame = 0;
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


void cBullet::think_move() {
    iCell = mapCamera->getCellFromAbsolutePosition(posX, posY);

    int iCellX = iCellGiveX(iCell);
    int iCellY = iCellGiveY(iCell);

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

    // still heading towards goal; we might hit something while flying
    if (isNonFlyingTerrainBullet()) {

        // hit structures, walls and mountains
        if (cellTypeAtCell == TERRAIN_MOUNTAIN) {
            die();
            return;
        }

        // non flying bullets hit against a wall, except for bullets from turrets
        if (cellTypeAtCell == TERRAIN_WALL && !isTurretBullet()) {
            damageWall(iCell, 0);
        }

        if (idOfStructureAtCell > -1) {
            // structures block non flying bullets, except when it is from the structure
            // which spawned the bullet. It will also 'shoot over' our own buildings.
            int id = idOfStructureAtCell;
            bool bHitsEnemyBuilding = false;

            if (isTurretBullet()) {
                if (id != iOwnerStructure) {
                    bHitsEnemyBuilding = true;
                } else {
                    // do not shoot yourself
                    if (structure[id]->getOwner() != iPlayer) {
                        bHitsEnemyBuilding = true;
                    }
                }
            }

            if (bHitsEnemyBuilding) {
                damageStructure(idOfStructureAtCell, 0);
            }
        }
    } // non-flying bullets
}

void cBullet::arrivedAtDestinationLogic() {
    //
    // we handle different kind of places where we can inflict damage
    // and we don't bail out after doing that, allowing to inflict damage on several layers on the battle field
    // for instance: damage a wall, but also a unit (ornithopter), and so forth
    //

    s_Bullet const &sBullet = gets_Bullet();

    // damage is inflicted to size of explosion
    int x = iCellGiveX(iCell);
    int y = iCellGiveY(iCell);

    int halfExplosionSize = std::round((float)(sBullet.explosionSize / 2));
    int startX = (x - halfExplosionSize);
    int startY = (y - halfExplosionSize);
    int endX = x + (halfExplosionSize + 1);
    int endY = y + (halfExplosionSize + 1);

    float maxDistanceFromCenter = halfExplosionSize + 0.5f;
    for (int sx = startX; sx < endX; sx++) {
        for (int sy = startY; sy < endY; sy++) {
            int iCx = sx;
            int iCy = sy;
            FIX_BORDER_POS(iCx, iCy);

            int cellToDamage = iCellMake(iCx, iCy);

            float actualDistance = ABS_length(iCx, iCy, x, y);
            if (actualDistance > maxDistanceFromCenter) actualDistance = maxDistanceFromCenter;

            float factor = 1.0f;
            if (actualDistance > 1.0f) {
                factor = 1.0f - (actualDistance / maxDistanceFromCenter);
            }

            char msg[255];
            sprintf(msg, "iCell %d : cellToDamage : %d : ExplosionSize is %d, maxDistanceFromCenter is %f , actualDistance = %f, iCx=%d, iCy=%d and factor = %f", iCell, cellToDamage, sBullet.explosionSize, maxDistanceFromCenter, actualDistance, iCx, iCy, factor);
            logbook(msg);

            damageStructure(cellToDamage, factor);                 // damage structure at cell if applicable
            damageWall(cellToDamage, factor);                      // damage wall if applicable
            detonateSpiceBloom(cellToDamage, factor);              // detonate spice bloom if applicable
            damageSandworm(cellToDamage, factor);                  // inflict damage on sandworm if applicable
            damageGroundUnit(cellToDamage, factor);                // inflict damage on ground unit if applicable
            damageAirUnit(cellToDamage, factor);                   // inflict damage on air unit (if rocket)

            // create particle of explosion
            if (sBullet.deadbmp > -1) {
                // depending on 'explosion size'
                int half = 16;
                int randomX = -8 + rnd(half);
                int randomY = -8 + rnd(half);
                int posX = map.getAbsoluteXPositionFromCellCentered(cellToDamage) + randomX;
                int posY = map.getAbsoluteYPositionFromCellCentered(cellToDamage) + randomY;
                PARTICLE_CREATE(posX, posY, sBullet.deadbmp, -1, -1);
            }

            damageTerrain(cellToDamage, factor);
        }
    }

    die();
}

void cBullet::damageTerrain(int cell, double factor) const {
    if (!bCellValid(cell)) return;
    float iDamage = getDamageToInflictToNonInfantry() * factor;

    int idOfStructureAtCell = map.getCellIdStructuresLayer(cell);
    int cellTypeAtCell = map.getCellType(cell);

    map.cellTakeDamage(cell, iDamage);

    if (cellTypeAtCell == TERRAIN_SLAB) {
        // change into rock, get destroyed. But only when we did not hit a structure.
        if (idOfStructureAtCell < 0 && !isInfantryBullet() && !isSonicWave()) {
            map.cellChangeType(cell, TERRAIN_ROCK);
            mapEditor.smoothAroundCell(cell);
        }
    }
}

bool cBullet::isInfantryBullet() const {
    return iType == BULLET_SMALL;
}

bool cBullet::isSonicWave() const {
    return iType == BULLET_SHIMMER;
}

/**
 * Handle damaging at cell, if cell is invalid or this bullet type is not a rocket, it will abort.
 */
void cBullet::damageAirUnit(int cell, double factor) const {
    if (!bCellValid(cell)) return;
    if (!isRocket()) return;
    int id = map.getCellIdAirUnitLayer(cell);
    if (id < 0) return;
    if (iOwnerUnit > 0 && id == iOwnerUnit) return; // do not damage self

    float iDamage = getDamageToInflictToNonInfantry() * factor;

    cUnit &airUnit = unit[id];
    airUnit.takeDamage(iDamage);

    if (airUnit.isDead()) {
        int iID = iOwnerUnit;

        if (iID > -1) {
            if (unit[iID].isValid()) {
                // TODO: update statistics
            }
        }
    }
}

/**
 * Inflicts damage on ground unit if it exists at this cell.
 * If cell param is invalid or no ground unit exists at (valid) cell, then this method aborts.
 * @param cell
 */
void cBullet::damageGroundUnit(int cell, double factor) const {
    if (!bCellValid(cell)) return;
    int id = map.getCellIdUnitLayer(cell);
    if (id < 0) return;

    cUnit &groundUnitTakingDamage = unit[id];

    float iDamage = getDamageToInflictToUnit(groundUnitTakingDamage) * factor;
    groundUnitTakingDamage.takeDamage(iDamage);

    // this unit will think what to do now (he got hit ouchy!)
    groundUnitTakingDamage.think_hit(iOwnerUnit, iOwnerStructure);

    // NO HP LEFT, DIE
    if (groundUnitTakingDamage.isDead()) {
        // who is to blame for killing this unit?
        if (iOwnerUnit > -1) {
            cUnit &ownerUnit = unit[iOwnerUnit];
            if (ownerUnit.isValid()) {
                // TODO: update statistics

                if (units[groundUnitTakingDamage.iType].infantry) {
                    ownerUnit.fExperience += 0.25; // 4 kills = 1 star
                } else {
                    ownerUnit.fExperience += 0.45; // ~ 3 kills = 1 star
                }
            }
        }
    }

    if (isDeviatorGas()) {
        // TODO: Stefan: is this needed?!- aren't we playing sound effects in a more generic way?
        play_sound_id_with_distance(SOUND_GAS, distanceBetweenCellAndCenterOfScreen(cell));

        // take over unit
        if (rnd(100) < 40) { // TODO: make property for probability of capturing unit?
            if (iOwnerUnit > -1) {
                cUnit &ownerUnit = unit[iOwnerUnit];
                groundUnitTakingDamage.iPlayer = ownerUnit.iPlayer;
            }

            groundUnitTakingDamage.iAttackStructure = -1;
            groundUnitTakingDamage.iAttackUnit = -1;
            groundUnitTakingDamage.iGroup = -1;
            groundUnitTakingDamage.iAction = ACTION_GUARD;
        }
    }
}

/**
 * Given a unit that is taking damage, decide what kind and how many damage to inflict
 * @param unitTakingDamage
 * @return
 */
float cBullet::getDamageToInflictToUnit(cUnit &unitTakingDamage) const {
    if (unitTakingDamage.isInfantryUnit()) {
        return getDamageToInflictToInfantry();
    }
    return getDamageToInflictToNonInfantry();
}

float cBullet::getDamageToInflictToInfantry() const {
    cPlayerDifficultySettings *difficultySettings = getDifficultySettings();

    float result = difficultySettings->getInflictDamage(bullets[iType].damage_inf);

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
void cBullet::detonateSpiceBloom(int cell, double factor) const {
    if (!bCellValid(cell)) return;
    int cellTypeAtCell = map.getCellType(cell);
    if (cellTypeAtCell != TERRAIN_BLOOM) return;

    // change type of terrain to sand
    mapEditor.createCell(cell, TERRAIN_SAND, 0);
    mapEditor.createField(cell, TERRAIN_SPICE, 50 + (rnd(75)));
    game.TIMER_shake = 20;
}

void cBullet::damageSandworm(int cell, double factor) const {
    if (!bCellValid(cell)) return;
    int id = map.getCellIdWormsLayer(cell);
    if (id < 0) return; // bail

    cUnit &worm = unit[id];
    float damage = getDamageToInflictToNonInfantry() * factor;
    worm.takeDamage(damage);
}

bool cBullet::isAtDestination() const {
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
void cBullet::damageWall(int cell, double factor) const {
    if (!bCellValid(cell)) return;
    int cellTypeAtCell = map.getCellType(cell);
    if (cellTypeAtCell != TERRAIN_WALL) return;

    float iDamage = getDamageToInflictToNonInfantry() * factor;

    map.cellTakeDamage(cell, iDamage);

    if (map.getCellHealth(cell) < 0) {
        // remove wall, turn into smudge:
        mapEditor.createCell(cell, TERRAIN_ROCK, 0);
        mapEditor.smoothAroundCell(cell);
        map.smudge_increase(SMUDGE_WALL, cell);
    }
}

void cBullet::moveBulletTowardsGoal() {
    // step 1 : look to the correct direction
    float angle = fRadians(posX, posY, targetX, targetY);

    // now do some thing to make
    // 1/8 of a cell (2 pixels) per movement
    int movespeed = 2; // this is fixed! (TODO: move this to bullet type data)
    posX += cos(angle) * movespeed;
    posY += sin(angle) * movespeed;
}

/**
 * Is bullet from a turret
 * @return
 */
bool cBullet::isTurretBullet() const {
    return iType == BULLET_TURRET;
}

bool cBullet::isNonFlyingTerrainBullet() const {
    return iType == BULLET_SMALL ||
           iType == BULLET_TRIKE ||
           iType == BULLET_QUAD ||
           iType == BULLET_TANK ||
           iType == BULLET_SIEGE ||
           iType == BULLET_DEVASTATOR ||
           iType == BULLET_TURRET;
}


bool cBullet::isDeviatorGas() const {
    return iType == BULLET_GAS;
}

bool cBullet::isRocket() const {
    return iType == ROCKET_NORMAL ||
           iType == ROCKET_SMALL_ORNI ||
           iType == ROCKET_SMALL ||
           iType == ROCKET_SMALL_FREMEN ||
           iType == ROCKET_RTURRET;
}

float cBullet::getDamageToInflictToNonInfantry() const {
    cPlayerDifficultySettings *pDifficultySettings = getDifficultySettings();
    const s_Bullet &sBullet = gets_Bullet();
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

cPlayerDifficultySettings *cBullet::getDifficultySettings() const {
    const cPlayer * cPlayer = getPlayer();
    cPlayerDifficultySettings *pDifficultySettings = cPlayer->getDifficultySettings();
    return pDifficultySettings;
}

s_Bullet cBullet::gets_Bullet() const {
    return bullets[iType];
}

cPlayer * cBullet::getPlayer() const {
    return &player[iPlayer];
}

/**
 * Damage the structure; if id < 0 then this method does nothing.
 *
 * @param difficultySettings
 * @param idOfStructureAtCell
 */
void cBullet::damageStructure(int idOfStructureAtCell, double factor) {
    if (!bCellValid(idOfStructureAtCell)) return;
    int id = map.getCellIdStructuresLayer(idOfStructureAtCell);
    if (id < 0) return; // bail

    cPlayerDifficultySettings *difficultySettings = getDifficultySettings();

    float iDamage = difficultySettings->getInflictDamage(bullets[iType].damage) * factor;

    cUnit *pUnit = nullptr;
    if (iOwnerUnit > -1) {
        if (&unit[iOwnerUnit] && unit[iOwnerUnit].isValid()) {
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

    pStructure->damage(iDamage);

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
int cBullet::getRandomY() const {
    int half = 16;
    int randomY = -8 + rnd(half);
    return pos_y() + half + randomY;
}

/**
 * Picks a random X position around current position. (max half tile distance)
 * @return
 */
int cBullet::getRandomX() const {
    int half = 16;
    int randomX = -8 + rnd(half);
    return pos_x() + half + randomX;
}

void cBullet::die() {
    bAlive = false;
}
