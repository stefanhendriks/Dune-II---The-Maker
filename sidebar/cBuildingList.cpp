#include <vector>
#include "../include/d2tmh.h"
#include "cBuildingList.h"


cBuildingList::cBuildingList(eListType listType) {
	TIMER_progress = 0;
	lastClickedId = 0;
	buttonIconIdPressed = 0;	// the button to draw at the left of the list
	buttonDrawX = 0;
	buttonDrawY = 0;
	available = false;		 // is this list available?
	memset(items, 0, sizeof(items));
	typeOfList = listType;
	maxItems = 0;
	acceptsOrders = true; // at default true, will be set to FALSE/TRUE by starport logic for starport list only
    m_itemBuilder = nullptr;
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
    m_itemBuilder = nullptr;
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
        cBuildingListItem * item = getItem(i);
        if (item == nullptr) continue;
        delete item;
        items[i] = nullptr;
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
    cBuildingListItem *item = new cBuildingListItem(upgradeType, upgrades[upgradeType],
                                                    upgrades[upgradeType].providesTypeSubList);
    if (!addItemToList(item)) {
        delete item;
    }
}

void cBuildingList::addStructureToList(int structureType, int subList) {
    cBuildingListItem *item = new cBuildingListItem(structureType, structures[structureType], subList);
    if (!addItemToList(item)) {
        delete item;
    }
}

void cBuildingList::addUnitToList(int unitType, int subList) {
    cBuildingListItem *item = new cBuildingListItem(unitType, units[unitType], subList);
    if (!addItemToList(item)) {
        delete item;
    }
}

void cBuildingList::addSpecialToList(int specialType, int subList) {
    cBuildingListItem *item = new cBuildingListItem(specialType, specials[specialType], subList);
    if (!addItemToList(item)) {
        delete item;
    }
}

bool cBuildingList::addItemToList(cBuildingListItem * item) {
	if (isItemInList(item)) {
//		logbook("Will not add, item is already in list.");
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

	if (m_itemBuilder) {
	    // first remove this, before deleting it!
        m_itemBuilder->removeItemFromList(item);
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
    return game.getMouse()->isOverRectangle(getButtonDrawX(), getButtonDrawY(), 33, 27);
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


// TODO: This should be set, not looked up. Ie, the item to place should be set somewhere (instead of setting
// placeIt = true, so that we *know* which item to place, and not have to retrospectively decide which item we where
// placing - this makes no sense. Especially when we are going to deploy stuff later as well... (and we assume
// that placement is ALWAYS from CONSTYARD; deployment from PALACE. Which may work, but is not flexible at all.
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

// TODO: This should be set, not looked up. Ie, the item to place should be set somewhere (instead of setting
// deployIt = true, so that we *know* which item to place, and not have to retrospectively decide which item we where
// deploying - this makes no sense.
cBuildingListItem * cBuildingList::getItemToDeploy() {
	for (int i = 0 ; i < MAX_ITEMS; i++) {
		cBuildingListItem *item = getItem(i);
		// valid pointer
		if (item) {
			// get isBuilding
			if (item->shouldDeployIt()) {
				return item;
			}
		}
	}
	return NULL;
}

void cBuildingList::setStatusPendingUpgrade(int subListId) {
    for (int i = 0 ; i < MAX_ITEMS; i++) {
        cBuildingListItem *item = getItem(i);
        // valid pointer
        if (item) {
            if (item->isTypeUpgrade()) {
                // match the "provides" sublist id here!
                if (item->getS_Upgrade().providesTypeSubList == subListId) {
                    item->setStatusPendingUpgrade();
                }
            } else {
                if (item->getSubList() == subListId) {
                    item->setStatusPendingUpgrade();
                }
            }
        }
    }
}

void cBuildingList::setStatusAvailable(int subListId) {
    for (int i = 0 ; i < MAX_ITEMS; i++) {
        cBuildingListItem *item = getItem(i);
        // valid pointer
        if (item) {
            if (item->isTypeUpgrade()) {
                // match the "provides" sublist id here!
                if (item->getS_Upgrade().providesTypeSubList == subListId) {
                    item->setStatusAvailable();
                }
            } else {
                if (item->getSubList() == subListId) {
                    item->setStatusAvailable();
                }
            }
        }
    }
}

void cBuildingList::setStatusPendingBuilding(int subListId) {
    for (int i = 0 ; i < MAX_ITEMS; i++) {
        cBuildingListItem *item = getItem(i);
        // valid pointer
        if (item) {
            if (item->isTypeUpgrade()) {
                // match the "provides" sublist id here!
                if (item->getS_Upgrade().providesTypeSubList == subListId) {
                    item->setStatusPendingBuilding();
                }
            } else {
                if (item->getSubList() == subListId) {
                    item->setStatusPendingBuilding();
                }
            }
        }
    }
}

void cBuildingList::removeAllSublistItems(int sublistId) {
    std::vector<int> buildIdsToRemove;
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = getItem(i);
        if (pItem == nullptr) continue;
        if (pItem->getSubList() == sublistId) {
            buildIdsToRemove.push_back(pItem->getBuildId());
        }
    }

    for(int buildId : buildIdsToRemove) {
        removeItemFromListByBuildId(buildId);
    }
}

void cBuildingList::resetTimesOrderedForAllItems() {
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = getItem(i);
        if (pItem == nullptr) continue;
        pItem->resetTimesOrdered();
    }
}

cBuildingListItem *cBuildingList::getFirstItemInSubList(int sublistId) {
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = getItem(i);
        if (pItem == nullptr) continue;
        if (pItem->getSubList() != sublistId) continue;
        return pItem;
    }
    return nullptr;
}
