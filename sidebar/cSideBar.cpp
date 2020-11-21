/*
 * cSideBar.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cSideBar::cSideBar(cPlayer & thePlayer) : m_Player(thePlayer) {
	assert(&thePlayer);
	selectedListID = -1; // nothing is selected
	memset(lists, 0, sizeof(lists));
}

cSideBar::~cSideBar() {
	for (int i = 0; i < LIST_MAX; i++) {
		if (lists[i]) {
		    lists[i]->removeAllItems();
			delete lists[i]; // delete list
		}
	}
}

void cSideBar::setList(int listId, cBuildingList* list) {
	assert(listId > -1);
	assert(listId < LIST_MAX);
	if (lists[listId]) {
		logbook("WARNING: Setting list, while list already set. Deleting old entry before assigning new.");
		delete lists[listId];
	}

	lists[listId] = list;
}

/**
 * Thinking for sidebar - timer based
 */
void cSideBar::think() {
	thinkAvailabilityLists();
}

/**
 * Think about the availability of lists.
 */
void cSideBar::thinkAvailabilityLists() {
	// CONSTYARD LIST
	cBuildingList * constyardList = getList(LIST_CONSTYARD);
	assert(constyardList);

	constyardList->setAvailable(m_Player.hasAtleastOneStructure(CONSTYARD));

	// INFANTRY LIST
	cBuildingList * infantryList = getList(LIST_FOOT_UNITS);

    infantryList->setAvailable(m_Player.hasBarracks() || m_Player.hasWor());

	// LIGHTFC LIST
	cBuildingList * listUnits = getList(LIST_UNITS);
    listUnits->setAvailable(m_Player.hasAtleastOneStructure(LIGHTFACTORY) ||
                            m_Player.hasAtleastOneStructure(HEAVYFACTORY) ||
                            m_Player.hasAtleastOneStructure(HIGHTECH)
    );

	// PALACE LIST
	cBuildingList * palaceList = getList(LIST_PALACE);
	palaceList->setAvailable(m_Player.hasAtleastOneStructure(PALACE));

	// STARPORT LIST
	cBuildingList * starportList = getList(LIST_STARPORT);
	starportList->setAvailable(m_Player.hasAtleastOneStructure(STARPORT));

	// when available, check if we accept orders
	if (starportList->isAvailable()) {
		cOrderProcesser * orderProcesser = m_Player.getOrderProcesser();
		bool acceptsOrders = orderProcesser->acceptsOrders();
		starportList->setAcceptsOrders(acceptsOrders);
	}

    // UPGRADES LIST (for now is always available - later should be depending on items in list)
    cBuildingList * upgradesList = getList(LIST_UPGRADES);
    upgradesList->setAvailable(true);
}

/**
 * Think about interaction (fps based)
 *
 */
void cSideBar::thinkInteraction() {
    cBuildingListDrawer * buildingListDrawer = drawManager->getBuildingListDrawer();

    // button interaction
	for (int i = LIST_CONSTYARD; i < LIST_MAX; i++) {
		if (i == selectedListID) continue; // skip selected list for button interaction
		cBuildingList *list = getList(i);

		if (list->isAvailable() == false) continue; // not available, so no interaction possible

		// interaction is possible.
		if (list->isOverButton(mouse_x, mouse_y)) {
			if (MOUSE_BTN_LEFT()) {
				// clicked on it. Set focus on this one
				selectedListID = i;
                play_sound_id(SOUND_BUTTON); // click sound
				break;
			}
		}
	}

	if (selectedListID < 0) return;
	if (!getList(selectedListID)->isAvailable()) return;

	// when mouse pressed, build item if over item
    cBuildingList *list = getList(selectedListID);

    cOrderProcesser * orderProcesser = m_Player.getOrderProcesser();
    cOrderDrawer * orderDrawer = drawManager->getOrderDrawer();

    // allow clicking on the order button
    if (list->getType() == LIST_STARPORT) {
        if (cMouse::isLeftButtonClicked() && orderDrawer->isMouseOverOrderButton()) {
            assert(orderProcesser);

            // handle "order" button interaction
            if (orderProcesser->canPlaceOrder()) {
                orderProcesser->placeOrder();
            }
        }
    }

    cBuildingListItem *item = buildingListDrawer->isOverItemCoordinates(list, mouse_x, mouse_y);
    if (item == nullptr) return;

    // mouse is over item - draw "messagebar"
    char msg[255];
    if (list->isAcceptsOrders()) {
        // build time is in global time units , using a timer cap of 35 * 5 miliseconds = 175 miliseconds
        int buildTimeInMs = item->getTotalBuildTime() * 175;
        // now we have in miliseconds, we know the amount of seconds too.
        int seconds = buildTimeInMs / 1000;

        if (item->getBuildType() == STRUCTURE) {
            s_Structures structureType = structures[item->getBuildId()];
            sprintf(msg, "$%d | %s | %d Power | %d Secs", item->getBuildCost(), structureType.name, (structureType.power_give - structureType.power_drain), seconds);
        } else if (item->getBuildType() == UNIT) {
            s_UnitP unitType = units[item->getBuildId()];
            if (item->getBuildCost() > 0) {
                sprintf(msg, "$%d | %s | %d Secs", item->getBuildCost(), unitType.name, seconds);
            } else {
                sprintf(msg, "%s", units[item->getBuildId()].name);
            }
        } else if (item->getBuildType() == UPGRADE){
            s_Upgrade upgrade = upgrades[item->getBuildId()];
            sprintf(msg, "UPGRADE: $%d | %s | %d Secs", item->getBuildCost(), upgrade.description, seconds);
        } else {
            sprintf(msg, "UNKNOWN BUILD TYPE");
        }

        drawManager->getMessageDrawer()->setMessage(msg);
    }

    if (cMouse::isLeftButtonClicked()) {
        if (list->getType() != LIST_STARPORT) {

            // icon is in "Place it" mode, meaning if clicked the "place the thing" state should be set
            if (item->shouldPlaceIt()) {
                game.bPlaceIt = true;
            } else {
                // Item should not be placed, so it can be built
                cItemBuilder *itemBuilder = m_Player.getItemBuilder();
                bool firstOfItsListType = itemBuilder->isBuildListItemTheFirstOfItsListType(item);

                if (item->isQueuable()) {
                    itemBuilder->addItemToList(item);
                } else if (firstOfItsListType) { // may only build if there is nothing else in the list type being built
                    itemBuilder->addItemToList(item);
                }
                list->setLastClickedId(item->getSlotId());
            }
        } else {
            // add orders
            if (orderProcesser->acceptsOrders()) {
                if (m_Player.credits >= item->getBuildCost()) {
                    item->increaseTimesOrdered();
                    orderProcesser->addOrder(item);
                    m_Player.substractCredits(item->getBuildCost());
                }
            }
        }
    }

    if (cMouse::isRightButtonClicked()) {
        // anything but the starport can 'build' things
        if (list->getType() != LIST_STARPORT) {
            if (item->getTimesToBuild() > 0) {
                item->decreaseTimesToBuild();
                item->setPlaceIt(false);

                if (item->getTimesToBuild() == 0) {
                    cLogger::getInstance()->log(LOG_INFO, COMP_SIDEBAR, "Cancel construction", "Item is last item in queue, will give money back.");
                    // only give money back for item that is being built
                    if (item->isBuilding()) {
                        // calculate the amount of money back:
                        player[HUMAN].credits += item->getRefundAmount();
                    }
                    item->setIsBuilding(false);
                    item->setProgress(0);
                    cItemBuilder *itemBuilder = m_Player.getItemBuilder();
                    itemBuilder->removeItemFromList(item);
                }
                // else, only the number is decreased (used for queueing)
            }
        } else {
            assert(orderProcesser);
            if (orderProcesser->isOrderPlaced() == false) {
                if (item->getTimesOrdered() > 0) {
                    item->decreaseTimesOrdered();
                    orderProcesser->removeOrder(item);
                }
            }
        }

        game.bPlaceIt = false;
    }
}