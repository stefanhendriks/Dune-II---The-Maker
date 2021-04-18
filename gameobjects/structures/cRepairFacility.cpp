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
    if (iUnitID > -1) {
        think_repairUnit();
    }

    // think like base class
    cAbstractStructure::think();

}

void cRepairFacility::think_repairUnit() {// must repair...
    cUnit &unitToRepair = unit[iUnitID];
    int maxHpForUnitType = units[unitToRepair.iType].hp;

    if (unitToRepair.iTempHitPoints < maxHpForUnitType) {
        TIMER_repairunit++;

        if (TIMER_repairunit < 15) return;

        TIMER_repairunit = 0;

        cPlayer *pPlayer = getPlayer();
        if (pPlayer->hasEnoughCreditsFor(2)) {
            unitToRepair.iTempHitPoints += 3; // ie, unit with 300 HP, costs 100 to repair
            pPlayer->substractCredits(1);

            if (unitToRepair.iTempHitPoints >= maxHpForUnitType) {
                unitToRepair.setMaxHitPoints();

                // dump unit, get rid of it
                int iNewCell = getNonOccupiedCellAroundStructure();

                if (iNewCell > -1) {
                    unitToRepair.setCell(iNewCell);
                } else {
                    logbook("Could not find space for this unit");
                    // TODO: Pick up by carry-all!?
                }

                // done & restore unit
                unitToRepair.iStructureID = -1;

                unitToRepair.iTempHitPoints = -1;

                unitToRepair.iGoalCell = unitToRepair.getCell();
                unitToRepair.iPathIndex = -1;

                unitToRepair.TIMER_movewait = 0;
                unitToRepair.TIMER_thinkwait = 0;

                if (getRallyPoint() > -1)
                    unitToRepair.move_to(getRallyPoint(), -1, -1);

                map.cellSetIdForLayer(unitToRepair.getCell(), MAPID_UNITS, iUnitID);

                iUnitID = -1;
            }
        }
    }
}

void cRepairFacility::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag();
}

void cRepairFacility::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
