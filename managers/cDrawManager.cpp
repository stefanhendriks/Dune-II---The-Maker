#include "../d2tmh.h"

cDrawManager::cDrawManager(cPlayer * thePlayer) {
	assert(thePlayer);
	player = thePlayer;
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer();
	upgradeDrawer = new cUpgradeDrawer();
	orderDrawer = new cOrderDrawer();
	mapDrawer = new cMapDrawer(&map, thePlayer, mapCamera);
	miniMapDrawer = new cMiniMapDrawer(&map, thePlayer, mapCamera);
	particleDrawer = new cParticleDrawer();
	messageDrawer = new cMessageDrawer();
	placeitDrawer = new cPlaceItDrawer();
}

cDrawManager::~cDrawManager() {
	delete sidebarDrawer;
	delete upgradeDrawer;
	delete orderDrawer;
	delete creditsDrawer;
	delete mapDrawer;
	delete miniMapDrawer;
	delete particleDrawer;
	delete messageDrawer;
	delete placeitDrawer;
	player = NULL;
}

void cDrawManager::draw() {
	// MAP
	assert(mapDrawer);
	map.draw_think();
	mapDrawer->drawTerrain();

	// Only draw units/structures, etc, when we do NOT press D
	if (!key[KEY_D] || !key[KEY_TAB])
	{
		map.draw_structures(0);
	}

	// draw layer 1 (beneath units, on top of terrain
	particleDrawer->drawLowerLayer();

	map.draw_units();

	map.draw_bullets();

	map.draw_structures(2); // draw layer 2
	map.draw_structures_health();
	map.draw_units_2nd();

	particleDrawer->drawHigherLayer();

	map.draw_minimap();

	mapDrawer->drawShroud();
	miniMapDrawer->draw();

	// GUI
	drawSidebar();
	drawStructurePlacing();
	drawCredits();
	drawUpgradeButton();
	drawOrderButton();

	// THE MESSAGE
	drawMessage();
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
		// TODO: fix the placeItDrawer, it crashes the game now!
		cBuildingListItem *itemToPlace = player->getSideBar()->getList(LIST_CONSTYARD)->getItemToPlace();
		if (itemToPlace) {
			assert(placeitDrawer);
			placeitDrawer->draw(itemToPlace);
		}
	}
}

void cDrawManager::drawMessage() {
	assert(messageDrawer);
	messageDrawer->draw();
}
