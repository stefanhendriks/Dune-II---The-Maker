/*
 * cSideBar.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cSideBar::cSideBar() {
	selectedListID = -1; // nothing is selected
	memset(lists, NULL, sizeof(lists));
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
	// think about building
}


/**
 * Think about the availability of lists.
 */
void cSideBar::thinkAvailabilityLists() {
	if (player[HUMAN].iStructures[CONSTYARD] > 0) {
		getList(LIST_CONSTYARD)->setAvailable(true);
	} else {
		getList(LIST_CONSTYARD)->setAvailable(false);
	}

	getList(LIST_INFANTRY)->setAvailable(true);
	getList(LIST_LIGHTFC)->setAvailable(true);
	getList(LIST_HEAVYFC)->setAvailable(true);
}

/**
 * Think about interaction (fps based)
 *
 */
void cSideBar::thinkInteraction() {

	// button interaction
	for (int i = LIST_CONSTYARD; i < LIST_MAX; i++) {
		cBuildingList *list = getList(i);
		if (i == selectedListID) continue; // skip selected list for button interaction

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
					cItemBuilder *builder = cItemBuilder::getInstance();
					builder->addItemToList(item);
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

						// give money back (and stop building)
						if (item->getTimesToBuild() == 0) {
//							cLogger::getInstance()->logCommentLine("times to build is 0 , give money back and stop building.");
							player[HUMAN].credits += item->getBuildCost();
							item->setIsBuilding(false);
							item->setProgress(0);
							cItemBuilder::getInstance()->removeItemFromList(item);
						} else {
							cLogger::getInstance()->logCommentLine("times to build is NOT 0 , Doing nothing.");
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

	if (selectedListID < 0) return;
	if (!getList(selectedListID)->isAvailable()) return;

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
