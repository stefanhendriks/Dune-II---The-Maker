#include "../../include/d2tmh.h"

// Constructor
cRepairFacility::cRepairFacility() {

	// other variables (class specific)
	TIMER_repairunit = 0;
}

int cRepairFacility::getType() {
	return REPAIR;
}

cRepairFacility::~cRepairFacility() {

}

void cRepairFacility::think() {

	// Repair unit here (if any)
	if (iUnitID > -1) {
		// must repair...
		if (unit[iUnitID].iTempHitPoints < units[unit[iUnitID].iType].hp) {
			TIMER_repairunit++;

			if (TIMER_repairunit > 15) {
				TIMER_repairunit = 0;

				if (player[getOwner()].credits > 2) {
					unit[iUnitID].iTempHitPoints += 3;
					player[getOwner()].credits--;

					if (unit[iUnitID].iTempHitPoints >= units[unit[iUnitID].iType].hp) {
						unit[iUnitID].iHitPoints = units[unit[iUnitID].iType].hp;

						// dump unit, get rid of it
						int iNewCell = iFreeAround();

						if (iNewCell > -1) {
							unit[iUnitID].iCell = iNewCell;
						} else
							logbook("Could not find space for this unit");

						// done & restore unit
						unit[iUnitID].iStructureID = -1;

						unit[iUnitID].iTempHitPoints = -1;

						unit[iUnitID].iGoalCell = unit[iUnitID].iCell;
						unit[iUnitID].iPathIndex = -1;

						unit[iUnitID].TIMER_movewait = 0;
						unit[iUnitID].TIMER_thinkwait = 0;

						if (getRallyPoint() > -1)
							unit[iUnitID].move_to(getRallyPoint(), -1, -1);

						if (DEBUGGING)
							assert(iUnitID > -1);

						map->cell[unit[iUnitID].iCell].gameObjectId[MAPID_UNITS] = iUnitID;

						iUnitID = -1;

					}
				}
			}
		}
	}

	// think like base class
	cAbstractStructure::think();

}

void cRepairFacility::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cRepairFacility::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
