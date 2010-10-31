/*
 * cSideBar.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "../d2tmh.h"

cSideBar::cSideBar(cPlayer * thePlayer) {
	assert(thePlayer);
	selectedListID = -1; // nothing is selected
	memset(lists, 0, sizeof(lists));
	player = thePlayer;
}

cSideBar::~cSideBar() {
	for (int i = 0; i < LIST_MAX; i++) {
		if (lists[i] != NULL) {
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

void cSideBar::thinkUpgradeButton() {
	cUpgradeUtils upgradeUtils;
	bool isOverUpgradeButton = upgradeUtils.isMouseOverUpgradeButton(mouse_x, mouse_y);

	if (isOverUpgradeButton) {
		int selectedListId = player->getSideBar()->getSelectedListID();

		if (selectedListId > -1) {
			cBuildingList * list = player->getSideBar()->getList(selectedListId);
			assert(list);
			if (list->isBuildingItem() == false) {
				int upgradeLevel = list->getUpgradeLevel();
				int techLevel = player->getTechLevel();

				cListUpgrade * upgrade = upgradeUtils.getListUpgradeForList(player, selectedListId, techLevel, upgradeLevel);
				bool isUpgradeApplicable = upgrade != NULL;

				if (upgrade != NULL) {
					bool isUpgrading = player->getUpgradeBuilder()->isUpgrading(selectedListId);
					char msg[255];
					if (!isUpgrading) {
						sprintf(msg, "$%d | Upgrade", upgrade->getTotalPrice());
					} else {
						cListUpgrade * upgradeInProgress = player->getUpgradeBuilder()->getListUpgrade(selectedListId);
						assert(upgradeInProgress);
						sprintf(msg, "Upgrade completed at %d percent",upgradeInProgress->getProgressAsPercentage());
					}
					drawManager->getMessageDrawer()->setMessage(msg);
				}

				if (isUpgradeApplicable && MOUSE_BTN_LEFT()) {
					assert(upgrade != NULL);
					assert(player->getUpgradeBuilder());
					player->getUpgradeBuilder()->addUpgrade(selectedListId, upgrade);
				}
			}
		}
	}
}

/**
 * Think about the availability of lists.
 */
void cSideBar::thinkAvailabilityLists() {

	// CONSTYARD LIST
	cBuildingList * constyardList = getList(LIST_CONSTYARD);
	assert(constyardList);
	assert(player);

	constyardList->setAvailable(player->iStructures[CONSTYARD] > 0);

	// INFANTRY LIST
	cBuildingList * infantryList = getList(LIST_INFANTRY);

	if (player->getHouse() == ATREIDES) {
		infantryList->setAvailable(player->iStructures[BARRACKS] > 0);
	} else if (player->getHouse() == HARKONNEN) {
		infantryList->setAvailable(player->iStructures[WOR] > 0);
	} else if (player->getHouse() == ORDOS) {
		infantryList->setAvailable(player->iStructures[BARRACKS] > 0 || player->iStructures[WOR] > 0);
	} else if (player->getHouse() == SARDAUKAR) {
		infantryList->setAvailable(player->iStructures[WOR] > 0);
	}

	// LIGHTFC LIST
	cBuildingList * lightfcList = getList(LIST_LIGHTFC);
	lightfcList->setAvailable(player->iStructures[LIGHTFACTORY] > 0);

	// HEAVYFC LIST
	cBuildingList * heavyfcList = getList(LIST_HEAVYFC);
	heavyfcList->setAvailable(player->iStructures[HEAVYFACTORY] > 0);

	// HIGHTECH (ORNI) LIST
	cBuildingList * orniList = getList(LIST_ORNI);
	orniList->setAvailable(player->iStructures[HIGHTECH] > 0);

	// PALACE LIST
	cBuildingList * palaceList = getList(LIST_PALACE);
	palaceList->setAvailable(player->iStructures[PALACE] > 0);

	// STARPORT LIST
	cBuildingList * starportList = getList(LIST_STARPORT);
	starportList->setAvailable(player->iStructures[STARPORT] > 0);

	// when available, check if we accept orders
	if (starportList->isAvailable()) {
		cOrderProcesser * orderProcesser = player->getOrderProcesser();
		bool acceptsOrders = orderProcesser->acceptsOrders();
		starportList->setAcceptsOrders(acceptsOrders);
	}
}

/**
 * Think about interaction (fps based)
 *
 */
void cSideBar::thinkInteraction() {
	thinkUpgradeButton();

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
	            play_sound_id(SOUND_BUTTON,-1); // click sound
				break;
			}
		}
	}

	// scroll buttons interaction
	thinkMouseZScrolling();

	// when mouse pressed, a list is selected, and that list is still available
	if (selectedListID > -1 && getList(selectedListID)->isAvailable() && cMouse::getInstance()->isLeftButtonClicked()) {
		cBuildingList *list = getList(selectedListID);

		bool mouseOverUp = mouseOverScrollUp();
		bool mouseOverDown = mouseOverScrollDown();
		assert(!(mouseOverUp == true && mouseOverDown == true));// can never be both.

		if (mouseOverUp) {
			list->scrollUp();
			// draw pressed
			draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_UP_PRESSED].dat, 571, 315);
		} else if (mouseOverDown) {
			list->scrollDown();
			// draw pressed
			draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_DOWN_PRESSED].dat, 623, 315);
		}
	}

	// when mouse pressed, build item if over item
	if (selectedListID > -1 && getList(selectedListID)->isAvailable() && getList(selectedListID)->isUpgrading() == false) {
		cBuildingList *list = getList(selectedListID);

		cBuildingListDrawer drawer;
		cBuildingListItem *item = drawer.isOverItemCoordinates(list, mouse_x,  mouse_y);

		cOrderProcesser * orderProcesser = player->getOrderProcesser();

		// mouse is over item
		if (item != NULL) {
			char msg[255];
			if (list->isAcceptsOrders()) {
				if (list->getType() == LIST_CONSTYARD) {
					sprintf(msg, "$%d | %s", item->getBuildCost(), structures[item->getBuildId()].name);
				} else {
					if (item->getBuildCost() > 0) {
						sprintf(msg, "$%d | %s", item->getBuildCost(), units[item->getBuildId()].name);
					} else {
						sprintf(msg, "%s", units[item->getBuildId()].name);
					}
				}
				drawManager->getMessageDrawer()->setMessage(msg);
			}
		}

		if (cMouse::getInstance()->isLeftButtonClicked()) {
			if (list != NULL) {
				if (list->getType() != LIST_STARPORT) {
					if (item != NULL) {
						if (item->shouldPlaceIt() == false) {
							cItemBuilder *itemBuilder = player->getItemBuilder();
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
						if (player->credits >= item->getBuildCost()) {
							item->increaseTimesOrdered();
							orderProcesser->addOrder(item);
							player->credits -= item->getBuildCost();
						}
					}
				}
			}
		}

		if (cMouse::getInstance()->isRightButtonClicked()) {
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
								cItemBuilder *itemBuilder = player->getItemBuilder();
								assert(itemBuilder);
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

bool cSideBar::mouseOverScrollUp() {
	if ((mouse_x >= 571 && mouse_y >= 315) && (mouse_x < 584 && mouse_y < 332)) {
		return true;
	}
	return false;
}

bool cSideBar::mouseOverScrollDown() {
	if ((mouse_x >= 623 && mouse_y >= 315) && (mouse_x < 636 && mouse_y < 332))	{
		return true;
	}
	return false;
}

void cSideBar::thinkMouseZScrolling() {

	if (selectedListID < 0) {
		return;
	}

	if (!getList(selectedListID)->isAvailable()) {
		return;
	}

	cBuildingList *list = getList(selectedListID);
	cMouse *mouse = cMouse::getInstance();

	// MOUSE WHEEL
	if (mouse->isMouseScrolledUp()) {

	   list->scrollUp();

		// draw pressed
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_UP_PRESSED].dat, 571, 315);

	}

	if (mouse->isMouseScrolledDown()) {
		// Only allow scrolling when there is an icon to show
	   list->scrollDown();

		// draw pressed
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_DOWN_PRESSED].dat, 623, 315);
	}
}
