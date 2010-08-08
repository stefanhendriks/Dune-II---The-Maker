#include "..\d2tmh.h"

cBuildingListItem::cBuildingListItem(int theID, s_Structures entry, cBuildingList *list) {
	assert(theID >= 0);
	assert(list);
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	type = STRUCTURE;
	progress = 0;
	available = true;
	building = false;
	myList = list;
	timesToBuild = 0;
	timesOrdered  = 0;
	if (entry.cost > 0) {
		creditsPerProgressTime = (float)entry.cost / (float)entry.build_time;
	}
	placeIt = false;
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry, cBuildingList *list) {
	assert(theID >= 0);
	assert(list);
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	type = UNIT;
	progress = 0;
	available = true;
	building = false;
	myList = list;
	timesToBuild = 0;
	timesOrdered  = 0;
	if (entry.cost > 0 && entry.build_time > 0) {
		creditsPerProgressTime = (float)entry.cost / (float)entry.build_time;
	}
	placeIt = false;
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
