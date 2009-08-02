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

	// think about interaction
	thinkInteraction();
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
 * Think about interaction
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
}
