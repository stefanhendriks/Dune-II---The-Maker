#include "../include/d2tmh.h"
#include "cInteractionManager.h"


cInteractionManager::cInteractionManager(cPlayer * thePlayer) {
	assert(thePlayer);
	// does not own these things!
	m_Player = thePlayer;
    sidebar = thePlayer->getSideBar();
	miniMapDrawer = drawManager->getMiniMapDrawer();
	mouseDrawer = drawManager->getMouseDrawer();
	placeItDrawer = drawManager->getPlaceItDrawer();
	orderDrawer = drawManager->getOrderDrawer();
}

cInteractionManager::~cInteractionManager() {
    m_Player = nullptr;
	sidebar = nullptr;
	miniMapDrawer = nullptr;
}

void cInteractionManager::interactWithKeyboard(){
    keyboardManager.interact();
}

void cInteractionManager::setPlayerToInteractFor(cPlayer *thePlayer) {
    this->sidebar = thePlayer->getSideBar();
    this->m_Player = thePlayer;
    char msg[255];
    sprintf(msg, "cInteractionManager::setPlayerToInteractFor for player [%d] [%s]", thePlayer->getId(), thePlayer->getHouseName().c_str());
    logbook(msg);
}

void cInteractionManager::onMouseClickedLeft(int x, int y) {
    cGameControlsContext *pContext = m_Player->getGameControlsContext();
    if (pContext->isMouseOnSidebarOrMinimap()) {
        sidebar->onMouseClickedLeft(x, y);
    }

    miniMapDrawer->onMouseClickedLeft(x, y);

    placeItDrawer->onMouseClickedLeft(x, y);
}

void cInteractionManager::onMouseClickedRight(int x, int y) {
    // not moving the map with the right mouse button, then this means it is a 'click' so act accordingly
    bool isANormalButtonClick = mouse_mv_x2 < -1 && mouse_mv_y2 < -1; // < -1 means we have had this evaluation before :/
    if (isANormalButtonClick) {
        if (game.bPlaceIt) {
            game.bPlaceIt = false;
        }
        if (game.bDeployIt) {
            game.bDeployIt = false;
        }
    }

    cGameControlsContext *pContext = m_Player->getGameControlsContext();
    if (pContext->isMouseOnSidebarOrMinimap()) {
        sidebar->onMouseClickedRight(x, y);
    }
}

void cInteractionManager::onMouseAt(int x, int y) {
    cGameControlsContext *pContext = m_Player->getGameControlsContext();
    pContext->onMouseAt(x, y);

    if (pContext->isMouseOnSidebarOrMinimap()) {
        sidebar->onMouseAt(x, y);
    }

    mouseDrawer->onMouseAt(x, y);
    miniMapDrawer->onMouseAt(x, y);
    orderDrawer->onMouseAt(x, y);
}

void cInteractionManager::onMouseScrolledUp() {
    // MOUSE WHEEL scrolling causes zooming in/out
    mapCamera->zoomOut();
}

void cInteractionManager::onMouseScrolledDown() {
    mapCamera->zoomIn();
}
