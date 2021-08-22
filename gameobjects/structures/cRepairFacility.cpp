#include "../../include/d2tmh.h"


// Constructor
cRepairFacility::cRepairFacility() {

    // other variables (class specific)
    TIMER_repairunit = 0;
}

int cRepairFacility::getType() const {
    return REPAIR;
}

cRepairFacility::~cRepairFacility() {

}


void cRepairFacility::think() {

    // Repair unit here (if any)
    if (hasUnitWithin()) {
        think_repairUnit();
    }

    // think like base class
    cAbstractStructure::think();

}

void cRepairFacility::think_repairUnit() {// must repair...
    int iUnitID = getUnitIdWithin();
    cUnit &unitToRepair = unit[iUnitID];
    int maxHpForUnitType = unitInfo[unitToRepair.iType].hp;

    if (unitToRepair.iTempHitPoints < maxHpForUnitType) {
        TIMER_repairunit++;

        // TODO: move to structure info? (or unit info?)
        int REPAIR_SPEED = 15;

        if (TIMER_repairunit < REPAIR_SPEED) return;

        TIMER_repairunit = 0;

        cPlayer *pPlayer = getPlayer();
        int REPAIR_COST_PER_TICK = 1;

        if (pPlayer->hasEnoughCreditsFor(REPAIR_COST_PER_TICK)) {
            // TODO: Move repair rate per tick (how much hp per tick will be increased?) into unit info?
            int REPAIR_RATE_HP_PER_TICK = 3; // ie, unit with 300 HP, costs 100 to repair

            unitToRepair.iTempHitPoints += REPAIR_RATE_HP_PER_TICK;
            
            // TODO: Move to unit info
            // Cost per tick to repair unit for REPAIR_RATE_HP_PER_TICK amount
            pPlayer->substractCredits(REPAIR_COST_PER_TICK);
        }
    } else if (unitToRepair.iTempHitPoints >= maxHpForUnitType) {
        unitToRepair.setMaxHitPoints();

        // dump unit, get rid of it
        unitLeavesStructure();
    }
}

void cRepairFacility::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag();
}

void cRepairFacility::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
