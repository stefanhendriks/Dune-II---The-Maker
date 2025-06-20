#include "cItemBuilder.h"

#include "d2tmc.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "gameobjects/units/cUnit.h"
#include "gameobjects/units/cReinforcements.h"
#include "player/cPlayer.h"
#include "player/cPlayerDifficultySettings.h"
#include "sGameEvent.h"
#include "sidebar/cBuildingList.h"
#include "structs.h"
#include "utils/cLog.h"
#include "utils/common.h"
#include "utils/cSoundPlayer.h"
#include "utils/d2tm_math.h"



#include <cassert>

cItemBuilder::cItemBuilder(cPlayer * thePlayer, cBuildingListUpdater * buildingListUpdater) :
        m_player(thePlayer),
        m_buildingListUpdater(buildingListUpdater),
        m_buildItemMultiplierEnabled(false)
{
	removeAllItems();
	memset(m_timers, 0, sizeof(m_timers));
}

cItemBuilder::~cItemBuilder() {
    removeAllItems();
}

/**
 * Timer cap is the 'delay' to consider how much the timer must have passed before progressing one tick for
 * building things. Ie, consider something has a buildTime of 200, this means the game timer (5 ms) has to run 200 times
 * which equals 1 second. However, from the 200 times, the delay timer has been increased, and only after the delay the
 * actual progress of building an item will pass.
 *
 * Meaning:
 * - 200 times gameloop
 * - default timer cap / delay = 35 times
 * - 200 / 35 = 5.714 progress items (or, 5 progress ticks, and still going on with the actual delay).
 * - do consider that the amount delay is based on chosen team. Some build slower, some faster. Ie, the 35 is baseline
 *   but is multiplied by the actual house difficulty settings. Atreides = baseline, Harkonnen builds slower; Ordos
 *   faster.
 *
 * BuildTime meaning:
 * - when a buildTime in the INI file is "100", this can be calculated into seconds like so:
 *   - 100 build time units * 35 = 3500 (3500 game ticks)
 *   - 3500 game ticks (* 5 miliseconds) = 17500 miliseconds
 *   - meaning: 17,5 seconds.
 * - the other way around:
 *  - given 17,5 seconds, that is 17500 milliseconds
 *  - 17500 / 5 = 3500 game ticks
 *  - 3500 / (default timer cap 35) = 100 build time
 *
 *  Example:
 *  If you want to have a build taking 1 minute (60 seconds), then:
 *  - 60 seconds -> 60000 milliseconds
 *  - 60000 / 5 = 12000 game ticks
 *  - 12000 / 35 = a buildTime of 342.85 (~ 343)
 *  (343 * 35 * 5 = 60025 ~ milliseconds)
 *
 * Also:
 * - when low power, the timer cap is increased so that building things become a real pain.
 *
 * @param item
 * @return
 */
int cItemBuilder::getTimerCap(cBuildingListItem *item) {
	int iTimerCap = game.isDebugMode() ? cBuildingListItem::DebugTimerCap : cBuildingListItem::DefaultTimerCap;

    // when player has low power, produce twice as slow
    if (item->getBuildType() == UNIT) {
        // the given unit will get out of a specific structure. This type
        // is within the units properties.
        int structureTypeItLeavesFrom = sUnitInfo[item->getBuildId()].structureTypeItLeavesFrom;
        int structureCount = m_player->getAmountOfStructuresForType(structureTypeItLeavesFrom);
        if (structureCount > 1) {
            iTimerCap /= structureCount;
        }
    } else if (item->getBuildType() == STRUCTURE) {
        // the given unit will get out of a specific structure. This type
        // is within the units properties.
        int structureCount = m_player->getAmountOfStructuresForType(CONSTYARD);
        if (structureCount > 1) {
            iTimerCap /= structureCount;
        }
    }

    cPlayerDifficultySettings *difficultySettings = m_player->getDifficultySettings();
    iTimerCap = difficultySettings->getBuildSpeed(iTimerCap);

    if (!m_player->bEnoughPower()) {
        iTimerCap *= 6; // make painful
    }

	return iTimerCap;
}

/**
 * Called every 5 ms
 */
void cItemBuilder::thinkFast() {
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
        m_timers[i]++;

        // determines how fast an item is built, this is the so called 'delay' before 1 'build time tick' has passed
        int timerCap = getTimerCap(item);
        item->setTimerCap(timerCap);

        // not yet done building
        if (m_timers[i] < timerCap) continue;

        // reset timer for next tick
        m_timers[i] = 0;

        // timer reached cap, so one 'tick' is reached for building this item
        bool isDoneBuilding = item->isDoneBuilding();

        float priceForTimeUnit = item->getCreditsPerProgressTime();

        if (!isDoneBuilding) {
            // Not done building yet , and can pay for progress?
            if (!item->shouldPlaceIt() && m_player->hasEnoughCreditsFor(priceForTimeUnit)) {
                // increase progress
                item->increaseProgress(1);
                // pay
                m_player->substractCredits(priceForTimeUnit);
            }
            continue;
        }

        // check if frame being drawn for building is still showing, if so wait for it
        if (item->getBuildProgressFrame() < 31) {
            continue; // wait
        }

        // DONE building
        itemIsDoneBuildingLogic(item);

        bool isAbleToBuildNewOneImmediately = item->getBuildType() == UNIT;

        if (!isAbleToBuildNewOneImmediately) continue;

        // stop building this item when we are done
        if (item->getTimesToBuild() == 0) {	// no more items to build
            // stop building (set flags)
            item->stopBuilding();

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

void cItemBuilder::itemIsDoneBuildingLogic(cBuildingListItem *item) {
    // remember them
    eBuildType eBuildType = item->getBuildType();
    int buildId = item->getBuildId();

    if (eBuildType == STRUCTURE) {
        // play voice when placeIt is false`
        if (!item->shouldPlaceIt()) {
            if (m_player->isHuman()) {
                game.playVoice(SOUND_VOICE_01_ATR, m_player->getId()); // "Construction Complete"
            }
            item->setPlaceIt(true);

            s_GameEvent event {
                    .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_PLACE_IT,
                    .entityType = item->getBuildType(),
                    .entityID = -1,
                    .player = m_player,
                    .entitySpecificType = item->getBuildId(),
                    .atCell = -1,
                    .isReinforce = false,
                    .buildingListItem = item // mandatory for this event!
            };

            game.onNotifyGameEvent(event);
        }
    } else {
        if (eBuildType == UNIT) {
            m_buildingListUpdater->onBuildItemCompleted(item);
            item->decreaseTimesToBuild(); // decrease amount of times to build

            assert(item->getTimesToBuild() > -1);

            // TODO: Remove duplication, which also exists in AI::think_buildingplacement()
            if (!item->getUnitInfo().airborn) {
                // ground-unit
                deployUnit(item, buildId);
            } else {
                // airborn unit
                int structureToDeployUnit = structureUtils.findHiTechToDeployAirUnit(m_player);
                if (structureToDeployUnit > -1) {
                    cAbstractStructure *pStructureToDeploy = structure[structureToDeployUnit];
                    pStructureToDeploy->setAnimating(true); // animate
                    int unitId = UNIT_CREATE(pStructureToDeploy->getCell(), buildId, m_player->getId(), false);
                    if (unitId > -1) {
                        int rallyPoint = pStructureToDeploy->getRallyPoint();
                        if (rallyPoint > -1) {
                            unit[unitId].move_to(rallyPoint, -1, -1, INTENT_MOVE);
                        }
                    }
                } else {
                    // got destroyed very recently
                }
            }

            // notify game that the item just has been finished
            s_GameEvent newEvent {
                    .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED,
                    .entityType = eBuildType,
                    .entityID = -1,
                    .player = nullptr,
                    .entitySpecificType = buildId,
                    .atCell = -1,
                    .isReinforce = false,
                    .buildingListItem = nullptr
            };

            game.onNotifyGameEvent(newEvent);
        } else if (eBuildType == SPECIAL) {
            m_buildingListUpdater->onBuildItemCompleted(item);
            const s_SpecialInfo &special = item->getSpecialInfo();

            if (special.providesType == UNIT) {
                item->decreaseTimesToBuild(); // decrease amount of times to build
                if (special.deployFrom == AT_STRUCTURE) {
                    // Case: Saboteur, is deployed at structure
                    if (special.providesType == UNIT) {
                        deployUnit(item, special.providesTypeId);
                    }
                    item->stopBuilding();
                    removeItemFromList(item);
                } else if (special.deployFrom == AT_RANDOM_CELL) {
                    // Case: Fremen is deployed at random cell on the map
                    if (special.providesType == UNIT) {
                        // determine cell
                        int iCll = map.getRandomCellWithinMapWithSafeDistanceFromBorder(4);

                        for (int j = 0; j < special.units; j++) {
                            bool passable = map.isCellPassableForFootUnits(iCll);

                            if (passable) {
                                UNIT_CREATE(iCll, special.providesTypeId, FREMEN, false);
                            } else {
                                REINFORCE(FREMEN, special.providesTypeId, iCll, -1);
                            }

                            int x = map.getCellX(iCll);
                            int y = map.getCellY(iCll);
                            int amount = rnd(2) + 1;

                            // randomly shift the cell one coordinate up/down/left/right
                            switch (rnd(4)) {
                                case 0:
                                    x += amount;
                                    break;
                                case 1:
                                    y += amount;
                                    break;
                                case 2:
                                    x -= amount;
                                    break;
                                case 3:
                                    y -= amount;
                                    break;
                            }
                            // change cell
                            cPoint::split(x, y) = map.fixCoordinatesToBeWithinMap(x, y);

                            iCll = map.makeCell(x, y);
                        }
                    }
                    item->stopBuilding();
                    removeItemFromList(item);
                }

                // notify game that the item just has been finished
                s_GameEvent newEvent {
                        .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED,
                        .entityType = eBuildType,
                        .entityID = -1,
                        .player = m_player,
                        .entitySpecificType = buildId,
                        .atCell = -1,
                        .isReinforce = false,
                        .buildingListItem = nullptr
                };

                game.onNotifyGameEvent(newEvent);
            } else if (special.providesType == BULLET) {
                // Case: Deathhand, it is finished, and the player should select a target first.
                // So emit a "special_ready" event first.
                if (special.deployFrom == AT_STRUCTURE) {
                    if (!item->shouldDeployIt()) {
                        item->setDeployIt(true);

                        s_GameEvent event {
                                .eventType = eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET,
                                .entityType = item->getBuildType(),
                                .entityID = -1,
                                .player = m_player,
                                .entitySpecificType = item->getBuildId(),
                                .atCell = -1,
                                .isReinforce = false,
                                .buildingListItem = item // mandatory for this event!
                        };

                        game.onNotifyGameEvent(event);
                    }
                } else {
                    // unsupported case
                    assert(false && "Unsupported special case");
                }

                // We do NOT send a "Finished" event here; see above - we send a special_ready instead
            }
        } else if (eBuildType == UPGRADE) {
            // notify game that the item just has been finished
            s_GameEvent newEvent {
                    .eventType = eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED,
                    .entityType = eBuildType,
                    .entityID = -1,
                    .player = m_player,
                    .entitySpecificType = buildId,
                    .atCell = -1,
                    .isReinforce = false,
                    .buildingListItem = item
            };

            game.onNotifyGameEvent(newEvent);

            // these destroy the data..
            m_buildingListUpdater->onUpgradeCompleted(item);
            removeItemFromList(item);
            //            list->removeItemFromList(item->getSlotId()); // no need to explicitly remove from list, will be done by onUpgradeCompleted
        }
    }
}

/**
 * Creates a unit and deploys it somewhere. Called after building complete (or super weapon FREMEN)
 *
 * @param item
 * @param buildId
 */
void cItemBuilder::deployUnit(cBuildingListItem *item, int buildId) const {
    int structureTypeByItem = structureUtils.findStructureTypeByTypeOfList(item);
    assert(structureTypeByItem > -1);
    int structureToDeployUnit = structureUtils.findStructureToDeployUnit(m_player, structureTypeByItem);
    int buildIdToProduce = buildId;
    if (structureToDeployUnit > -1) {
        cAbstractStructure *pStructureToDeploy = structure[structureToDeployUnit];
        // TODO: Remove duplication, which also exists in AI::think_buildingplacement()
        int cell = pStructureToDeploy->getNonOccupiedCellAroundStructure();
        if (cell > -1) {
            pStructureToDeploy->setAnimating(true); // animate
            int unitId = UNIT_CREATE(cell, buildIdToProduce, m_player->getId(), false);
            if (unitId > -1) {
                int rallyPoint = pStructureToDeploy->getRallyPoint();
                if (rallyPoint > -1) {
                    unit[unitId].move_to(rallyPoint, -1, -1, INTENT_MOVE);
                }
            }
        } else {
            logbook("cItemBuilder: huh? I was promised that this structure would have some place to deploy unit at!?");
        }
    } else {
        structureToDeployUnit = m_player->getPrimaryStructureForStructureType(structureTypeByItem);
        if (structureToDeployUnit < 0) {
            // find any structure of type (regardless if we can deploy or not)
            for (int structureId = 0; structureId < MAX_STRUCTURES; structureId++) {
                cAbstractStructure *pStructure = structure[structureId];
                if (pStructure &&
                    pStructure->isValid() &&
                    pStructure->belongsTo(m_player->getId()) &&
                    pStructure->getType() == structureTypeByItem) {
                    structureToDeployUnit = structureId;
                    break;
                }
            }
        }

        cAbstractStructure * pStructureToDeploy = structure[structureToDeployUnit];
        if (pStructureToDeploy && pStructureToDeploy->isValid()) {
            int cellToDeploy = structure[structureToDeployUnit]->getCell();
            if (pStructureToDeploy->getRallyPoint() > -1) {
                cellToDeploy = pStructureToDeploy->getRallyPoint();
            }
            REINFORCE(m_player->getId(), buildIdToProduce, cellToDeploy, -1, false);
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
		if (m_items[i] == nullptr) {
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
	if (m_buildItemMultiplierEnabled) {
        item->increaseTimesToBuildNTimes(4); // 4 times makes more sense (for structures ~ 4 slabs)
	} else {
        item->increaseTimesToBuild();
    }

	if (!isItemInList(item)) {
		cLogger::getInstance()->log(LOG_TRACE, COMP_SIDEBAR, "Add item to item builder", "item is not in list, adding.");
		// add to list
		m_items[slot] = item;
	} else {
		cLogger::getInstance()->log(LOG_TRACE, COMP_SIDEBAR, "Add item to item builder", "item is in list already. Only times to build is updated.");
	}
}

bool cItemBuilder::isItemInList(cBuildingListItem *item) {
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (m_items[i] == item) {
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
	return nullptr;
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
bool cItemBuilder::isAnythingBeingBuiltForListType(eListType listType, int sublistType) {
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
bool cItemBuilder::isAnythingBeingBuiltForListIdAwaitingPlacement(eListType listType, int sublistType) {
    cBuildingListItem *pItem = getListItemWhichIsAwaitingPlacement(listType, sublistType);
    return pItem != nullptr;
}

/**
 * Returns true if any item awaits deployment for the listType / subListType.
 * If you wish to know which item , use getListItemWhichIsAwaitingDeployment()
 * @param listType
 * @param sublistType
 * @return
 */
bool cItemBuilder::isAnythingBeingBuiltForListIdAwaitingDeployment(eListType listType, int sublistType) {
    cBuildingListItem *pItem = getListItemWhichIsAwaitingDeployment(listType, sublistType);
    return pItem != nullptr;
}

/**
 * Iterates over listType and subList, finds any item that isBuilding() returns true.
 * @param listType
 * @param sublistType
 * @return
 */
cBuildingListItem *cItemBuilder::getListItemWhichIsBuilding(eListType listType, int sublistType) {
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
 * Iterates over listType and subList, returns item that needs to be placed
 * @param listType
 * @param sublistType
 * @return
 */
cBuildingListItem *cItemBuilder::getListItemWhichIsAwaitingPlacement(eListType listType, int sublistType) {
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
 * Iterates over listType and subList, returns item that should be deployed
 * @param listType
 * @param sublistType
 * @return
 */
cBuildingListItem *cItemBuilder::getListItemWhichIsAwaitingDeployment(eListType listType, int sublistType) {
    cBuildingListItem *pItem = nullptr;

    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *listItem = getItem(i);
        if (listItem) {
            cBuildingList *pList = listItem->getList();
            // check if list is available
            if (pList && listType == pList->getType() &&
                sublistType == listItem->getSubList()) {
                if (listItem->shouldDeployIt()) {
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
	return findBuildingListItemOfSameListAs(item) == nullptr;
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
        cBuildingListItem *pItem = m_items[i];
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
	m_items[position] = nullptr;
    m_timers[position] = 0;
}

cBuildingListItem * cItemBuilder::getItem(int position) {
	assert(position > -1);
	assert(position < MAX_ITEMS);
	return m_items[position];
}

void cItemBuilder::startBuilding(cBuildingListItem *item) {
    if (item == nullptr) return;
    item->setIsBuilding(true);
    item->setTimerCap(getTimerCap(item));
    item->resetProgressFrameTimer();
    m_buildingListUpdater->onBuildItemStarted(item);
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
        cBuildingListItem *pItem = m_items[i];
        if (pItem == nullptr) continue;
        if (pItem->getBuildType() != buildType) continue;
        if (pItem->getBuildId() != buildId) continue;

        removeItemFromList(pItem);
    }
}

void cItemBuilder::onNotifyMouseEvent(const s_MouseEvent &) {
    // NOOP
}

void cItemBuilder::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    switch (event.eventType) {
        case eKeyEventType::HOLD:
            onKeyHold(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressed(event);
            break;
        default:
            break;
    }
}

void cItemBuilder::onKeyHold(const cKeyboardEvent &event) {
    if (event.hasEitherKey(KEY_LSHIFT, KEY_RSHIFT)) {
        m_buildItemMultiplierEnabled = true;
    }
}

void cItemBuilder::onKeyPressed(const cKeyboardEvent &event) {
    if (event.hasEitherKey(KEY_LSHIFT, KEY_RSHIFT)) {
        m_buildItemMultiplierEnabled = false;
    }
}

