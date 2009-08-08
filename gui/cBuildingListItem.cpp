#include "..\d2tmh.h"

cBuildingListItem::cBuildingListItem(int theID, s_Structures entry, cBuildingList *list) {
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	type = STRUCTURE;
	progress = 0;
	available = true;
	building = false;
	myList = list;
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry, cBuildingList *list) {
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	type = UNIT;
	progress = 0;
	available = true;
	building = false;
	myList = list;
}

cBuildingListItem::cBuildingListItem(cBuildingList *list) {
	ID = -1;
	cost = 0;
	icon = -1;
	type = UNIT;
	building = false;
	progress = 0;
	available = true;
	myList = list;
}

cBuildingListItem::cBuildingListItem(int theIcon, int theID, eBuildType theType, int theCost, cBuildingList *list) {
	ID = theID;
	cost = theCost;
	icon = theIcon;
	type = theType;
	building = false;
	progress = 0;
	available = true;
	myList = list;
}

bool cBuildingListItem::canPay() {
	int costs;

	// get the ID
	if (type == UNIT) {
		costs = units[ID].cost;
	} else if (type == STRUCTURE) {
		costs = structures[ID].cost;
	}

	if (player[0].credits >= costs) {
		return true;
	}

	return false;
}
