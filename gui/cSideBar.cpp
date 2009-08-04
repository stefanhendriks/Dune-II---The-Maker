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
			delete lists[i]; // delete
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
	if (selectedListID > -1 && getList(selectedListID)->isAvailable() && game.bMousePressedLeft) { // TODO: replace game.bMousePressedLeft with some more mouse handling class as this does not work as expected :(
		cBuildingList *list = getList(selectedListID);
		logbook("Mouse button pressed; evaluating scroll buttons.");

		bool mouseOverUp = mouseOverScrollUp();
		bool mouseOverDown = mouseOverScrollDown();
		assert(!(mouseOverUp == true && mouseOverDown == true));// can never be both.

		if (mouseOverUp) {
			logbook("Mouse was over scroll up button.");
			list->scrollUp();
			// draw pressed
			draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_UP_PRESSED].dat, 571, 315);
		} else if (mouseOverDown) {
			logbook("Mouse was over scroll down button.");
			list->scrollDown();
			// draw pressed
			draw_sprite(bmp_screen, (BITMAP *)gfxinter[BTN_DOWN_PRESSED].dat, 623, 315);
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
