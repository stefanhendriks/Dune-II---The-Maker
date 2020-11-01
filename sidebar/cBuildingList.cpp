#include "../include/d2tmh.h"

cBuildingList::cBuildingList(int theId) {
	TIMER_progress = 0;
	lastClickedId = 0;
	buttonIconIdPressed = 0;	// the button to draw at the left of the list
	buttonDrawX = 0;
	buttonDrawY = 0;
	available = false;		 // is this list available?
	memset(items, 0, sizeof(items));
	typeOfList = theId;
	upgradeLevel = 0;
	upgrading = false;
	maxItems = 0;
	acceptsOrders = true;
}

cBuildingList::~cBuildingList() {
	TIMER_progress = 0;
	lastClickedId = 0;
	buttonIconIdPressed = 0;	// the button to draw at the left of the list
	buttonDrawX = 0;
	buttonDrawY = 0;
	available = false;		 // is this list available?
	removeAllItems();
	memset(items, 0, sizeof(items));
	upgradeLevel = 0;
	upgrading = false;
	maxItems = 0;
	acceptsOrders = false;
}

cBuildingListItem * cBuildingList::getItem(int position) {
	assert(position > -1);
	assert(position < MAX_ICONS);
	return items[position];
}

/**
 * Return a flee slot within the array to be used.
 *
 * @return
 */
int cBuildingList::getFreeSlot() {
	for (int i = 0; i < MAX_ICONS; i++) {
		if (items[i] == NULL) {
			return i; // return free slot
		}
	}

	return -1; // no free slot
}

void cBuildingList::removeAllItems() {
	for (int i =0 ; i < MAX_ICONS; i++) {
		removeItemFromList(i);
	}
}

bool cBuildingList::isItemInList(cBuildingListItem * item) {
	assert(item);
	return hasItemType(item->getBuildId());
}

bool cBuildingList::hasItemType(int itemTypeId) {
	assert(itemTypeId >= 0);
	for (int i =0; i < MAX_ICONS; i++) {
		cBuildingListItem * itemInList = getItem(i);

		// item already in list (same build id)
		if (itemInList) {
			if (itemTypeId == itemInList->getBuildId()) {
				return true;
			}
		}
	}
	return false;
}



void cBuildingList::addStructureToList(int structureType, int subList) {
    addItemToList(new cBuildingListItem(structureType, structures[structureType], subList));
}

void cBuildingList::addUnitToList(int unitType, int subList) {
    addItemToList(new cBuildingListItem(unitType, units[unitType], subList));
}

bool cBuildingList::addItemToList(cBuildingListItem * item) {
	assert(item);

	if (isItemInList(item)) {
		logbook("Will not add, item is already in list.");
		// item is already in list, do not add
		return false;
	}

	int slot = getFreeSlot();
	if (slot < 0 ) {
		logbook("Failed to add icon to cBuildingList, no free slot left in list");
        return false;
	}

	// add
	items[slot] = item;
	item->setSlotId(slot);
	item->setList(this);
	maxItems = slot;
//	char msg[355];
//	sprintf(msg, "Icon added with id [%d] added to cBuilding list, put in slot[%d], set maxItems to [%d]", item->getBuildId(), slot, maxItems);
//	logbook(msg);
    return true;
}

/**
 * Remove item from list. Delete item object and set NULL in array.
 *
 * @param position
 */
void cBuildingList::removeItemFromList(int position) {
	assert(position > -1);
	assert(position < MAX_ICONS);
	cBuildingListItem * item = getItem(position);
	if (item == NULL) {
		// item can be null, in that case do nothing.
	} else {
		delete item;
		items[position] = NULL;
		maxItems--;
	}

}

/**
 * Return true when given X,Y is within the button area. Use this method to determine
 * if mouse is over the button.
 *
 * @param x
 * @param y
 * @return
 */
bool cBuildingList::isOverButton(int x, int y) {
    return cMouse::isOverRectangle(getButtonDrawX(), getButtonDrawY(), 27, 27);
}

/**
 * Returns an array of id's of subLists of items being built. The array is 5 items big (ie, we assume we don't have
 * more than 5 sublists in a building list). When a value > -1 then it means something is being built for that subList.
 * Ie, the index in the array corresponds with the subList id. Meaning, result[1] = 5 means sublist 1 is building something
 * of ID 5. (depending on the TYPE, it is either a structure or a unit).
 *
 * @return
 */
std::array<int, 5> cBuildingList::isBuildingItem() {
    std::array<int, 5> subListIds;
    for (int i = 0; i < 5; i++) {
        subListIds[i] = -1;
    }
	for (int i = 0 ; i < MAX_ITEMS; i++) {
		cBuildingListItem *item = getItem(i);

		// valid pointer
		if (item) {
			// get isBuilding
			if (item->isBuilding() || item->shouldPlaceIt()) {
                subListIds[item->getSubList()] = item->getBuildId();
			}
		}
	}
	return subListIds;
}


cBuildingListItem * cBuildingList::getItemToPlace() {
	for (int i = 0 ; i < MAX_ITEMS; i++) {
		cBuildingListItem *item = getItem(i);
		// valid pointer
		if (item) {
			// get isBuilding
			if (item->shouldPlaceIt()) {
				return item;
			}
		}
	}
	return NULL;
}

void cBuildingList::increaseUpgradeLevel() {
    this->upgradeLevel++;
}
