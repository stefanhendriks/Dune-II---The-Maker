#include "../include/d2tmh.h"

/**
 *
 * @param type
 * @param buildId (id of the TYPE, ie structureType ID or unitType ID or upgradeType ID)
 * @param cost
 * @param icon
 * @param totalBuildTime
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(eBuildType type, int buildId, int cost, int icon, int totalBuildTime, cBuildingList *list, int subList, bool queuable) {
    assert(buildId >= 0);
    this->buildId = buildId;
    this->cost = cost;
    this->icon = icon;
    this->totalBuildTime = totalBuildTime;
    this->type = type;
    this->queuable = queuable;
    progress = 0;
    state = AVAILABLE;
    building = false;
    myList = list; // this can be nullptr! (it will be set from the outside by cBuildingList convenience methods)
    timesToBuild = 0;
    timesOrdered  = 0;
    slotId = -1; // is set later
    this->subList = subList;
    if (cost > 0 && totalBuildTime > 0) {
        creditsPerProgressTime = (float)this->cost / (float)this->totalBuildTime;
    }
    placeIt = false;
}

cBuildingListItem::~cBuildingListItem() {
    myList = NULL;
}

/**
 * Constructor for Structures
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_Structures entry, cBuildingList *list, int subList) :
                    cBuildingListItem(STRUCTURE, theID, entry.cost, entry.icon, entry.build_time, list, subList, entry.queuable) {
}

/**
 * Constructor for Upgrades
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_Upgrade entry, cBuildingList *list, int subList) :
                    cBuildingListItem(UPGRADE, theID, entry.cost, entry.icon, entry.buildTime, list, subList, false) {
}

cBuildingListItem::cBuildingListItem(int theID, s_Structures entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

/**
 * Constructor for units
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry, cBuildingList *list, int subList) :
                    cBuildingListItem(UNIT, theID, entry.cost, entry.icon, entry.build_time, list, subList, entry.queuable) {
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

cBuildingListItem::cBuildingListItem(int theID, s_Upgrade entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

int cBuildingListItem::getCosts() {
    int costs = 0;

    if (type == UNIT) {
        costs = units[buildId].cost;
    } else if (type == STRUCTURE) {
        costs = structures[buildId].cost;
    } else if (type == UPGRADE) {
        costs = upgrades[buildId].cost;
    }
    return costs;
}

/**
 * Return the amount of money the player gets back
 * @return
 */
float cBuildingListItem::getRefundAmount() {
	float fProgress = progress;
	if (fProgress < 1.0F) {
		return 0.0F;
	}
	return (fProgress * creditsPerProgressTime);
}

void cBuildingListItem::decreaseTimesToBuild() {
	timesToBuild--;
}

void cBuildingListItem::increaseProgress(int byAmount) {
    setProgress(getProgress() + byAmount);
}

int cBuildingListItem::getBuildTime() {
//    if (DEBUGGING) return 1;
    if (type == STRUCTURE) {
        return structures[buildId].build_time;
    }
    if (type == UPGRADE) {
        return upgrades[buildId].buildTime;
    }
    // assumes other things (ie super weapons and such) are also under 'units' array.
    return units[buildId].build_time;
}

bool cBuildingListItem::isDoneBuilding() {
    return getProgress() >= getBuildTime();
}
