#include "..\d2tmh.h"

cBuildingListItem::cBuildingListItem(int theID, s_Structures entry) {
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	type = STRUCTURE;
	progress = 0;
	available = true;
	building = false;
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitP entry) {
	ID = theID;
	cost = entry.cost;
	icon = entry.icon;
	type = UNIT;
	progress = 0;
	available = true;
	building = false;
}

cBuildingListItem::cBuildingListItem() {
	ID = -1;
	cost = 0;
	icon = -1;
	type = UNIT;
	building = false;
	progress = 0;
	available = true;
}

cBuildingListItem::cBuildingListItem(int theIcon, int theID, eBuildType theType, int theCost) {
	ID = theID;
	cost = theCost;
	icon = theIcon;
	type = theType;
	building = false;
	progress = 0;
	available = true;
}
