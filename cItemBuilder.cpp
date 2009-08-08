/*
 * cItemBuilder.cpp
 *
 *  Created on: Aug 8, 2009
 *      Author: Stefan
 */

#include "d2tmh.h"

cItemBuilder::cItemBuilder() {
	removeAllItems();
	memset(timers, 0, sizeof(timers));
}

cItemBuilder::~cItemBuilder() {
}

cItemBuilder *cItemBuilder::instance = NULL;

cItemBuilder *cItemBuilder::getInstance() {
	if (instance == NULL) {
		instance = new cItemBuilder();
	}

	return instance;
}
int cItemBuilder::getTimerCap(cBuildingList *list, cBuildingListItem *item) {
	int iTimerCap = 35; // was 35 = ORIGINAL

	if (DEBUGGING) {
		iTimerCap = 3;
	}

	// when player has low power, produce twice as slow
	if (player[0].bEnoughPower() == false) {
		iTimerCap *= 3; // make painful
	} else {
		if (list->getType() == LIST_CONSTYARD) {
			iTimerCap /= (1+(player[0].iStructures[item->getBuildId()]/2));
		}
	}

	iTimerCap = player[0].iBuildSpeed(iTimerCap);

	return iTimerCap;
}

int cItemBuilder::getBuildTime(cBuildingList *list, cBuildingListItem *item) {
	// get max build time:
	int iBuildTime = 0;

	if (list->getType() == LIST_CONSTYARD) {
		iBuildTime = structures[item->getBuildId()].build_time;
	} else {
		iBuildTime = units[item->getBuildId()].build_time;
	}

	return iBuildTime;
}

void cItemBuilder::think() {
	// go through all the items and increase progress counters... yay
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *item = getItem(i);
		if (item) {
			if (item->isBuilding()) {
				timers[i]++;
				cBuildingList *list = item->getList();

				int timerCap = getTimerCap(list, item);

				// build stuff
				if (timers[i] >= timerCap) {
					int buildTime = getBuildTime(list, item);
					// increase progress
					item->setProgress((item->getProgress() + 1));

					// VOICE: construction complete
					if (item->getProgress() == buildTime) {
						if (list->getType() == LIST_CONSTYARD) {
							play_voice(SOUND_VOICE_01_ATR);
						} else {

							item->setTimesToBuild((item->getTimesToBuild() - 1)); // decrease amount of times to build

							assert(item->getTimesToBuild() > -1);

							// construct unit

							// stop building this item when we are done
							if (item->getTimesToBuild() == 0) {
								item->setIsBuilding(false);

								// now try to find an item that is in the same list. If so, start building it.
								cBuildingListItem *similarItem = getSimilarListType(item);

								if (similarItem && similarItem->canPay()) {
									player[0].credits -= similarItem->getBuildCost();
									similarItem->setIsBuilding(true);
								}

								removeItemFromList(i); // remove, not needed to evaluate anymore
							}
						}
					}

					timers[i] = 0;
				}
			}
		}
	}
}

/**
 * Return a flee slot within the array to be used.
 *
 * @return
 */
int cItemBuilder::getFreeSlot() {
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (items[i] == NULL) {
			return i; // return free slot
		}
	}

	return -1; // no free slot
}

void cItemBuilder::removeAllItems() {
	for (int i =0 ; i < MAX_ITEMS; i++) {
		removeItemFromList(i);
	}
}

/**
 * Adds an item to list (to build). Set the item (isBuilding to true) building to true
 * when it is the first (and only) of its type.
 *
 * when item already exists, it will up the 'amount' to build.
 *
 * @param item
 */
void cItemBuilder::addItemToList(cBuildingListItem * item) {
	assert(item);
	int slot = getFreeSlot();
	if (slot < 0) {
		assert(false);
		return;
	}

	if (isTheFirstListType(item)) {
		item->setIsBuilding(true); // build it immediately
		// pay it
		if (item->canPay()) {
			player[0].credits -= item->getBuildCost();
		}
	}

	// increase amount
	item->setTimesToBuild((item->getTimesToBuild() + 1));

	// add amount of times to build
	if (!isItemInList(item)) {
		items[slot] = item;
	}
}

bool cItemBuilder::isItemInList(cBuildingListItem *item) {
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (items[i] == item) {
			return true; // return slot where this item is
		}
	}

	return false;
}

/**
 * Returns the cBuildingListItem pointer of a similar item of the same type
 *
 * @param item
 * @return
 */
cBuildingListItem *cItemBuilder::getSimilarListType(cBuildingListItem *item) {
	if (item == NULL) return NULL;

	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *listItem = getItem(i);
		if (listItem) {
			if (item->getList() == listItem->getList()) {
				return item;
			}
		}
	}
	return NULL;
}

bool cItemBuilder::isTheFirstListType(cBuildingListItem *item) {
	return getSimilarListType(item) == NULL;
}

/**
 * Remove item from list. Delete item object and set NULL in array.
 *
 * @param position
 */
void cItemBuilder::removeItemFromList(int position) {
	assert(position > -1);
	assert(position < MAX_ICONS);
	cBuildingListItem * item = getItem(position);
	if (item == NULL) {
		// item can be null, in that case do nothing.
	} else {
		delete item;
		items[position] = NULL;
	}

}

cBuildingListItem * cItemBuilder::getItem(int position) {
	assert(position > -1);
	assert(position < MAX_ITEMS);
	return items[position];
}
