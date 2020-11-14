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

	// when mouse pressed, build item if over item
	if (selectedListID > -1 && getList(selectedListID)->isAvailable() && getList(selectedListID)->isUpgrading() == false) {
		cBuildingList *list = getList(selectedListID);

		cBuildingListDrawer drawer;
		cBuildingListItem *item = drawer.isOverItemCoordinates(list, mouse_x,  mouse_y);

		cOrderProcesser * orderProcesser = m_Player.getOrderProcesser();

		// mouse is over item
		if (item != NULL) {
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
		}

		if (cMouse::isLeftButtonClicked()) {
			if (list != NULL) {
				if (list->getType() != LIST_STARPORT) {
					if (item != NULL) {
						if (item->shouldPlaceIt() == false) {
							cItemBuilder *itemBuilder = m_Player.getItemBuilder();
							assert(itemBuilder);
							itemBuilder->addItemToList(item);
							list->setLastClickedId(item->getSlotId());
						} else {
							game.bPlaceIt = true;
						}
					}
				} else {
					// list is starport
					assert(orderProcesser);

					// handle order button interaction
					if (orderProcesser->hasOrderedAnything() &&
						orderProcesser->isOrderPlaced() == false) {
						cOrderDrawer orderDrawer;
						if (orderDrawer.isMouseOverOrderButton(mouse_x, mouse_y)) {
							orderProcesser->placeOrder();
						}
					}

					if (item != NULL && orderProcesser->acceptsOrders()) {
						if (m_Player.credits >= item->getBuildCost()) {
							item->increaseTimesOrdered();
							orderProcesser->addOrder(item);
                            m_Player.substractCredits(item->getBuildCost());
						}
					}
				}
			}
		}

		if (cMouse::isRightButtonClicked()) {
			if (list != NULL) {
				// anything but the starport can 'build' things
				if (list->getType() != LIST_STARPORT) {
					if (item != NULL) {
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
					}
				} else {
					assert(orderProcesser);
					if (item != NULL && orderProcesser->isOrderPlaced() == false) {
						if (item->getTimesOrdered() > 0) {
							item->decreaseTimesOrdered();
							orderProcesser->removeOrder(item);
						}
					}
				}

				if (item == NULL) {
					game.bPlaceIt = false;
				}
			}
		}

	}

}