#include "..\d2tmh.h"

cBuildingList::cBuildingList() {
	TIMER_progress = 0;
	lastBuiltId = 0;
	buttonIconId = 0;	// the button to draw at the left of the list
	buttonDrawX = 0;
	buttonDrawY = 0;
	scrollingOffset = 0; 	// the offset for scrolling through the list.
	available = false;		 // is this list available?

	memset(items, NULL, sizeof(items));
}

cBuildingList::~cBuildingList() {
	TIMER_progress = 0;
	lastBuiltId = 0;
	buttonIconId = 0;	// the button to draw at the left of the list
	buttonDrawX = 0;
	buttonDrawY = 0;
	scrollingOffset = 0; 	// the offset for scrolling through the list.
	available = false;		 // is this list available?
	removeAllItems();
	memset(items, NULL, sizeof(items));
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

void cBuildingList::addItemToList(cBuildingListItem * item) {
	assert(item);
	int slot = getFreeSlot();
	if (slot < 0 ) {
		logbook("Failed to add icon to cBuildingList.");
		assert(false);
		return;
	}

	// add
	items[slot] = item;
	maxItems++;
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
	int drawX = getButtonDrawX();
	int drawY = getButtonDrawY();
	return (x >= drawX && x <= (drawX + 40) && (y >= drawY && y <= (drawY + 40)));
}

void cBuildingList::setScrollingOffset(int value) {

	if (value > scrollingOffset) {
		if (value > (maxItems - 5)) value = (maxItems - 5);
	}
	else if (value < scrollingOffset) {
		if (value < 0) value = 0;
	}

	scrollingOffset = value;
}

/**
 * Scroll list up one item
 */
void cBuildingList::scrollUp() {
	logbook("cBuildingList::scrollUp");
	int offset = getScrollingOffset() - 1;
	setScrollingOffset(offset);
}

/**
 * Scroll list down one item
 */
void cBuildingList::scrollDown() {
	logbook("cBuildingList::scrollDown");
	int offset = getScrollingOffset() + 1;
	setScrollingOffset(offset);
}
