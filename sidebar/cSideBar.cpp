/*
 * cSideBar.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cSideBar::cSideBar(cPlayer * thePlayer) {
	assert(thePlayer);
	selectedListID = -1; // nothing is selected
	memset(lists, NULL, sizeof(lists));
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
 * Thinking for sidebar
 */
void cSideBar::think() {
	thinkAvailabilityLists();
	thinkUpgradeButton();
}

void cSideBar::thinkUpgradeButton() {
	cUpgradeUtils upgradeUtils;
	bool isOverUpgradeButton = upgradeUtils.isMouseOverUpgradeButton(mouse_x, mouse_y);

	if (isOverUpgradeButton ) {
		int selectedListId = player->getSideBar()->getSelectedListID();

		if (selectedListId > -1) {
			cBuildingList * list = player->getSideBar()->getList(selectedListId);
			assert(list);
			int upgradeLevel = list->getUpgradeLevel();
			int techLevel = player->getTechLevel();

			bool isUpgradeApplicable = upgradeUtils.isUpgradeApplicableForPlayerAndList(player, selectedListId, techLevel, upgradeLevel );

			if (isUpgradeApplicable && MOUSE_BTN_LEFT()) {
				// upgrade
			}
		}
	}
}

/**
 * Think about the availability of lists.
 */
void cSideBar::thinkAvailabilityLists() {

	// CONSTYARD LIST
	cBuildingList * constyardList = player->getSideBar()->getList(LIST_CONSTYARD);

	constyardList->setAvailable(player->iStructures[CONSTYARD] > 0);

	// INFANTRY LIST
	cBuildingList * infantryList = player->getSideBar()->getList(LIST_INFANTRY);

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
	cBuildingList * lightfcList = player->getSideBar()->getList(LIST_LIGHTFC);
	lightfcList->setAvailable(player->iStructures[LIGHTFACTORY] > 0);

	// HEAVYFC LIST
	cBuildingList * heavyfcList = player->getSideBar()->getList(LIST_HEAVYFC);
	heavyfcList->setAvailable(player->iStructures[HEAVYFACTORY] > 0);

	// HIGHTECH (ORNI) LIST
	cBuildingList * orniList = player->getSideBar()->getList(LIST_ORNI);
	orniList->setAvailable(player->iStructures[HIGHTECH] > 0);

	// PALACE LIST
	cBuildingList * palaceList = player->getSideBar()->getList(LIST_PALACE);
	palaceList->setAvailable(player->iStructures[PALACE] > 0);

	// STARPORT LIST
	cBuildingList * starportList = player->getSideBar()->getList(LIST_STARPORT);
	starportList->setAvailable(player->iStructures[STARPORT] > 0);
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
	            play_sound_id(SOUND_BUTTON,-1); // click sound
				break;
			}
		}
	}

	// scroll buttons interaction
	thinkMouseZScrolling();

	// when mouse pressed, a list is selected, and that list is still available
	if (selectedListID > -1 && getList(selectedListID)->isAvailable() && game.bMousePressedLeft) {
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
	if (selectedListID > -1 && getList(selectedListID)->isAvailable()) {
		cBuildingList *list = getList(selectedListID);
		if (game.bMousePressedLeft) {
			if (list != NULL) {
				cBuildingListDrawer *drawer = new cBuildingListDrawer();
				cBuildingListItem *item = drawer->isOverItemCoordinates(list, mouse_x,  mouse_y);

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

				delete drawer;
			}
		}

		if (game.bMousePressedRight) {
			if (list != NULL) {
				cBuildingListDrawer *drawer = new cBuildingListDrawer();
				cBuildingListItem *item = drawer->isOverItemCoordinates(list, mouse_x,  mouse_y);

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

				delete drawer;
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

	// MOUSE WHEEL
	if (mouse_z > game.iMouseZ) {

	   list->scrollUp();

		// draw pressed
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_UP_PRESSED].dat, 571, 315);

		game.iMouseZ=mouse_z;
	}

	if (mouse_z < game.iMouseZ) {
		// Only allow scrolling when there is an icon to show
	   list->scrollDown();

		// draw pressed
		draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_DOWN_PRESSED].dat, 623, 315);

		game.iMouseZ=mouse_z;
	}
}
