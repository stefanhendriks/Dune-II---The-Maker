#include "../include/d2tmh.h"

cItemBuilder::cItemBuilder(cPlayer * thePlayer, cBuildingListUpdater * buildingListUpdater) {
	assert(thePlayer);
	this->m_Player = thePlayer;
    this->buildingListUpdater = buildingListUpdater;
	removeAllItems();
	memset(timers, 0, sizeof(timers));
}

cItemBuilder::~cItemBuilder() {
    removeAllItems();
}

int cItemBuilder::getTimerCap(cBuildingListItem *item) {
	int iTimerCap = 35; // was 35 = ORIGINAL

    // when m_Player has low power, produce twice as slow
    if (item->getBuildType() == UNIT) {
        // the given unit will get out of a specific structure. This type
        // is within the units properties.
        int structureTypeItLeavesFrom = units[item->getBuildId()].structureTypeItLeavesFrom;
        int structureCount = m_Player->getAmountOfStructuresForType(structureTypeItLeavesFrom);
        if (structureCount > 1) {
            iTimerCap /= structureCount;
        }
    } else if (item->getBuildType() == STRUCTURE) {
        // the given unit will get out of a specific structure. This type
        // is within the units properties.
        int structureCount = m_Player->getAmountOfStructuresForType(CONSTYARD);
        if (structureCount > 1) {
            iTimerCap /= structureCount;
        }
    }

    cPlayerDifficultySettings *difficultySettings = m_Player->getDifficultySettings();
    iTimerCap = difficultySettings->getBuildSpeed(iTimerCap);

    if (!m_Player->bEnoughPower()) {
        iTimerCap *= 6; // make painful
    }

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
            bool anotherItemOfSameListIsBeingBuilt = isAnotherBuildingListItemInTheSameListBeingBuilt(item);

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
            if (!item->shouldPlaceIt()) {
                if (m_Player->isHuman()) {
                    play_voice(SOUND_VOICE_01_ATR); // "Construction Complete"
                }
                item->setPlaceIt(true);
            }
        } else if (item->getBuildType() == UNIT) {
            buildingListUpdater->onBuildItemCompleted(item);
            item->decreaseTimesToBuild(); // decrease amount of times to build

            assert(item->getTimesToBuild() > -1);

            // TODO: Remove duplication, which also exists in AI::think_buildingplacement()
            if (!units[item->getBuildId()].airborn) {
                deployUnit(item, item->getBuildId());
            } else {
                // airborn unit
                int structureToDeployUnit = structureUtils.findHiTechToDeployAirUnit(m_Player);
                if (structureToDeployUnit > -1) {
                    cAbstractStructure *pStructureToDeploy = structure[structureToDeployUnit];
                    pStructureToDeploy->setAnimating(true); // animate
                    int unitId = UNIT_CREATE(pStructureToDeploy->getCell(), item->getBuildId(), m_Player->getId(), false);
                    int rallyPoint = pStructureToDeploy->getRallyPoint();
                    if (rallyPoint > -1) {
                        unit[unitId].move_to(rallyPoint, -1, -1);
                    }
                } else {
                    // got destroyed very recently
                }
            }

        } else if (item->getBuildType() == SPECIAL) {
            buildingListUpdater->onBuildItemCompleted(item);
            item->decreaseTimesToBuild(); // decrease amount of times to build
            const s_Special &special = item->getS_Special();

            if (special.deployAt == AT_STRUCTURE) {
                if (special.providesType == UNIT) {
                    deployUnit(item, special.providesTypeId);
                }
                removeItemFromList(item);
            } else if (special.deployAt == AT_RANDOM_CELL) {
                if (special.providesType == UNIT) {
                    // determine cell
                    cCellCalculator cellCalculator = cCellCalculator(&map);
                    int iCll = cellCalculator.getCellWithMapBorders(4 + rnd(game.map_width - 8), 4 + rnd(game.map_height - 8));

                    for (int j = 0; j < special.units; j++) {
                        bool passable = map.isCellPassableAndNotBlockedForFootUnits(iCll);

                        if (passable) {
                            UNIT_CREATE(iCll, special.providesTypeId, FREMEN, false);
                        } else {
                            REINFORCE(FREMEN, special.providesTypeId, iCll, -1);
                        }

                        int x = iCellGiveX(iCll);
                        int y = iCellGiveY(iCll);
                        int amount = rnd(2) + 1;

                        // randomly shift the cell one coordinate up/down/left/right
                        switch (rnd(4)) {
                            case 0:
                                x += amount;
                                break;
                            case 1:
                                y+= amount;
                                break;
                            case 2:
                                x -= amount;
                                break;
                            case 3:
                                y -= amount;
                                break;
                        }
                        // change cell
                        FIX_POS(x, y);

                        iCll = cellCalculator.getCell(x, y);
                    }
                }

                removeItemFromList(item);
            }

            if (special.deployAt == AT_STRUCTURE) {
                item->setDeployIt(true);
            }
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
            item->resetProgress(); // set back progress

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
            item->resetProgress(); // set back progress
            startBuilding(item);
        }
	}
}

void cItemBuilder::deployUnit(cBuildingListItem *item, int buildId) const {
    int structureTypeByItem = structureUtils.findStructureTypeByTypeOfList(item);
    assert(structureTypeByItem > -1);
    int structureToDeployUnit = structureUtils.findStructureToDeployUnit(m_Player, structureTypeByItem);
    int buildIdToProduce = buildId;
    if (structureToDeployUnit > -1) {
        cAbstractStructure *pStructureToDeploy = structure[structureToDeployUnit];
        // TODO: Remove duplication, which also exists in AI::think_buildingplacement()
        int cell = pStructureToDeploy->getNonOccupiedCellAroundStructure();
        if (cell > -1) {
            pStructureToDeploy->setAnimating(true); // animate
            int unitId = UNIT_CREATE(cell, buildIdToProduce, m_Player->getId(), false);
            int rallyPoint = pStructureToDeploy->getRallyPoint();
            if (rallyPoint > -1) {
                unit[unitId].move_to(rallyPoint, -1, -1);
            }
        } else {
            logbook("cItemBuilder: huh? I was promised that this structure would have some place to deploy unit at!?");
        }
    } else {
        structureToDeployUnit = m_Player->getPrimaryStructureForStructureType(structureTypeByItem);
        if (structureToDeployUnit < 0) {
            // find any structure of type (regardless if we can deploy or not)
            for (int structureId = 0; structureId < MAX_STRUCTURES; structureId++) {
                cAbstractStructure *pStructure = structure[structureId];
                if (pStructure &&
                    pStructure->isValid() &&
                    pStructure->belongsTo(m_Player->getId()) &&
                    pStructure->getType() == structureTypeByItem) {
                    structureToDeployUnit = structureId;
                    break;
                }
            }
        }

        int cellToDeploy = structure[structureToDeployUnit]->getCell();
        cAbstractStructure * pStructureToDeploy = structure[structureToDeployUnit];
        if (pStructureToDeploy->getRallyPoint() > -1) {
            cellToDeploy = pStructureToDeploy->getRallyPoint();
        }

        REINFORCE(m_Player->getId(), buildIdToProduce, cellToDeploy, -1);
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

/**
 * Removes all references to items, because this itemBuilder does not *own* the items it is building
 */
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
	if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
        item->increaseTimesToBuildNTimes(4); // 4 times makes more sense (for structures ~ 4 slabs)
	} else {
        item->increaseTimesToBuild();
    }

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

/**
 * Checks if another cBuildingListItem exists which has the same list ID, and is building. If so returns true.
 * @param item
 * @return
 */
bool cItemBuilder::isAnotherBuildingListItemInTheSameListBeingBuilt(cBuildingListItem *item) {
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
 * Returns true if any item is being built for the listType / subListType.
 * If you wish to know which item , use getListItemWhichIsBuilding()
 * @param listType
 * @param sublistType
 * @return
 */
bool cItemBuilder::isAnythingBeingBuiltForListId(int listType, int sublistType) {
    cBuildingListItem *pItem = getListItemWhichIsBuilding(listType, sublistType);
    return pItem != nullptr;
}

/**
 * Returns true if any item awaits placement for the listType / subListType.
 * If you wish to know which item , use getListItemWhichIsAwaitingPlacement()
 * @param listType
 * @param sublistType
 * @return
 */
bool cItemBuilder::isAnythingBeingBuiltForListIdAwaitingPlacement(int listType, int sublistType) {
    cBuildingListItem *pItem = getListItemWhichIsAwaitingPlacement(listType, sublistType);
    return pItem != nullptr;
}

/**
 * Iterates over listType and subList, finds any item that isBuilding() returns true.
 * @param listType
 * @param sublistType
 * @return
 */
cBuildingListItem *cItemBuilder::getListItemWhichIsBuilding(int listType, int sublistType) {
    cBuildingListItem *pItem = nullptr; // get through the build list and find an item that is of the same list.

    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *listItem = getItem(i);
        if (listItem) {
            cBuildingList *pList = listItem->getList();
            if (listType == pList->getType() &&
                sublistType == listItem->getSubList()) {
                if (listItem->isBuilding()) {
                    pItem = listItem;
                    break;
                }
            }
        }
    }
    return pItem;
}

/**
 * Iterates over listType and subList, finds any item that isBuilding() returns true.
 * @param listType
 * @param sublistType
 * @return
 */
cBuildingListItem *cItemBuilder::getListItemWhichIsAwaitingPlacement(int listType, int sublistType) {
    cBuildingListItem *pItem = nullptr;

    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *listItem = getItem(i);
        if (listItem) {
            cBuildingList *pList = listItem->getList();
            // check if list is available
            if (pList && listType == pList->getType() &&
                sublistType == listItem->getSubList()) {
                if (listItem->shouldPlaceIt()) {
                    pItem = listItem;
                    break;
                }
            }
        }
    }
    return pItem;
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
 * Finds item being built by eBuildType (UNIT/STRUCTURE/SPECIAL) and BuildId (TRIKE, QUAD, WINDTRAP, NUKE)
 * @param buildType
 * @param iBuildId
 * @return
 */
cBuildingListItem * cItemBuilder::getBuildingListItem(eBuildType buildType, int iBuildId) {
    for (int i = 0; i < MAX_ICONS; i++) {
        cBuildingListItem *pItem = items[i];
        if (pItem == nullptr) continue;
        if (pItem->getBuildType() != buildType) continue;
        if (pItem->getBuildId() != iBuildId) continue;
        return pItem;
    }
    return nullptr;
}

/**
 * Remove item from list. This does do any delete operation, it merely sets the pointer to NULL. This is because
 * the item builder does not own any of the items being built.
 *
 * @param position
 */
void cItemBuilder::removeItemFromList(int position) {
	assert(position > -1);
	assert(position < MAX_ICONS);

	// remove
	items[position] = nullptr;
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

void cItemBuilder::removeItemsFromListType(eListType listType, int subListId) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *theItem = getItem(i);
        if (theItem == nullptr) continue;
        if (theItem->getListType() != listType) continue;
        if (theItem->getSubList() != subListId) continue;
        removeItemFromList(theItem);
    }
}

void cItemBuilder::removeItemsByBuildId(eBuildType buildType, int buildId) {
    for (int i = 0; i < MAX_ICONS; i++) {
        cBuildingListItem *pItem = items[i];
        if (pItem == nullptr) continue;
        if (pItem->getBuildType() != buildType) continue;
        if (pItem->getBuildId() != buildId) continue;

        removeItemFromList(pItem);
    }
}

