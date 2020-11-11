#include "../include/d2tmh.h"

cDrawManager::cDrawManager(const cPlayer & thePlayer) : m_Player(thePlayer) {
	assert(&thePlayer);
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer();
	orderDrawer = new cOrderDrawer();
	mapDrawer = new cMapDrawer(&map, thePlayer, mapCamera);
	miniMapDrawer = new cMiniMapDrawer(&map, thePlayer, mapCamera);
	particleDrawer = new cParticleDrawer();
	messageDrawer = new cMessageDrawer();
	messageBarDrawer = new cMessageBarDrawer();
	placeitDrawer = new cPlaceItDrawer();
	structureDrawer = new cStructureDrawer();
	mouseDrawer = new cMouseDrawer(thePlayer);

	cMessageBar * messageBar = messageBarDrawer->getMessageBar();
	messageBar->setX(200);
	messageBar->setY(200);
	messageBar->setWidth(game.screen_x - 160);
}

cDrawManager::~cDrawManager() {
	delete sidebarDrawer;
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
    mapDrawer->drawTerrain(0, 42);

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
	mapDrawer->drawShroud(0, 42);

	drawRallyPoint();

	// GUI
	drawSidebar();
	miniMapDrawer->draw();

	drawStructurePlacing();
	drawCredits();
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
    if (game.selected_structure < 0) return;
    cAbstractStructure * theStructure = structure[game.selected_structure];
    if (!theStructure) return;
    int rallyPointCell = theStructure->getRallyPoint();
    if (rallyPointCell < 0) return;

    set_trans_blender(0,0,0,128);
    int drawX = mapCamera->getWindowXPositionFromCell(rallyPointCell);
    int drawY = mapCamera->getWindowYPositionFromCell(rallyPointCell);

    BITMAP *mouseMoveBitmap = (BITMAP *) gfxdata[MOUSE_MOVE].dat;

    int rallyPointWidthScaled = mapCamera->factorZoomLevel(mouseMoveBitmap->w);
    int rallyPointHeightScaled = mapCamera->factorZoomLevel(mouseMoveBitmap->h);
    allegroDrawer->stretchSprite(mouseMoveBitmap, bmp_screen, drawX, drawY, rallyPointWidthScaled, rallyPointHeightScaled);

    int startX = theStructure->iDrawX() + mapCamera->factorZoomLevel(theStructure->getWidthInPixels() / 2);
    int startY = theStructure->iDrawY() + mapCamera->factorZoomLevel(theStructure->getHeightInPixels() / 2);

    int offset = (mouseMoveBitmap->w/2);
    drawX = mapCamera->getWindowXPositionFromCellWithOffset(rallyPointCell, offset);
    drawY = mapCamera->getWindowYPositionFromCellWithOffset(rallyPointCell, offset);

    int endX = drawX;
    int endY = drawY;

    line(bmp_screen, startX, startY, endX, endY, player[HUMAN].getMinimapColor());
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

void cDrawManager::drawStructurePlacing() {
	if (game.bPlaceIt) {
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
	unscare_mouse();
    select_mouse_cursor(MOUSE_CURSOR_ALLEGRO);
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
