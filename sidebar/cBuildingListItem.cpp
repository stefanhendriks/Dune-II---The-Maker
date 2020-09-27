#include "../include/d2tmh.h"

cBuildingListItem::cBuildingListItem(int theID, s_Structures entry, cBuildingList *list, int subList) {
	assert(theID >= 0);
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	totalBuildTime = entry.build_time;
	type = STRUCTURE;
	progress = 0;
	available = true;
	building = false;
	myList = list;
	timesToBuild = 0;
	timesOrdered  = 0;
	this->subList = subList;
	if (entry.cost > 0) {
		creditsPerProgressTime = (float)entry.cost / (float)entry.build_time;
	}
	placeIt = false;
}

cBuildingListItem::cBuildingListItem(int theID, s_Structures entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry, cBuildingList *list, int subList) {
	assert(theID >= 0);
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	totalBuildTime = entry.build_time;
	type = UNIT;
	progress = 0;
	available = true;
	building = false;
	myList = list;
	timesToBuild = 0;
	timesOrdered  = 0;
    this->subList = subList;
	if (entry.cost > 0 && entry.build_time > 0) {
		creditsPerProgressTime = (float)entry.cost / (float)entry.build_time;
	}
	placeIt = false;
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

bool cBuildingListItem::canPay() {
	int costs = 0;

	// get the ID
	if (type == UNIT) {
		costs = units[ID].cost;
	} else if (type == STRUCTURE) {
		costs = structures[ID].cost;
	} else {
		costs = 0;
	}

	if (player[0].credits >= costs) {
		return true;
	}

	return false;
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
    if (!myList) return 0;
    if (myList->getType() == LIST_CONSTYARD) {
        return structures[getBuildId()].build_time;
    }
    // assumes other things (ie super weapons and such) are also under 'units' array.
    return units[getBuildId()].build_time;
}

bool cBuildingListItem::isDoneBuilding() {
    return getProgress() >= getBuildTime();
}
