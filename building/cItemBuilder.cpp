#include "../include/d2tmh.h"
#include "cItemBuilder.h"


cItemBuilder::cItemBuilder(cPlayer * thePlayer, cBuildingListUpdater * buildingListUpdater) {
	assert(thePlayer);
	this->m_Player = thePlayer;
    this->buildingListUpdater = buildingListUpdater;
	removeAllItems();
	memset(timers, 0, sizeof(timers));
}

cItemBuilder::~cItemBuilder() {
}

int cItemBuilder::getTimerCap(cBuildingListItem *item) {
	int iTimerCap = 35; // was 35 = ORIGINAL

	// when m_Player has low power, produce twice as slow
	if (m_Player->bEnoughPower() == false) {
		iTimerCap *= 6; // make painful
	} else {
		if (item->getBuildType() == UNIT) {
			// the given unit will get out of a specific structure. This type
			// is within the units properties.
			int structureTypeItLeavesFrom = units[item->getBuildId()].structureTypeItLeavesFrom;
			if (structureTypeItLeavesFrom > -1) {
				iTimerCap /= (1+(m_Player->getAmountOfStructuresForType(structureTypeItLeavesFrom) / 2));
			}
		}
	}

	cPlayerDifficultySettings *difficultySettings = m_Player->getDifficultySettings();
	iTimerCap = difficultySettings->getBuildSpeed(iTimerCap);

	return iTimerCap;
}

void cItemBuilder::think() {
	// go through all the items and increase progress counters...
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *item = getItem(i);
		if (item == nullptr) continue;

        // not building now, but in list.
        // Build as soon as possible.
        if (!item->isBuilding()) {
            bool anotherItemOfSameListIsBeingBuilt = isASimilarItemBeingBuilt(item);

            // only start building this, if no other item is already being built in the same list.
            if (!anotherItemOfSameListIsBeingBuilt) {
                startBuilding(item);
            }
            continue;
        }

        // ITEM is building
        timers[i]++;

        // determines how fast an item is built, this is the so called 'delay' before 1 'build time tick' has passed
        int timerCap = getTimerCap(item);

        // not yet done building
        if (timers[i] < timerCap) continue;

        // reset timer for next tick
        timers[i] = 0;

        // timer reached cap, so one 'tick' is reached for building this item
        bool isDoneBuilding = item->isDoneBuilding();

        float priceForTimeUnit = item->getCreditsPerProgressTime();

        if (!isDoneBuilding) {
            // Not done building yet , and can pay for progress?
            if (!item->shouldPlaceIt() && m_Player->hasEnoughCreditsFor(priceForTimeUnit)) {
                // increase progress
                item->increaseProgress(1);
                // pay
                m_Player->substractCredits(priceForTimeUnit);
            }
            continue;
        }

        // DONE building
        if (item->getBuildType() == STRUCTURE) {
            // play voice when placeIt is false
            if (!item->shouldPlaceIt() && (m_Player->isHuman())) {
                play_voice(SOUND_VOICE_01_ATR); // "Construction Complete"
                item->setPlaceIt(true);
            }
        } else if (item->getBuildType() == UNIT) {
            buildingListUpdater->onBuildItemCompleted(item);
            item->decreaseTimesToBuild(); // decrease amount of times to build

            assert(item->getTimesToBuild() > -1);

            int structureTypeByItem = structureUtils.findStructureTypeByTypeOfList(item);
            assert(structureTypeByItem > -1);
            int primaryBuildingIdOfStructureType = structureUtils.findStructureToDeployUnit(m_Player, structureTypeByItem);

            if (primaryBuildingIdOfStructureType > -1) {
                cAbstractStructure * theStructure = structure[primaryBuildingIdOfStructureType];
                int cell = theStructure->iFreeAround();
                theStructure->setAnimating(true); // animate
                int unitId = UNIT_CREATE(cell, item->getBuildId(), m_Player->getId(), false);
                int rallyPoint = theStructure->getRallyPoint();
                if (rallyPoint > -1) {
                    unit[unitId].move_to(rallyPoint, -1, -1);
                }
            }
        } else if (item->getBuildType() == SPECIAL) {
            buildingListUpdater->onBuildItemCompleted(item);
            // super weapons and that kind of stuff
        } else if (item->getBuildType() == UPGRADE) {
            buildingListUpdater->onUpgradeCompleted(item);
            removeItemFromList(item);
//            list->removeItemFromList(item->getSlotId()); // no need to explicitly remove from list, will be done by onUpgradeCompleted
            continue;
        }

        bool isAbleToBuildNewOneImmediately = item->getBuildType() == UNIT;

        if (!isAbleToBuildNewOneImmediately) continue;

        // stop building this item when we are done
        if (item->getTimesToBuild() == 0) {	// no more items to build
            // stop building (set flags)
            item->setIsBuilding(false);
            item->setProgress(0); // set back progress

            // remove this item from the build list (does not delete item, so pointer is still valid)
            removeItemFromList(item);

            // now try to find an item that is in the same list. If so, start building it.
            cBuildingListItem *itemInSameList = findBuildingListItemOfSameListAs(item);

            // found item
            if (itemInSameList) {
                startBuilding(itemInSameList);
            }

        } else {
            // item still needs to be built more times.
            item->setProgress(0); // set back progress
            startBuilding(item);
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
	    logbook("ERROR: Unable to add item to list because no slots are free!");
		return;
	}

	// check if there is a similar type in the list
	if (isBuildListItemTheFirstOfItsListType(item)) {
        // build it immediately, if we don't do this the start of building is a bit delayed by the think timer
        // of the ItemBuilder.
		startBuilding(item);
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
cBuildingListItem *cItemBuilder::findBuildingListItemOfSameListAs(cBuildingListItem *item) {
	assert(item != NULL);

	// get through the build list and find an item that is of the same list.
	for (int i = 0; i < MAX_ITEMS; i++) {
		cBuildingListItem *listItem = getItem(i);
        if (!listItem) continue;
        if (item->getList() == listItem->getList() &&
            item->getSubList() == listItem->getSubList()) {
            return listItem;
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

/**
 * Returns true when the buildingListItem is the first built in its list.
 * Ie, the list is of type ConstYard. Then if there are no other buildings being constructed, this method
 * returns true. Else, it returns false.
 *
 * @param item
 * @return
 */
bool cItemBuilder::isBuildListItemTheFirstOfItsListType(cBuildingListItem *item) {
	return findBuildingListItemOfSameListAs(item) == NULL;
}

/**
 * Remove item from list; finds its index and removes it.
 * @param item
 */
void cItemBuilder::removeItemFromList(cBuildingListItem *item) {
	if (item == nullptr) return;

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
 * Remove item from list. This does do any delete operation, it merely sets the pointer to NULL
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

void cItemBuilder::startBuilding(cBuildingListItem *item) {
    if (item == nullptr) return;
    item->setIsBuilding(true);
    buildingListUpdater->onBuildItemStarted(item);
}
