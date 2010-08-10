#include "../d2tmh.h"

cDrawManager::cDrawManager(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer();
	upgradeDrawer = new cUpgradeDrawer();
	orderDrawer = new cOrderDrawer();
}

cDrawManager::~cDrawManager() {
	delete sidebarDrawer;
	delete upgradeDrawer;
	delete orderDrawer;
	delete creditsDrawer;
	player = NULL;
}

void cDrawManager::draw() {
	drawSidebar();
	drawCredits();
	drawUpgradeButton();
	drawOrderButton();
}

void cDrawManager::drawCredits() {
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
