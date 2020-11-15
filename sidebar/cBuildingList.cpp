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
	maxItems = 0;
	acceptsOrders = true; // at default true, will be set to FALSE/TRUE by starport logic for starport list only
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
	maxItems = 0;
	acceptsOrders = false;
}

cBuildingListItem * cBuildingList::getItem(int i) {
    if (i < 0) return nullptr;
    if (i >= MAX_ICONS) return nullptr;
	return items[i];
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
	if (item == nullptr) return false;
	return getItemByBuildId(item->getBuildId()) != nullptr;
}

cBuildingListItem * cBuildingList::getItemByBuildId(int buildId) {
	for (int i =0; i < MAX_ICONS; i++) {
		cBuildingListItem * itemInList = getItem(i);
		if (itemInList == nullptr) continue;

		// item already in list (same build id)
        if (buildId == itemInList->getBuildId()) {
            return itemInList;
        }
	}
	return nullptr;
}

void cBuildingList::addUpgradeToList(int upgradeType) {
    addItemToList(new cBuildingListItem(upgradeType, upgrades[upgradeType], upgradeType));
}

void cBuildingList::addStructureToList(int structureType, int subList) {
    addItemToList(new cBuildingListItem(structureType, structures[structureType], subList));
}

void cBuildingList::addUnitToList(int unitType, int subList) {
    addItemToList(new cBuildingListItem(unitType, units[unitType], subList));
}

bool cBuildingList::addItemToList(cBuildingListItem * item) {
	if (isItemInList(item)) {
		logbook("Will not add, item is already in list.");
		// item is already in list, do not add
		return false;
	}

	int slotId = getFreeSlot();
	if (slotId < 0 ) {
		logbook("Failed to add icon to cBuildingList, no free slot left in list");
        return false;
	}

	// add
	items[slotId] = item;
	item->setSlotId(slotId);
	item->setList(this);
	maxItems = slotId + 1;
//	char msg[355];
//	sprintf(msg, "Icon added with id [%d] added to cBuilding list, put in slot[%d], set maxItems to [%d]", item->getBuildId(), slot, maxItems);
//	logbook(msg);
    return true;
}

bool cBuildingList::removeItemFromList(cBuildingListItem * item) {
    if (item == nullptr) return false;
    return removeItemFromList(item->getSlotId());
}

bool cBuildingList::removeItemFromListByBuildId(int buildId) {
    cBuildingListItem * item = getItemByBuildId(buildId);
    return removeItemFromList(item);
}
/**
 * Remove item from list. Delete item object and set NULL in array. Makes sure to shift all items so that
 * there won't be gaps
 *
 * @param position
 */
bool cBuildingList::removeItemFromList(int position) {
    if (position < 0) return false;
    if (position >= MAX_ICONS) return false;

	cBuildingListItem * item = getItem(position);
	if (item == nullptr) {
		// item can be null, in that case do nothing.
		return false;
	}

	delete item;
    items[position] = nullptr;

    // starting from 'position' which became NULL, make sure everything
    // after that slotIndex is moved. So we don't get gaps.
    for (int i = (position + 1); i < maxItems; i++) {
        items[i-1] = items[i];
        items[i-1]->setSlotId(i-1);

        // and clear it out, which in the next loop will be filled
        // if there is any other pointer. If not, the 'last' item is NULL now.
        items[i] = nullptr;
    }

    maxItems--; // now we can do this
    return true;
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
 * @return std::array<int, 5>
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
