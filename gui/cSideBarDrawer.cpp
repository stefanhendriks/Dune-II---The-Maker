/*
 * cSideBarDrawer.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cSideBarDrawer::cSideBarDrawer() {
}

cSideBarDrawer::~cSideBarDrawer() {
}

// draws the sidebar on screen
void cSideBarDrawer::drawSideBar(cSideBar *sidebar) {
	// draw the sidebar itself (the backgrounds, borders, etc)
	// TODO: draw sidebar backgrounds here

	// draw the buildlist icons
	cBuildingListDrawer buildingListDrawer;
	int selectedList = sidebar->getSelectedListID();

	for (int listId = LIST_CONSTYARD; listId < LIST_MAX; listId++) {
		cBuildingList *list = sidebar->getList(listId);
		bool isListIdSelectedList = (selectedList == listId);
		buildingListDrawer.drawButton(list, isListIdSelectedList);
	}

	// draw the buildlist itself (take scrolling into account)
	if (selectedList > -1) {
		cBuildingList *list = sidebar->getList(selectedList);
		buildingListDrawer.drawList(list, selectedList, list->getScrollingOffset());
	}

	// draw the minimap
	// TODO: draw minimap here

	// draw the capacities (max spice/max power)
	// TODO: draw capacities here
}
