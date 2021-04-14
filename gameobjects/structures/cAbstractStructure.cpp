/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "../../include/d2tmh.h"


// "default" Constructor
cAbstractStructure::cAbstractStructure() {
    iHitPoints=-1;      // default = no hitpoints
    iCell=-1;

    armor = 1;

    fConcrete=0.0f;

    iPlayer=-1;

    iFrame=-1;

    bRepair=false;
    bAnimate=false;

    iRallyPoint=-1;

    iBuildFase=-1;

    iUnitID=-1;

    iWidth=-1;
    iHeight=-1;

    // TIMERS
    TIMER_repair=-1;
    TIMER_flag=-1;
    TIMER_fade=-1;

    TIMER_damage=0;   // damaging stuff
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
}

int cAbstractStructure::pos_x() {
    return map.getAbsoluteXPositionFromCell(iCell);
}

int cAbstractStructure::pos_y() {
    return map.getAbsoluteYPositionFromCell(iCell);
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
	return &player[iPlayer];
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
    cPlayer &thePlayer = player[iPlayer];

    thePlayer.decreaseStructureAmount(getType()); // remove from player building indexes

    // fix up power usage
    thePlayer.use_power -= structures[getType()].power_drain;

    // less power
    thePlayer.has_power -= structures[getType()].power_give;

	if (getType() == SILO) {
        thePlayer.max_credits -= 1000;
	}

	if (getType() == REFINERY) {
        thePlayer.max_credits -= 1500;
	}

    // UnitID > -1, means the unit inside will die too
    if (iUnitID > -1) {
        unit[iUnitID].init(iUnitID); // die here... softly
    }

	int iCll=iCell;
	int iCX=iCellGiveX(iCll);
	int iCY=iCellGiveY(iCll);

    // create destroy particles
    for (int w = 0; w < iWidth; w++) {
        for (int h = 0; h < iHeight; h++) {
			iCll=iCellMake(iCX+w, iCY+h);

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
    game.TIMER_shake = (iWidth * iHeight) * 20;

    // elegible for cleanup
    dead = true;
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

/**
 * Searches around the border of a structure (from top left to bottom right) for a free cell. If found, returns it.
 * If fails, it returns -1
 * @return
 */
int cAbstractStructure::getNonOccupiedCellAroundStructure() {
    int iStartX = iCellGiveX(iCell);
    int iStartY = iCellGiveY(iCell);

    int iEndX = (iStartX + iWidth) + 1;
    int iEndY = (iStartY + iHeight) + 1;

    iStartX--;
    iStartY--;

    int iCx = 0;
    int iCy = 0;

    for (int x = iStartX; x < iEndX; x++) {
        for (int y = iStartY; y < iEndY; y++) {
            iCx = x;
            iCy = y;

            // iCx and iCy are passed by reference
            FIX_BORDER_POS(iCx, iCy);

            // so they are for sure not at the outer edges on the map now...
            int cll = iCellMakeWhichCanReturnMinusOneWithinMapBorders(iCx, iCy);

            if (cll > -1 && !map.occupied(cll)) {
                return cll;
            }
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

void cAbstractStructure::think_damage() {
    TIMER_damage--;
    if (TIMER_damage < 0) {
        TIMER_damage = rnd(500)+250;

        // chance based (so it does not decay all the time)
        if (rnd(100) < getPercentageNotPaved()) {
			if (iHitPoints > (structures[getType()].hp / 2)) {
				iHitPoints -= 1;
			}
        }

        // AI reacting to this damage
        // TODO: remove here
		if (iPlayer != HUMAN && player[iPlayer].credits > 50) {
			if (iHitPoints < ((structures[getType()].hp / 4)*3)) { // lower than 75%
                bRepair=true;
			}
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
	assert(cell < MAX_CELLS);
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
        die();
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
}

void cAbstractStructure::setOwner(int player) {
	iPlayer = player;
}

/**
	Think actions like any other structure would have.
**/
void cAbstractStructure::think() {
	// AI
    if (iPlayer > 0) {
    	aiplayer[iPlayer].think_repair_structure(this);
    }

    // Other
    think_damage();
    think_repair();
}

void cAbstractStructure::think_repair()
{
    // REPAIRING
    if (bRepair) {
		if (player[iPlayer].credits > 1.0f) {
			TIMER_repair++;

			if (TIMER_repair > 7)
			{
				TIMER_repair=0;
				iHitPoints += structures[getType()].fixhp;
				player[iPlayer].credits--;
				if (player[iPlayer].credits < 0.0f) {
					player[iPlayer].credits = 0.0f;
				}
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

s_Structures cAbstractStructure::
getS_StructuresType() const {
	return structures[getType()];
}

int cAbstractStructure::getPercentageNotPaved() {
	return fConcrete * 100;
}

bool cAbstractStructure::isPrimary() {
	cPlayer * thePlayer = getPlayer();
	return thePlayer->getPrimaryStructureForStructureType(getType()) == id;
}

int cAbstractStructure::getPowerUsage() {
	return getS_StructuresType().power_drain;
}

bool cAbstractStructure::isValid() {
    if (iPlayer < 0)
        return false;

    if (dead) // flagged for deletion, so no longer 'valid'
        return false;

    if (iCell < 0 || iCell >= MAX_CELLS)
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
 * Based on health of structure. (< 50% has 3x higher probability to spawn smoke)
 * @return
 */
int cAbstractStructure::getSmokeChance() {
    if (getHitPoints() < (getMaxHP() / 2)) {
        return 45;
    }

    return 15;
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
