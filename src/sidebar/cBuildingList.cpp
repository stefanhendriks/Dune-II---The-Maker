#include "../include/d2tmh.h"

cBuildingList::cBuildingList(int theId) {
	TIMER_progress = 0;
	lastClickedId = 0;
	buttonIconIdPressed = 0; // the button to draw at the left of the list
	buttonIconIdUnpressed = 0;
	buttonDrawX = 0;
	buttonDrawY = 0;
	scrollingOffset = 0; // the offset for scrolling through the list.
	available = false; // is this list available?
	memset(items, NULL, sizeof(items));
	typeOfList = theId;
	upgradeLevel = 0;
	upgrading = false;
	maxItems = 0;
	acceptsOrders = true;
}

cBuildingList::~cBuildingList() {
	TIMER_progress = 0;
	lastClickedId = 0;
	buttonIconIdPressed = 0; // the button to draw at the left of the list
	buttonIconIdUnpressed = 0;
	buttonDrawX = 0;
	buttonDrawY = 0;
	scrollingOffset = 0; // the offset for scrolling through the list.
	available = false; // is this list available?
	removeAllItems();
	memset(items, NULL, sizeof(items));
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
	for (int i = 0; i < MAX_ICONS; i++) {
		removeItemFromList(i);
	}
}

bool cBuildingList::isItemInList(cBuildingListItem * item) {
	assert(item);
	return hasItemType(item->getBuildId());
}

bool cBuildingList::hasItemType(int itemTypeId) {
	assert(itemTypeId >= 0);
	for (int i = 0; i < MAX_ICONS; i++) {
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

void cBuildingList::addItemToList(cBuildingListItem * item) {
	assert(item);

	if (isItemInList(item)) {
		logbook("Failed to add icon to cBuildingList, item is already in list.");
		// item is already in list, do not add
		return;
	}

	int slot = getFreeSlot();
	if (slot < 0) {
		logbook("Failed to add icon to cBuildingList, no free slot left in list");
		assert(false);
		return;
	}

	// add
	items[slot] = item;
	item->setSlotId(slot);
	maxItems = slot;
	//	char msg[355];
	//	sprintf(msg, "Icon added with id [%d] added to cBuilding list, put in slot[%d], set maxItems to [%d]", item->getBuildId(), slot, maxItems);
	//	logbook(msg);
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
		item = NULL;
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
	int drawX = getButtonDrawX();
	int drawY = getButtonDrawY();
	return (x >= drawX && x <= (drawX + 51) && (y >= drawY && y <= (drawY + 37)));
}

void cBuildingList::setScrollingOffset(int value) {
	assert(value > -1);
	assert(value < MAX_ICONS);
	scrollingOffset = value;
}

/**
 * Scroll list up one item
 */
void cBuildingList::scrollUp() {
	logbook("cBuildingList::scrollUp");
	int offset = getScrollingOffset() - 1;
	if (offset > -1) {
		setScrollingOffset(offset);
	}
}

/**
 * Scroll list down one item
 */
void cBuildingList::scrollDown() {
	logbook("cBuildingList::scrollDown");
	int oldOffset = getScrollingOffset();
	int offset = oldOffset + 1;
	int max = maxItems - 4;

	char msg[255];
	sprintf(msg, "old offset is [%d], new offset is [%d], maxItems is [%d] max is [%d].", oldOffset, offset, maxItems, max);
	logbook(msg);

	if (offset <= max) {
		logbook("cBuildingList::scrolling down");
		setScrollingOffset(offset);
	} else {
		logbook("cBuildingList::scrolling down not allowed");
	}
}

/**
 * Return true if an item is being built in this list.
 *
 * @return
 */
bool cBuildingList::isBuildingItem() {
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *item = getItem(i);
		// valid pointer
		if (item) {
			// get isBuilding
			if (item->isBuilding() || item->shouldPlaceIt()) {
				return true;
			}
		}
	}
	return false;
}

cBuildingListItem * cBuildingList::getItemToPlace() {
	for (int i = 0; i < MAX_ITEMS; i++) {
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
