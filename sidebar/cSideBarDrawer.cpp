/*
 * cSideBarDrawer.cpp
 *
 *  Created on: Aug 2, 2009
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cSideBarDrawer::cSideBarDrawer() {
	upgradeDrawer = new cUpgradeDrawer();
	buildingListDrawer = new cBuildingListDrawer();
}

cSideBarDrawer::~cSideBarDrawer() {
	delete upgradeDrawer;
	delete buildingListDrawer;
}

// draws the sidebar on screen
void cSideBarDrawer::drawSideBar(cPlayer * player) {
	// draw the sidebar itself (the backgrounds, borders, etc)
	// TODO: draw sidebar backgrounds here
	cSideBar *sidebar = player->getSideBar();

	// draw the buildlist icons
	int selectedListId = sidebar->getSelectedListID();

	for (int listId = LIST_CONSTYARD; listId < LIST_MAX; listId++) {
		cBuildingList *list = sidebar->getList(listId);
		bool isListIdSelectedList = (selectedListId == listId);
		buildingListDrawer->drawButton(list, isListIdSelectedList);
	}

	// draw the buildlist itself (take scrolling into account)
	cBuildingList *selectedList = NULL;

	if (selectedListId > -1) {
		selectedList = sidebar->getList(selectedListId);
		buildingListDrawer->drawList(selectedList, selectedListId, selectedList->getScrollingOffset());
	}

	// draw the minimap
	// TODO: draw minimap here

	// draw the upgrade button
	if (selectedListId) {
		upgradeDrawer->drawUpgradeButtonForSelectedListIfNeeded(player, selectedList);
	}

	// draw the capacities (max spice/max power)
	// TODO: draw capacities here
}
