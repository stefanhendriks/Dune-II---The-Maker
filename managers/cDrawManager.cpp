#include "../include/d2tmh.h"

cDrawManager::cDrawManager(const cPlayer & thePlayer) : m_Player(thePlayer) {
	assert(&thePlayer);
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer();
	upgradeDrawer = new cUpgradeDrawer();
	orderDrawer = new cOrderDrawer();
	mapDrawer = new cMapDrawer(&map, thePlayer, mapCamera);
	miniMapDrawer = new cMiniMapDrawer(&map, thePlayer, mapCamera);
	particleDrawer = new cParticleDrawer();
	messageDrawer = new cMessageDrawer();
	messageBarDrawer = new cMessageBarDrawer();
	placeitDrawer = new cPlaceItDrawer();
	structureDrawer = new cStructureDrawer();
	mouseDrawer = new cMouseDrawer(thePlayer, cMouse::getInstance());

	cMessageBar * messageBar = messageBarDrawer->getMessageBar();
	messageBar->setX(200);
	messageBar->setY(200);
	messageBar->setWidth(game.screen_x - 160);
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
	delete structureDrawer;
	delete mouseDrawer;
}

void cDrawManager::draw() {
	// MAP
	assert(mapDrawer);
	map.draw_think();
	mapDrawer->drawTerrain();

	// Only draw units/structures, etc, when we do NOT press D
	// TODO: this should be something like : if (keyboard->isDebuggingStructures())
	if (!key[KEY_D] || !key[KEY_TAB])
	{
		structureDrawer->drawStructuresFirstLayer();
	}

	// draw layer 1 (beneath units, on top of terrain)
	particleDrawer->drawLowerLayer();

	map.draw_units();

	map.draw_bullets();

	structureDrawer->drawStructuresSecondLayer();
	structureDrawer->drawStructuresHealthBars();

	map.draw_units_2nd();

	particleDrawer->drawHigherLayer();
	mapDrawer->drawShroud();

	drawRallyPoint();

	// GUI
	drawSidebar();
	miniMapDrawer->draw();

	drawStructurePlacing();
	drawCredits();
	drawUpgradeButton();
	drawOrderButton();

	// THE MESSAGE
	drawMessage();

	// DO COMBAT MOUSE (TODO: remove this eventually, as it updates state and that is not what
	// this class should be doing)
	game.combat_mouse();

	// MOUSE
	drawMouse();
}

void cDrawManager::drawCredits() {
	assert(creditsDrawer);
	creditsDrawer->draw();
}

void cDrawManager::drawRallyPoint() {
	if (game.selected_structure > -1) {
		cAbstractStructure * theStructure = structure[game.selected_structure];
		int rallyPointCell = theStructure->getRallyPoint();

		// show draw a target on this cell so we know this is the rally point.
		if (rallyPointCell > -1) {
			// draw this thing ...
			set_trans_blender(0,0,0,128);
			draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_MOVE].dat, getDrawXForCell(rallyPointCell), getDrawYForCell(rallyPointCell));

			int startX = theStructure->iDrawX() + (theStructure->getS_StructuresType().bmp_width / 2);
			int startY = theStructure->iDrawY() + (theStructure->getS_StructuresType().bmp_height / 2);

			int endX = getDrawXForCell(rallyPointCell) + 16;
			int endY = getDrawYForCell(rallyPointCell) + 16;

			line(bmp_screen, startX, startY, endX, endY, player[HUMAN].getMinimapColor());
		}
	}
}

int cDrawManager::getDrawXForCell(int cell) {
	int cellX = iCellGiveX(cell);
	return (cellX * 32) - (mapCamera->getX() * 32);
}

int cDrawManager::getDrawYForCell(int cell) {
	int cellY = iCellGiveY(cell);
	return (cellY * 32) - (mapCamera->getY() * 32) +42; // + 42 is the top bar (options/upgrade/credits)
}

void cDrawManager::drawOrderButton() {
	// draw the order button
	if (m_Player.getSideBar()->getSelectedListID() == LIST_STARPORT) {
		orderDrawer->drawOrderButton(m_Player);
	}
}

void cDrawManager::drawSidebar() {
	sidebarDrawer->drawSideBar(m_Player);
}

void cDrawManager::drawUpgradeButton() {
	// draw the upgrade button
	int selectedListId = m_Player.getSideBar()->getSelectedListID();
	if (selectedListId > -1) {
		upgradeDrawer->drawUpgradeButtonForSelectedListIfNeeded(m_Player);
	}
}

void cDrawManager::drawStructurePlacing() {
	if (game.bPlaceIt) {
		// TODO: fix the placeItDrawer, it crashes the game now!
		cBuildingListItem *itemToPlace = m_Player.getSideBar()->getList(LIST_CONSTYARD)->getItemToPlace();
		if (itemToPlace) {
			assert(placeitDrawer);
			placeitDrawer->draw(itemToPlace);
		}
	}
}

void cDrawManager::drawMessage() {
	assert(messageDrawer);
	messageDrawer->draw();

	// TODO: replace messageDrawer with drawMessageBar?
	// messageBarDrawer->drawMessageBar();
}

void cDrawManager::drawMouse() {
	assert(mouseDrawer);
	mouseDrawer->draw();
	cGameControlsContext *context = m_Player.getGameControlsContext();
	if (context->shouldDrawToolTip()) {
		mouseDrawer->drawToolTip();
	}
}

//int points[] =
//{
//    0,0,
//    100,100,
//    540,100,
//    640,0
//};
//
//spline(bmp_screen, points, makecol(255,255,255));//will draw a nice loopy curve
