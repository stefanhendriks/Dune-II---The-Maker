#include "../include/d2tmh.h"
#include "cDrawManager.h"


cDrawManager::cDrawManager(cPlayer * thePlayer) : player(thePlayer) {
	assert(&thePlayer);
	creditsDrawer = new CreditsDrawer(thePlayer);
	sidebarDrawer = new cSideBarDrawer(thePlayer);
	orderDrawer = new cOrderDrawer(thePlayer);
	mapDrawer = new cMapDrawer(&map, thePlayer, mapCamera);
	miniMapDrawer = new cMiniMapDrawer(&map, thePlayer, mapCamera);
	particleDrawer = new cParticleDrawer();
	messageDrawer = new cMessageDrawer();
	placeitDrawer = new cPlaceItDrawer(thePlayer);
	structureDrawer = new cStructureDrawer();
	mouseDrawer = new cMouseDrawer(thePlayer);
    topBarBmp = nullptr;
    optionsBar = nullptr;
    sidebarColor = makecol(214, 149, 20);
    textDrawer = new cTextDrawer(game_font);
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
    if (optionsBar) {
        destroy_bitmap(optionsBar);
    }
    delete textDrawer;
}

void cDrawManager::drawCombatState() {
    // MAP
	assert(mapDrawer);
	allegroDrawer->setClippingFor(bmp_screen, 0, cSideBar::TopBarHeight, mapCamera->getWindowWidth(), game.screen_y);
  mapDrawer->drawTerrain();

	// Only draw units/structures, etc, when we do NOT press D
	// TODO: this should be something like : if (keyboard->isDebuggingStructures())
    if (!key[KEY_D] || !key[KEY_TAB]) {
        structureDrawer->drawStructuresFirstLayer();
    }

	// draw layer 1 (beneath units, on top of terrain)
  particleDrawer->determineParticlesToDraw();
	particleDrawer->drawLowerLayer();

	map.draw_units();

	map.draw_bullets();

	structureDrawer->drawStructuresSecondLayer();
	structureDrawer->drawStructuresHealthBars();

	map.draw_units_2nd();

  particleDrawer->drawTopLayer();
	mapDrawer->drawShroud();

	drawRallyPoint();

    allegroDrawer->resetClippingFor(bmp_screen);

    // GUI
	drawSidebar();

	drawOptionBar();

	allegroDrawer->setClippingFor(bmp_screen, 0, cSideBar::TopBarHeight, mapCamera->getWindowWidth(), mapCamera->getWindowHeight() + cSideBar::TopBarHeight);
	drawStructurePlacing();
    allegroDrawer->resetClippingFor(bmp_screen);

    drawDeployment();
    drawTopBarBackground();
	drawCredits();


	// THE MESSAGE
	drawMessage();

    drawNotifications();
//
//	// DO COMBAT MOUSE (TODO: remove this eventually, as it updates state and that is not what
//	// this class should be doing)
//    game.onCombatMouseEvent();

    // for now, call this on game class.
    // TODO: move this "combat" state into own game state class
    game.drawCombatMouse();

    allegroDrawer->resetClippingFor(bmp_screen);

    if (game.bDrawUsages) {
        drawDebugInfoUsages();
        particleDrawer->drawDebugInfo();
    }

	// MOUSE
    drawCombatMouse();
}

void cDrawManager::drawDebugInfoUsages() const {
    int unitsUsed = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        if (unit[i].isValid()) {
            unitsUsed++;
        }
    }

    int structuresUsed = 0;
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = structure[i];
        if (pStructure) {
            structuresUsed++;
        }
    }

    int bulletsUsed = 0;
    for (int i = 0; i < MAX_BULLETS; i++) {
        cBullet &pBullet = bullet[i];
        if (pBullet.bAlive) {
            bulletsUsed++;
        }
    }

    int particlesUsed = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        cParticle &pParticle = particle[i];
        if (pParticle.isValid()) {
            particlesUsed++;
        }
    }

    int startY = 74;
    int height = 14;
    textDrawer->drawTextWithTwoIntegers(0, startY, "Units %d/%d", unitsUsed, MAX_UNITS);
    textDrawer->drawTextWithTwoIntegers(0, startY + 1*height, "Structures %d/%d", structuresUsed, MAX_STRUCTURES);
    textDrawer->drawTextWithTwoIntegers(0, startY + 2*height, "Bullets %d/%d", bulletsUsed, MAX_BULLETS);
    textDrawer->drawTextWithTwoIntegers(0, startY + 3*height, "Particles %d/%d", particlesUsed, MAX_PARTICLES);
}

void cDrawManager::drawCredits() {
	assert(creditsDrawer);
	creditsDrawer->draw();
}

void cDrawManager::drawRallyPoint() {
    cPlayer &humanPlayer = players[HUMAN];
    if (humanPlayer.selected_structure < 0) return;
    cAbstractStructure * theStructure = structure[humanPlayer.selected_structure];
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

    line(bmp_screen, startX, startY, endX, endY, players[HUMAN].getMinimapColor());
}

void cDrawManager::drawSidebar() {
    allegroDrawer->setClippingFor(bmp_screen, game.screen_x-cSideBar::SidebarWidth, 0, game.screen_x, game.screen_y);
    sidebarDrawer->draw();
    miniMapDrawer->draw();
    allegroDrawer->resetClippingFor(bmp_screen);
}

/**
 * When placing a structure, draw a transparent version of the structure we want to place.
 */
void cDrawManager::drawStructurePlacing() {
    if (player->bDeployIt) return; // do not do this
    if (!player->bPlaceIt) return;


    cBuildingListItem *itemToPlace = player->getSideBar()->getList(LIST_CONSTYARD)->getItemToPlace();
    if (itemToPlace == nullptr) return;

    assert(placeitDrawer);
    placeitDrawer->draw(itemToPlace);
}

/**
 * When deploying something
 */
void cDrawManager::drawDeployment() {
    if (player->bPlaceIt) return;
    if (!player->bDeployIt) return;

    // mouse attack special?
    game.getMouse()->setTile(MOUSE_ATTACK);

    cBuildingListItem *itemToDeploy = player->getSideBar()->getList(LIST_PALACE)->getItemToDeploy();
    if (itemToDeploy == nullptr) return;

    int iMouseCell = player->getGameControlsContext()->getMouseCell();

    if (game.getMouse()->isLeftButtonClicked() && iMouseCell > -1) {
        s_GameEvent event {
                .eventType = eGameEventType::GAME_EVENT_SPECIAL_LAUNCH,
                .entityType = eBuildType::SPECIAL,
                .entityID = -1,
                .player = player,
                .entitySpecificType = itemToDeploy->getBuildId(),
                .atCell = iMouseCell,
                .isReinforce = false,
                .buildingListItem = itemToDeploy
        };
        game.onNotify(event);
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

    cGameControlsContext *context = player->getGameControlsContext();
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

        set_palette(player->pal);

        allegroDrawer->drawSprite(topBarBmp, (BITMAP *)gfxinter[BTN_OPTIONS].dat, 1, 0);
    }

    allegroDrawer->drawSprite(bmp_screen, topBarBmp, 0, 0);

    //HACK HACK: for now do it like this, instead of using an actual GUI object here
    cRectangle optionsRect = cRectangle(0,0, 162, 30);
    if (game.getMouse()->isLeftButtonClicked() && game.getMouse()->isOverRectangle(&optionsRect)) {
        game.setNextStateToTransitionTo(GAME_OPTIONS);
    }
}

void cDrawManager::destroy() {
    if (topBarBmp != nullptr) {
        destroy_bitmap(topBarBmp);
    }
}

void cDrawManager::setPlayerToDraw(cPlayer * playerToDraw) {
    this->player = playerToDraw;
    this->creditsDrawer->setPlayer(playerToDraw);
    this->sidebarDrawer->setPlayer(playerToDraw);
    this->orderDrawer->setPlayer(playerToDraw);
    this->miniMapDrawer->setPlayer(playerToDraw);
    this->mapDrawer->setPlayer(playerToDraw);
}

void cDrawManager::drawOptionBar() {
    // upper bar
    rectfill(bmp_screen, 0, 0, game.screen_x, cSideBar::TopBarHeight, makecol(0,0,0));
    if (optionsBar == NULL) {
        optionsBar = create_bitmap(game.screen_x, 40);
        clear_to_color(optionsBar, sidebarColor);

        for (int w = 0; w < (game.screen_x + 800); w += 789) {
            draw_sprite(optionsBar, (BITMAP *)gfxinter[BMP_GERALD_TOP_BAR].dat, w, 31);
        }
    }

    draw_sprite(bmp_screen, optionsBar, 0, 0);
}

void cDrawManager::drawNotifications() {
    std::vector<cPlayerNotification> &notifications = player->getNotifications();
//    int y = cSideBar::TopBarHeight + 14; // 12 pixels
    int y = game.screen_y - 11;
    for (auto &notification : notifications) {
        textDrawer->drawText(4, y, notification.getColor(), notification.getMessage().c_str());
        y-=15;
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
