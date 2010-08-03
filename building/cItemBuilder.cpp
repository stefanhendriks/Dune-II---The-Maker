/*
 * cItemBuilder.cpp
 *
 *  Created on: Aug 8, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cItemBuilder::cItemBuilder(cPlayer * thePlayer) {
	assert(thePlayer);
	removeAllItems();
	memset(timers, 0, sizeof(timers));
	player = thePlayer;
}

cItemBuilder::~cItemBuilder() {
}

int cItemBuilder::getTimerCap(cBuildingList *list, cBuildingListItem *item) {
	int iTimerCap = 35; // was 35 = ORIGINAL

	if (DEBUGGING) {
		iTimerCap = 3;
	}

	// when player has low power, produce twice as slow
	if (player->bEnoughPower() == false) {
		iTimerCap *= 3; // make painful
	} else {
		if (list->getType() != LIST_CONSTYARD) {
			// the given unit will get out of a specific structure. This type
			// is within the units properties.
			int structureTypeItLeavesFrom = units[item->getBuildId()].structureTypeItLeavesFrom;
			if (structureTypeItLeavesFrom > -1) {
				iTimerCap /= (1+(player->iStructures[structureTypeItLeavesFrom]/2));
			}
		}
	}

	cPlayerDifficultySettings *difficultySettings = player->getDifficultySettings();
	iTimerCap = difficultySettings->getBuildSpeed(iTimerCap);

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

				float priceForTimeUnit = item->getCreditsPerProgressTime();

				// build stuff
				if (timers[i] >= timerCap) {
					int buildTime = getBuildTime(list, item);

					bool isDoneBuilding = item->getProgress() >= buildTime;

					// Not done building yet , and can pay for progress?
					if (!isDoneBuilding && !item->shouldPlaceIt() &&
						player->credits > priceForTimeUnit) {
						// increase progress
						item->setProgress((item->getProgress() + 1));
						// pay
						player->credits -= priceForTimeUnit;
					}

					if (isDoneBuilding) {
						bool isAbleToBuildNewOneImmidiately = true;
						if (item->getBuildType() == STRUCTURE) {

							isAbleToBuildNewOneImmidiately = false;

							// play voice when placeIt is false
							if (!item->shouldPlaceIt() && (player == &player[HUMAN])) {
								play_voice(SOUND_VOICE_01_ATR);
								item->setPlaceIt(true);
							}

						} else if (item->getBuildType() == UNIT){

							item->decreaseTimesToBuild(); // decrease amount of times to build

							assert(item->getTimesToBuild() > -1);

							int structureTypeOfList = structureUtils.findStructureTypeByTypeOfList(list, item);
							assert(structureTypeOfList > -1);
							int primaryBuildingIdOfStructureType = structureUtils.findStructureToDeployUnit(player, structureTypeOfList);

							if (primaryBuildingIdOfStructureType > -1) {
								int cell = structure[primaryBuildingIdOfStructureType]->iFreeAround();
								cAbstractStructure * theStructure = structure[primaryBuildingIdOfStructureType];
								theStructure->setAnimating(true); // animate
								// TODO: construct unit here
								int unitId = UNIT_CREATE(cell, item->getBuildId(), player->getId(), false);
								int rallyPoint = theStructure->getRallyPoint();
								if (rallyPoint > -1) {
									unit[unitId].move_to(rallyPoint, -1, -1);
								}
							}
						} else if (item->getBuildType() == SPECIAL) {
							// super weapons and that kind of stuff

							// requires user interaction
							isAbleToBuildNewOneImmidiately = false;
						}

						if (isAbleToBuildNewOneImmidiately) {
							// stop building this item when we are done
							if (item->getTimesToBuild() == 0) {	// no more items to build
								// stop building (set flags)
								item->setIsBuilding(false);
								item->setProgress(0); // set back progress

								// remove this item from the build list (does not delete item, so pointer is still valid)
								removeItemFromList(item);

								// now try to find an item that is in the same list. If so, start building it.
								cBuildingListItem *itemInSameList = getSimilarListType(item);

								// found item
								if (itemInSameList) {
									itemInSameList->setIsBuilding(true);
								}

							} else {
								// item still needs to be built more times.
								item->setProgress(0); // set back progress
								item->setIsBuilding(true);
							}
						}
					}

					timers[i] = 0;
				}
			} else {
				// not building now, but in list.
				// Build as soon as possible.

				bool anotherItemOfSameListIsBeingBuilt = isASimilarItemBeingBuilt(item);

				// only start building this, if no other item is already being built in the same list.
				if (!anotherItemOfSameListIsBeingBuilt) {
					item->setIsBuilding(true);
				}
			}
		} else {
			// item not valid (no pointer)
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

	// check if there is a similiar type in the list
	if (isTheFirstListType(item)) {
		item->setIsBuilding(true); // build it immediately
	}

	// increase amount
	item->increaseTimesToBuild();

	if (!isItemInList(item)) {
		cLogger::getInstance()->log(LOG_TRACE, COMP_SIDEBAR, "Add item to item builder", "item is not in list, adding.");
		// add to list
		items[slot] = item;
	} else {
		cLogger::getInstance()->log(LOG_TRACE, COMP_SIDEBAR, "Add item to item builder", "item is in list already. Only times to build is updated.");
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
	assert(item != NULL);

	// get through the build list and find an item that is of the same list.
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *listItem = getItem(i);
		if (listItem) {
			if (listItem == item) continue; // do not check self

			if (item->getList() == listItem->getList()) {
				return listItem;
			}
		}
	}
	return NULL;
}

bool cItemBuilder::isASimilarItemBeingBuilt(cBuildingListItem *item) {
	assert(item != NULL);

	// get through the build list and find an item that is of the same list.
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *listItem = getItem(i);
		if (listItem) {
			if (listItem == item) continue; // do not check self

			if (item->getList() == listItem->getList()) {
				if (listItem->isBuilding()) return true;
			}
		}
	}
	return false;
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

	// remove
	items[position] = NULL;
	timers[position] = 0;
}

cBuildingListItem * cItemBuilder::getItem(int position) {
	assert(position > -1);
	assert(position < MAX_ITEMS);
	return items[position];
}
