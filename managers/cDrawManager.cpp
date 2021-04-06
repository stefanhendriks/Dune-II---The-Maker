#include "../include/d2tmh.h"
#include "cDrawManager.h"


cDrawManager::cDrawManager(cPlayer * thePlayer) : m_Player(thePlayer) {
	assert(&thePlayer);
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer(thePlayer);
	orderDrawer = new cOrderDrawer();
	mapDrawer = new cMapDrawer(&map, thePlayer, mapCamera);
	miniMapDrawer = new cMiniMapDrawer(&map, thePlayer, mapCamera);
	particleDrawer = new cParticleDrawer();
	messageDrawer = new cMessageDrawer();
	placeitDrawer = new cPlaceItDrawer(thePlayer);
	structureDrawer = new cStructureDrawer();
	mouseDrawer = new cMouseDrawer(thePlayer);
    topBarBmp = nullptr;
}

cDrawManager::~cDrawManager() {
    delete creditsDrawer;
    delete sidebarDrawer;
    delete orderDrawer;
	delete mapDrawer;
	delete miniMapDrawer;
	delete particleDrawer;
	delete messageDrawer;
	delete placeitDrawer;
	delete structureDrawer;
	delete mouseDrawer;
}

void cDrawManager::drawCombatState() {
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
	drawDeployment();
    drawTopBarBackground();
	drawCredits();

	// THE MESSAGE
	drawMessage();

	// DO COMBAT MOUSE (TODO: remove this eventually, as it updates state and that is not what
	// this class should be doing)
    game.combat_mouse();

	// MOUSE
    drawCombatMouse();
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

void cDrawManager::drawSidebar() {
    sidebarDrawer->draw();
}

/**
 * When placing a structure, draw a transparent version of the structure we want to place.
 */
void cDrawManager::drawStructurePlacing() {
    if (game.bDeployIt) return; // do not do this
    if (!game.bPlaceIt) return;


    cBuildingListItem *itemToPlace = m_Player->getSideBar()->getList(LIST_CONSTYARD)->getItemToPlace();
    if (itemToPlace == nullptr) return;

    assert(placeitDrawer);
    placeitDrawer->draw(itemToPlace);
}

/**
 * When placing a structure, draw a transparent version of the structure we want to place.
 */
void cDrawManager::drawDeployment() {
    if (game.bPlaceIt) return;
    if (!game.bDeployIt) return;

    // mouse attack special?
    mouse_tile = MOUSE_ATTACK;

    cBuildingListItem *itemToDeploy = m_Player->getSideBar()->getList(LIST_PALACE)->getItemToDeploy();
    if (itemToDeploy == nullptr) return;

    int iMouseCell = m_Player->getGameControlsContext()->getMouseCell();

    //
    if (cMouse::isLeftButtonClicked() && iMouseCell > -1) {
        if (itemToDeploy->getBuildType() == eBuildType::SPECIAL) {
            const s_Special &special = itemToDeploy->getS_Special();
            if (special.providesType == eBuildType::BULLET) {
                // from where
                int structureId = structureUtils.findStructureBy(this->m_Player->getId(), special.deployAtStructure,
                                                                 false);
                if (structureId > -1) {
                    cAbstractStructure *pStructure = structure[structureId];
                    if (pStructure && pStructure->isValid()) {
                        play_sound_id(SOUND_PLACE);
                        create_bullet(special.providesTypeId, pStructure->getCell(), iMouseCell, -1, structureId);
                    }
                }
            }
        }

        itemToDeploy->decreaseTimesToBuild();
        itemToDeploy->setDeployIt(false);
        itemToDeploy->setIsBuilding(false);
        itemToDeploy->resetProgress();
        if (itemToDeploy->getTimesToBuild() < 1) {
            m_Player->getItemBuilder()->removeItemFromList(itemToDeploy);
        }

        game.bDeployIt = false;
    }
}

void cDrawManager::drawMessage() {
	assert(messageDrawer);
	messageDrawer->draw();

	// TODO: replace messageDrawer with drawMessageBar?
	// messageBarDrawer->drawMessageBar();
}

void cDrawManager::drawCombatMouse() {
    drawMouse();

    cGameControlsContext *context = m_Player->getGameControlsContext();
    if (context->shouldDrawToolTip()) {
        mouseDrawer->drawToolTip();
    }
}

void cDrawManager::drawMouse() {
	assert(mouseDrawer);
    select_mouse_cursor(MOUSE_CURSOR_ALLEGRO);
	mouseDrawer->draw();
}

void cDrawManager::drawTopBarBackground() {
    if (topBarBmp == nullptr) {
        topBarBmp = create_bitmap(game.screen_x, 30);
        BITMAP *topbarPiece = (BITMAP *)gfxinter[BMP_TOPBAR_BACKGROUND].dat;
        for (int x = 0; x < game.screen_x; x+= topbarPiece->w) {
            allegroDrawer->drawSprite(topBarBmp, topbarPiece, x, 0);
        }
    }

    allegroDrawer->drawSprite(bmp_screen, topBarBmp, 0, 0);
}

void cDrawManager::destroy() {
    if (topBarBmp != nullptr) {
        destroy_bitmap(topBarBmp);
    }
}

void cDrawManager::setPlayerToDraw(cPlayer * playerToDraw) {
    this->m_Player = playerToDraw;
    this->creditsDrawer->setPlayer(playerToDraw);
    this->sidebarDrawer->setPlayer(playerToDraw);
//    this->orderDrawer->setPlayer(playerToDraw);
    this->miniMapDrawer->setPlayer(playerToDraw);
    interactionManager->setPlayerToInteractFor(playerToDraw);
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
