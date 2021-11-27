#include "../../include/d2tmh.h"

// Constructor
cRefinery::cRefinery() {

    // other variables (class specific)
    shouldAnimateWhenUnitHeadsTowardsStructure = true;
}

int cRefinery::getType() const {
    return REFINERY;
}

cRefinery::~cRefinery() {

}


/*  OVERLOADED FUNCTIONS  */

// Specific Construction Yard thinking
void cRefinery::think() {

    if (hasUnitWithin()) { // unit has entered structure
        think_unit_occupation();
    }

    // think like base class
    cAbstractStructure::think();
}

void cRefinery::think_unit_occupation() {
    int iUnitID = getUnitIdWithin();
    cUnit &cUnit = unit[iUnitID];

    // the unit id is filled in, that means the unit is IN this structure
    // the TIMER_harvest of the unit will be used to dump the harvest in the
    // refinery
    cUnit.TIMER_harvest++;

    cPlayer *pPlayer = getPlayer();
    cPlayerDifficultySettings *difficultySettings = pPlayer->getDifficultySettings();

    if (cUnit.TIMER_harvest < difficultySettings->getDumpSpeed(10)) return;

    cUnit.TIMER_harvest = 0;

    // dump credits
    if (cUnit.iCredits > 0) {
        int iAmount = 5;

        // cap at max
        s_UnitInfo &unitType = sUnitInfo[cUnit.iType];

        if (cUnit.iCredits > unitType.credit_capacity) {
            cUnit.iCredits = unitType.credit_capacity;
            // this fixes the upper bound (so no unit can cheat !?)
        }

        // can substract credits? if not, choose remaining
        if ((cUnit.iCredits - iAmount) < 0) {
            iAmount = cUnit.iCredits;
        }

        pPlayer->dumpCredits(iAmount);
        cUnit.iCredits -= iAmount;
        return;
    }

    // Dumping credits is finished
    unitLeavesStructure();

    setAnimating(false);

    // let player know...
    if (pPlayer->isHuman()) {
        play_voice(SOUND_VOICE_02_ATR);
    }

    // perhaps we can find a carryall to help us out
    int iHarvestCell = UNIT_find_harvest_spot(iUnitID);

    if (iHarvestCell > -1) {
        int iCarry = CARRYALL_TRANSFER(iUnitID, iHarvestCell);

        if (iCarry > -1) {
            cUnit.TIMER_movewait = 500;
            cUnit.TIMER_thinkwait = 500;
        }
    }
}

void cRefinery::think_harvester_deploy() {
    if (hasUnitWithin()) {
        iFrame = 7;
        return;
    } else {
        if (!isAnimating()) {
            iFrame = 0;
        }
    }

    if (!isAnimating()) return; // do nothing when not animating

    // harvester stuff
    if (iFrame < 0) {
        iFrame = 0;
    }

    TIMER_flag++;

    if (TIMER_flag > 30) {
        TIMER_flag = 0;

        iFrame++;
        if (iFrame > 6) {
            iFrame = 0;
        }
    }
}

void cRefinery::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
    think_harvester_deploy();
}

void cRefinery::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cRefinery::getSpiceSiloCapacity() {
    float percentage = ((float) getHitPoints() / (float) sStructureInfo[getType()].hp);
    return 1000 * percentage;
}
