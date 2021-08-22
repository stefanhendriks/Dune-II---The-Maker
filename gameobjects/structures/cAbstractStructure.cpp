/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

#include "../../include/d2tmh.h"
#include "cAbstractStructure.h"


// "default" Constructor
cAbstractStructure::cAbstractStructure() {
    iHitPoints=-1;      // default = no hitpoints
    iCell=-1;

    armor = 1;

    fConcrete=0.0f;

    iPlayer=-1;

    iFrame=-1;

    posX = -1;
    posY = -1;

    bRepair=false;
    bAnimate=false;

    iRallyPoint=-1;

    iBuildFase=-1;

    iUnitIDWithinStructure = -1;
    iUnitIDHeadingForStructure = -1;
    iUnitIDEnteringStructure = -1;

    iWidth=-1;
    iHeight=-1;

    // TIMERS
    TIMER_repair=-1;
    TIMER_flag=-1;
    TIMER_fade=-1;

    TIMER_decay=0;   // damaging stuff
    TIMER_prebuild=0;

    id = -1; // invalid ID at first, it is set in the structureFactory

    dead = false;

    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "(cAbstractStructure)(ID %d) Constructor", this->id);
        logbook(msg);
    }
}

cAbstractStructure::~cAbstractStructure() {
    // destructor
    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "(~cAbstractStructure)(ID %d) Destructor", this->id);
        logbook(msg);
    }
    iHitPoints = -1;
    iCell = -1;
    posX = -1;
    posY = -1;
}

int cAbstractStructure::pos_x() {
    return posX;
}

int cAbstractStructure::pos_y() {
    return posY;
}

// X drawing position
int cAbstractStructure::iDrawX() {
    return mapCamera->getWindowXPosition(pos_x());
}

// Y drawing position
int cAbstractStructure::iDrawY() {
    return mapCamera->getWindowYPosition(pos_y());
}

BITMAP * cAbstractStructure::getBitmap() {
    return this->getPlayer()->getStructureBitmap(getType());
}

BITMAP * cAbstractStructure::getShadowBitmap() {
	s_Structures structureType = getS_StructuresType();
	return structureType.shadow;
}

cPlayer * cAbstractStructure::getPlayer() {
	assert(iPlayer >= HUMAN);
	assert(iPlayer < MAX_PLAYERS);
	return &players[iPlayer];
}

int cAbstractStructure::getMaxHP() {
	int type = getType();
	return structures[type].hp;
}

int cAbstractStructure::getCaptureHP() {
	int type = getType();
    // TODO: Capture hp threshold (property in structure)
    return ((float)structures[type].hp) * 0.30f;
}

int cAbstractStructure::getSight() {
	int type = getType();
	return structures[type].sight;
}

int cAbstractStructure::getRange() {
	int type = getType();
	return structures[type].sight;
}


// this structure dies
void cAbstractStructure::die() {
    // selected structure
    if (game.selected_structure == id) {
        game.selected_structure = -1;
    }

	// remove from array
    structure[id]=NULL;

    // Destroy structure, take stuff in effect for the player
    cPlayer &thePlayer = players[iPlayer];

    thePlayer.decreaseStructureAmount(getType()); // remove from player building indexes

    // UnitID > -1, means the unit inside will die too
    if (iUnitIDWithinStructure > -1) {
        unit[iUnitIDWithinStructure].init(iUnitIDWithinStructure); // die here... softly
    }

    if (iUnitIDEnteringStructure > -1) {
        unit[iUnitIDEnteringStructure].die(true, false);
    }

    if (iUnitIDHeadingForStructure > -1) {
        // reset structure ID
        unit[iUnitIDHeadingForStructure].iStructureID = -1;
        iUnitIDHeadingForStructure = -1;
    }

	int iCll=iCell;
    int iCX= map.getCellX(iCll);
    int iCY= map.getCellY(iCll);

    // create destroy particles
    for (int w = 0; w < iWidth; w++) {
        for (int h = 0; h < iHeight; h++) {
            iCll= map.makeCell(iCX + w, iCY + h);

			map.cellChangeType(iCll, TERRAIN_ROCK);
			mapEditor.smoothAroundCell(iCll);

            int half = 16;
            int posX = map.getAbsoluteXPositionFromCell(iCll);
            int posY = map.getAbsoluteYPositionFromCell(iCll);

            PARTICLE_CREATE(posX + half,
                            posY + half, OBJECT_BOOM01, -1, -1);

            for (int i=0; i < 3; i++)
            {
				map.smudge_increase(SMUDGE_ROCK, iCll);

                // create particle
                PARTICLE_CREATE(posX + half,
                                posY + half, EXPLOSION_STRUCTURE01 + rnd(2), -1, -1);

                // create smoke
                if (rnd(100) < 15) {
                    int randomX = -8 + rnd(16);
                    int randomY = -8 + rnd(16);

                    PARTICLE_CREATE(posX + half + randomX,
                                    posY + half + randomY, OBJECT_SMOKE, -1, -1);
                }

                // create fire
                if (rnd(100) < 15) {
                    int randomX = -8 + rnd(16);
                    int randomY = -8 + rnd(16);

                    PARTICLE_CREATE(posX + half + randomX,
                                    posY + half + randomY, EXPLOSION_FIRE, -1, -1);
                }

            }
        }
    }

    // play sound
    play_sound_id_with_distance(SOUND_CRUMBLE01 + rnd(2),
                                distanceBetweenCellAndCenterOfScreen(iCell));

    // remove from the playground
    map.remove_id(id, MAPID_STRUCTURES);

    // screen shaking
    game.shakeScreen((iWidth * iHeight) * 20);

    // elegible for cleanup
    dead = true;

    s_GameEvent event {
        .eventType = eGameEventType::GAME_EVENT_DESTROYED,
        .entityType = eBuildType::STRUCTURE,
        .entityID = getStructureId(),
        .player = getPlayer(),
        .entitySpecificType = getType()
    };

    game.onNotify(event);
}


void cAbstractStructure::think_prebuild() {
    // not yet done prebuilding
    // Buildfase 1, 3, 5, 7, 9 are all 'prebuilds'
    TIMER_prebuild--;

    if (TIMER_prebuild < 0) {
        iBuildFase++;
        int structureSize = getWidthInPixels() * getHeightInPixels();
        TIMER_prebuild = ((std::min(structureSize/16, 250)) / iBuildFase);
    }
}

std::vector<int> cAbstractStructure::getCellsAroundStructure() {
    int iStartX = map.getCellX(iCell);
    int iStartY = map.getCellY(iCell);

    int iEndX = (iStartX + iWidth) + 1;
    int iEndY = (iStartY + iHeight) + 1;

    iStartX--;
    iStartY--;

    std::vector<int> cells = std::vector<int>();

    for (int x = iStartX; x < iEndX; x++) {
        for (int y = iStartY; y < iEndY; y++) {
            int cell = map.getCellWithMapBorders(x, y);
            if (cell > -1) {
                cells.push_back(cell);
            }
        }
    }

    return cells;
}

/**
 * Returns a list of all cells that this structure occupies. Ie a windtrap of 2x2 returns a list of 4 cells.
 * @return
 */
std::vector<int> cAbstractStructure::getCellsOfStructure() {
    int iStartX = map.getCellX(iCell);
    int iStartY = map.getCellY(iCell);

    int iEndX = (iStartX + iWidth);
    int iEndY = (iStartY + iHeight);

    std::vector<int> cells = std::vector<int>();

    for (int x = iStartX; x < iEndX; x++) {
        for (int y = iStartY; y < iEndY; y++) {
            int cell = map.getCellWithMapBorders(x, y);
            if (cell > -1) {
                cells.push_back(cell);
            }
        }
    }

    return cells;
}

/**
 * Searches around the border of a structure (from top left to bottom right) for a free cell. If found, returns it.
 * If fails, it returns -1
 * @return
 */
int cAbstractStructure::getNonOccupiedCellAroundStructure() {
    const std::vector<int> &cells = getCellsAroundStructure();

    for (auto &cll : cells) {
        if (!map.occupied(cll)) {
            return cll;
        }
    }

    return -1; // fail
}

void cAbstractStructure::think_guard() {
 // filled in by derived class
}

// This method thinks about basic animation
void cAbstractStructure::think_animation() {
    // show (common) prebuild animation
	if (iBuildFase < 10) {
        think_prebuild();
	}
}

void cAbstractStructure::setRepairing(bool value) {
    bRepair = value;
}

void cAbstractStructure::think_flag() {
	if (isAnimating()) return; // do no flag animation when animating

	TIMER_flag++;

    if (TIMER_flag > 70) {
        iFrame++;

		// switch between 0 and 1.
		if (iFrame > 1) {
            iFrame=0;
		}

        TIMER_flag=0;
    }
}

void cAbstractStructure::think_decay() {
    TIMER_decay--;
    if (TIMER_decay > 0) {
        return; // wait until timer is passed
    }

    TIMER_decay = rnd(500) + 500;

    // chance based (so it does not decay all the time)
    if (rnd(100) < getPercentageNotPaved()) {
        if (iHitPoints > (getS_StructuresType().hp / 2)) {
            decay(1);
        }
    }
}

void cAbstractStructure::setWidth(int width) {
	assert(width > 0);
	assert(width < 4);
	iWidth = width;
}

void cAbstractStructure::setHeight(int height) {
	assert(height > 0);
	assert(height < 4);
	iHeight = height;
}

void cAbstractStructure::setRallyPoint(int cell) {
	assert(cell > -2); // -1 is allowed (means disable);
	assert(cell < map.getMaxCells());
	iRallyPoint = cell;
}

void cAbstractStructure::setAnimating(bool value) {
	bAnimate = value;
}

void cAbstractStructure::setFrame(int frame) {
	iFrame = frame;
}

/**
	Damage structure by amount of hp. The amount passed to this method
	must be > 0. When it is < 0, it will be wrapped to > 0 anyway and
	an error is written in the log.

	Decay will never get structure below 1 HP (so won't let the building be destroyed).
**/
void cAbstractStructure::decay(int hp) {
    int damage = hp;
    if (damage < 0) {
        logbook("cAbstractStructure::decay() got negative parameter, wrapped");
        damage *= -1; // - * - = +
    }

    iHitPoints -= damage; // do damage
    if (iHitPoints < 1) iHitPoints = 1;

    s_GameEvent event {
            .eventType = eGameEventType::GAME_EVENT_DECAY,
            .entityType = eBuildType::STRUCTURE,
            .entityID = getStructureId(),
            .player = getPlayer(),
            .entitySpecificType = getType()
    };

    game.onNotify(event);
}


/**
	Damage structure by amount of hp. The amount passed to this method
	must be > 0. When it is < 0, it will be wrapped to > 0 anyway and
	an error is written in the log.

	When the hitpoints drop below 1, the structure will die / cause destroyed animation and set 'dead' flag
    so that the structure can be cleaned up from memory.
**/
void cAbstractStructure::damage(int hp) {
	int damage = hp;
	if (damage < 0) {
		logbook("cAbstractStructure::damage() got negative parameter, wrapped");
		damage *= -1; // - * - = +
	}

	iHitPoints -= damage; // do damage
    char msg[255];
    sprintf(msg, "cAbstractStructure::damage() - Structure [%d] received [%d] damage, HP is now [%d]", id, damage, iHitPoints);
    logbook(msg);

    if (iHitPoints < 1) {
        // TODO: update statistics? (structure lost)
        die();
    } else {
        int iChance = getSmokeChance();

        // Structure on fire?
        if (rnd(100) < iChance) {
            PARTICLE_CREATE(getRandomPosX(), getRandomPosY(), OBJECT_SMOKE, -1, -1);
        }

        s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_DAMAGED,
                .entityType = eBuildType::STRUCTURE,
                .entityID = getStructureId(),
                .player = getPlayer(),
                .entitySpecificType = getType()
        };

        game.onNotify(event);
    }
}

/**
 * Set HP of structure, caps it at its maximum.
 */
void cAbstractStructure::setHitPoints(int hp) {
	iHitPoints = hp;
	int maxHp = structures[getType()].hp;

	if (iHitPoints > maxHp) {
		char msg[256];
		sprintf(msg, "setHitpoints(%d) while max is %d; capped at max.", hp, maxHp);
		logbook(msg);

		// will fail (uncomment to let it be capped)
		assert(iHitPoints <= maxHp); // may never be more than the maximum of that structure

		iHitPoints = maxHp;
	}

}

void cAbstractStructure::setCell(int cell) {
	iCell = cell;
    posX = map.getAbsoluteXPositionFromCell(iCell);
    posY = map.getAbsoluteYPositionFromCell(iCell);
}

void cAbstractStructure::setOwner(int player) {
	iPlayer = player;
}

/**
	Think actions like any other structure would have.
**/
void cAbstractStructure::think() {
    think_decay();
    think_repair();
}

void cAbstractStructure::think_repair() {
    // REPAIRING
    if (bRepair) {
		if (players[iPlayer].hasEnoughCreditsFor(1.0f)) {
			TIMER_repair++;

			if (TIMER_repair > 7)
			{
				TIMER_repair=0;
				iHitPoints += structures[getType()].fixhp;
				players[iPlayer].substractCredits(1);
			}

			// done repairing
			if (iHitPoints >= getMaxHP()) {
				iHitPoints = getMaxHP();
				bRepair=false;
			}
		}
		assert(iHitPoints <= structures[getType()].hp);
	}
}

s_Structures cAbstractStructure::getS_StructuresType() const {
	return structures[getType()];
}

int cAbstractStructure::getPercentageNotPaved() {
	return fConcrete * 100;
}

bool cAbstractStructure::isPrimary() {
    return getPlayer()->getPrimaryStructureForStructureType(getType()) == id;
}

int cAbstractStructure::getPowerUsage() {
	return getS_StructuresType().power_drain;
}

bool cAbstractStructure::isValid() {
    if (iPlayer < 0)
        return false;

    if (dead) // flagged for deletion, so no longer 'valid'
        return false;

    if (!map.isValidCell(iCell))
        return false;

    return true;
}

float cAbstractStructure::getHealthNormalized() {
    const s_Structures &structureType = getS_StructuresType();
    float flMAX  = structureType.hp;
    return (iHitPoints / flMAX);
}

int cAbstractStructure::getWidthInPixels() {
    return getS_StructuresType().bmp_width;
}

int cAbstractStructure::getHeightInPixels() {
    return getS_StructuresType().bmp_height;
}

bool cAbstractStructure::isDamaged() {
    return getHitPoints() < getMaxHP();
}

/**
 * Probability between 0-100 when to create smoke particles.
 * Based on health of structure.
 * @return
 */
int cAbstractStructure::getSmokeChance() {
    if (getHitPoints() < (getMaxHP() / 2)) {
        return 15;
    }

    return 5;
}

bool cAbstractStructure::belongsTo(int playerId) const {
    return iPlayer == playerId;
}

bool cAbstractStructure::belongsTo(const cPlayer * other) const {
    if (other == nullptr) return false;
    return belongsTo(other->getId());
}

/**
 * Makes sure this structure switches owner, and takes care of the player internal bookkeeping.
 * @param pPlayer
 */
void cAbstractStructure::getsCapturedBy(cPlayer *pPlayer) {
    getPlayer()->decreaseStructureAmount(getType());
    iPlayer = pPlayer->getId();
    pPlayer->increaseStructureAmount(getType());
}

int cAbstractStructure::getRandomPosX() {
    return pos_x() + rnd(getWidthInPixels()); // posX = most left, so just increase
}

int cAbstractStructure::getRandomPosY() {
    return pos_y() + rnd(getHeightInPixels()); // posY = top coordinate, so just increase
}

void cAbstractStructure::startRepairing() {
    bRepair = true;
}

void cAbstractStructure::setUnitIdWithin(int unitId) {
    if (unitId > -1) {
        if (unitId != iUnitIDWithinStructure) {
            assert(iUnitIDWithinStructure < 0 &&
                   "cAbstractStructure::setUnitIdWithin - Cannot set iUnitIDWithinStructure, because it has been set already!");
        }
    }
    this->iUnitIDWithinStructure = unitId;
}

void cAbstractStructure::setUnitIdHeadingTowards(int unitId) {
    if (unitId > -1) {
        if (unitId != iUnitIDHeadingForStructure) {
            assert(iUnitIDHeadingForStructure < 0 &&
                   "cAbstractStructure::setUnitIdHeadingTowards - Cannot set iUnitIDHeadingForStructure, because it has been set already!");
        }
    }
    this->iUnitIDHeadingForStructure = unitId;
}

void cAbstractStructure::setUnitIdEntering(int unitId) {
    if (unitId > -1) {
        if (unitId != iUnitIDEnteringStructure) {
            assert(iUnitIDEnteringStructure < 0 &&
                   "cAbstractStructure::setUnitIdHeadingTowards - Cannot set iUnitIDEnteringStructure, because it has been set already!");
        }
    }
    this->iUnitIDEnteringStructure = unitId;
}

void cAbstractStructure::enterStructure(int unitId) {
    setUnitIdHeadingTowards(-1);
    setUnitIdWithin(unitId);
    setUnitIdEntering(-1);
    setAnimating(false);
    setFrame(0);

    cUnit &pUnit = unit[unitId];

    pUnit.hideUnit();
    pUnit.setCell(getCell());
    pUnit.updateCellXAndY();

    map.remove_id(unitId, MAPID_UNITS);
}

void cAbstractStructure::unitLeavesStructure() {
    cUnit &unitToLeave = unit[iUnitIDWithinStructure];
    int iNewCell = getNonOccupiedCellAroundStructure();

    if (iNewCell > -1) {
        unitToLeave.setCell(iNewCell);
    } else {
        logbook("Could not find space for this unit");
        // TODO: Pick up by carry-all!?
    }

    // done & restore unit
    unitToLeave.iCredits = 0;
    unitToLeave.iStructureID = -1;

    unitToLeave.TIMER_harvest = 0;
    unitToLeave.restoreFromTempHitPoints();

    unitToLeave.iGoalCell = unitToLeave.getCell();
    unitToLeave.iPathIndex = -1;

    unitToLeave.TIMER_movewait = 0;
    unitToLeave.TIMER_thinkwait = 0;

    if (getRallyPoint() > -1) {
        unitToLeave.move_to(getRallyPoint(), -1, -1);
    }

    map.cellSetIdForLayer(unitToLeave.getCell(), MAPID_UNITS, iUnitIDWithinStructure);

    setUnitIdWithin(-1);
    setUnitIdHeadingTowards(-1);
    setAnimating(false);
}

void cAbstractStructure::startEnteringStructure(int unitId) {
    setUnitIdWithin(-1);
    setUnitIdEntering(unitId);
    setUnitIdHeadingTowards(-1);
}

void cAbstractStructure::unitHeadsTowardsStructure(int unitId) {
    setUnitIdHeadingTowards(unitId);
    setAnimating(true);
}

int cAbstractStructure::getRandomStructureCell() {
    return getCell() + rnd(getWidth()) + (rnd(getHeight()) * map.getWidth());
}

/**
 * Returns true when a different unit id is entering the structure or it has been occupied
 * @param unitId
 * @return
 */
bool cAbstractStructure::isInProcessOfBeingEnteredOrOccupiedByUnit(int unitId) {
    return hasUnitWithin() || (hasUnitEntering() && getUnitIdEntering() != unitId);
}
