#include "../d2tmh.h"

cDrawManager::cDrawManager(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer();
	upgradeDrawer = new cUpgradeDrawer();
	orderDrawer = new cOrderDrawer();
	mapDrawer = new cMapDrawer(&map, thePlayer, mapCamera);
}

cDrawManager::~cDrawManager() {
	delete sidebarDrawer;
	delete upgradeDrawer;
	delete orderDrawer;
	delete creditsDrawer;
	delete mapDrawer;
	player = NULL;
}

void cDrawManager::draw() {
	// MAP
	drawMap();

	// GUI
	drawSidebar();
	drawStructurePlacing();
	drawCredits();
	drawUpgradeButton();
	drawOrderButton();

	// STRUCTURES

	// UNITS
}

void cDrawManager::drawMap() {
	assert(mapDrawer);
	map.draw_think();

	mapDrawer->draw();
}

void cDrawManager::drawCredits() {
	assert(creditsDrawer);
	creditsDrawer->draw();
}

void cDrawManager::drawOrderButton() {
	// draw the order button
	if (player->getSideBar()->getSelectedListID() == LIST_STARPORT) {
		orderDrawer->drawOrderButton(player);
	}
}

void cDrawManager::drawSidebar() {
	sidebarDrawer->drawSideBar(player);
}

void cDrawManager::drawUpgradeButton() {
	// draw the upgrade button
	int selectedListId = player->getSideBar()->getSelectedListID();
	if (selectedListId > -1) {
		cBuildingList * selectedList = player->getSideBar()->getList(selectedListId);
		upgradeDrawer->drawUpgradeButtonForSelectedListIfNeeded(player, selectedList);
	}
}

void cDrawManager::drawStructurePlacing() {
	if (game.bPlaceIt) {
		game.draw_placeit();
	}
}
