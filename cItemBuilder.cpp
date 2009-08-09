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
	// go through all the items and increase progress counters...
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

							item->decreaseTimesToBuild(); // decrease amount of times to build

							assert(item->getTimesToBuild() > -1);

							// construct unit

							// stop building this item when we are done
							if (item->getTimesToBuild() == 0) {
								item->setIsBuilding(false);
								item->setProgress(0); // set back progress

								// now try to find an item that is in the same list. If so, start building it.
								cBuildingListItem *similarItem = getSimilarListType(item);

								removeItemFromList(item); // remove, not needed to evaluate anymore

								if (similarItem && similarItem->canPay()) {
									player[0].credits -= similarItem->getBuildCost();
									similarItem->setIsBuilding(true);
								}

							} else {
								item->setProgress(0); // set back progress

								if (item->canPay()) {
									item->setIsBuilding(true);
									player[HUMAN].credits -= item->getBuildCost();
								} else {
									// stop building when cannot pay it.
									item->setIsBuilding(false);
								}
							}
						}
					}

					timers[i] = 0;
				}
			} else {
				// not building now, but in list.
				// Build as soon as possible.

				bool anotherItemOfSameListIsBeingBuilt = getSimilarListType(item);

				// only start building this, if no other item is already being built in the same list.
				if (!anotherItemOfSameListIsBeingBuilt && item->canPay()) {
					player[HUMAN].credits -= item->getBuildCost();
					item->setIsBuilding(true);
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
	assert(item != NULL);
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
	item->increaseTimesToBuild();

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
			if (listItem == item) continue; // do not check self

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
 * Remove item from list; finds its index and removes it.
 * @param item
 */
void cItemBuilder::removeItemFromList(cBuildingListItem *item) {
	if (item == NULL) return;

	int indexToDelete = -1;
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *theItem = getItem(i);
		if (theItem == item) {
			indexToDelete = i;
			break;
		}
	}

	if (indexToDelete > -1) {
		removeItemFromList(indexToDelete);
	} else {
		// log statement?
	}
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

		// DO NOT DELETE HERE, it will delete the instance; that is not what should happen here.
		// only remove the reference (pointer) to it.
		//delete item;
		assert(item->isBuilding() == false);
//		item->setIsBuilding(false);
		items[position] = NULL;
		timers[position] = 0;
	}

}

cBuildingListItem * cItemBuilder::getItem(int position) {
	assert(position > -1);
	assert(position < MAX_ITEMS);
	return items[position];
}
