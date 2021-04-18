#include "../../include/d2tmh.h"

// Constructor
cRefinery::cRefinery()
{


 // other variables (class specific)

}

int cRefinery::getType() const {
	return REFINERY;
}

cRefinery::~cRefinery()
{

}


/*  OVERLOADED FUNCTIONS  */

// Specific Construction Yard thinking
void cRefinery::think() {

     if (iUnitID > -1) { // unit has entered structure
         think_unit_occupation();
     }

    // think like base class
    cAbstractStructure::think();

}

void cRefinery::think_unit_occupation() {
    int iMyID = -1;

    for (int i = 0; i < MAX_STRUCTURES; i++) {
        if (structure[i] == this) {
            iMyID = i;
            break;
        }
    }

    // TODO: REMOVE? the iUnitID is only filled when unit already entered?! so this is wrong?
    // when the unit somehow does not go to us anymore, stop animating
    cUnit &cUnit = unit[iUnitID];

    if (!cUnit.isValid()) {
        iUnitID = -1;
        setAnimating(false);
        return;
    }

    // TODO: REMOVE? the iUnitID is only filled when unit already entered?! so this is wrong?
    // unit decided to go to other structure
    if (cUnit.iStructureID != iMyID) {
        iUnitID = -1;
        setAnimating(false);
        return;
    }

    // the unit id is filled in, that means the unit is IN this structure
    // the TIMER_harvest of the unit will be used to dump the harvest in the
    // refinery

    cUnit.TIMER_harvest++;

    cPlayerDifficultySettings *difficultySettings = player[getOwner()].getDifficultySettings();

    if (cUnit.TIMER_harvest < difficultySettings->getDumpSpeed(10)) return;

    cUnit.TIMER_harvest = 0;

    // dump credits
    if (cUnit.iCredits > 0) {
        int iAmount = 5;

        // cap at max
        if (cUnit.iCredits > units[cUnit.iType].credit_capacity)
            cUnit.iCredits = units[cUnit.iType].credit_capacity;


        if ((cUnit.iCredits - iAmount) < 0) {
            iAmount = cUnit.iCredits;
        }

        if (player[cUnit.iPlayer].credits < player[cUnit.iPlayer].max_credits) {
            player[cUnit.iPlayer].credits += iAmount;
            // TODO: update harvested amount (player stats)
        }

        cUnit.iCredits -= iAmount;
        return;
    }

    // the unit is empty (no more credits to dump)
    int iNewCell = getNonOccupiedCellAroundStructure();

    if (iNewCell > -1) {
        cUnit.setCell(iNewCell);

        // let player know...
        if (cUnit.iPlayer == 0)
            play_voice(SOUND_VOICE_02_ATR);

    } else {
        logbook("Could not find space for this unit");
        // TODO: make carryall pick this up
        return;
    }

    // done & restore unit
    cUnit.iCredits = 0;
    cUnit.iStructureID = -1;
    cUnit.restoreFromTempHitPoints();
    cUnit.iGoalCell = cUnit.getCell();
    cUnit.iPathIndex = -1;
    cUnit.updateCellXAndY();

    cUnit.TIMER_movewait = 0;
    cUnit.TIMER_thinkwait = 0;

    map.cellSetIdForLayer(cUnit.getCell(), MAPID_UNITS, iUnitID);

    // perhaps we can find a carryall to help us out
    int iHarvestCell = UNIT_find_harvest_spot(iUnitID);

    if (iHarvestCell > -1) {
        int iCarry = CARRYALL_TRANSFER(iUnitID, iHarvestCell);

        if (iCarry > -1) {
            cUnit.TIMER_movewait = 500;
            cUnit.TIMER_thinkwait = 500;
        }
    }

    iUnitID = -1;
}

void cRefinery::think_harvester_deploy() {
	if (!isAnimating()) return; // do nothing when not animating
    // harvester stuff
	if (iFrame < 0)  {
        iFrame = 1;
	}

    TIMER_flag++;

    if (TIMER_flag > 70) {
        TIMER_flag = 0;

        iFrame++;
		if (iFrame > 4) {
			iFrame = 1;
		}
	}
}

void cRefinery::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
	think_harvester_deploy();
}

void cRefinery::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cRefinery::getSpiceSiloCapacity() {
	float percentage = ((float)getHitPoints() / (float)structures[getType()].hp);
	return 1000 * percentage;
}
