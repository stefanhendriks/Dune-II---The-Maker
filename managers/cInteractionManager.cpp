#include "../include/d2tmh.h"
#include "cInteractionManager.h"


cInteractionManager::cInteractionManager(cPlayer * thePlayer) {
	assert(thePlayer);
	// does not own these things!
	sidebar = thePlayer->getSideBar();
	m_Player = thePlayer;
	miniMapDrawer = drawManager->getMiniMapDrawer();
}

cInteractionManager::~cInteractionManager() {
    m_Player = nullptr;
	sidebar = nullptr;
	miniMapDrawer = nullptr;
}

void cInteractionManager::interact() {
    cGameControlsContext *pContext = m_Player->getGameControlsContext();

    if (pContext->isMouseOnSidebarOrMinimap()) {
        mouseInteractWithSidebarOrMinimap();
    }

    if (pContext->isMouseOnBattleField()) {
        mouseInteractWithBattlefield();
    }
}

void cInteractionManager::interactWithKeyboard(){
    keyboardManager.interact();
}

void cInteractionManager::mouseInteractWithSidebarOrMinimap() {
	sidebar->thinkInteraction();
	miniMapDrawer->interact();
}

void cInteractionManager::mouseInteractWithBattlefield() {
    // MOUSE WHEEL scrolling causes zooming in/out
    if (cMouse::isMouseScrolledUp()) {
        mapCamera->zoomOut();
    }

    if (cMouse::isMouseScrolledDown()) {
        mapCamera->zoomIn();
    }

    if (game.bPlaceIt && cMouse::isRightButtonClicked()) {
        // not moving the map with the right mouse button, then cancel placement
        if (mouse_mv_x2 < -1 && mouse_mv_y2 < -1) { // < -1 means we have had this evaluation before :/
            game.bPlaceIt = false;
        }
    }

    // make -1 to -2, so that we can prevent placeIt=false when just stopped viewport dragging
    if (mouse_mv_x2 == -1) {
        mouse_mv_x2 = -2;
    }
    if (mouse_mv_y2 == -1) {
        mouse_mv_y2 = -2;
    }
}

void cInteractionManager::setPlayerToInteractFor(cPlayer *thePlayer) {
    this->sidebar = thePlayer->getSideBar();
    this->m_Player = thePlayer;
    char msg[255];
    sprintf(msg, "cInteractionManager::setPlayerToInteractFor for player [%d] [%s]", thePlayer->getId(), thePlayer->getHouseName().c_str());
    logbook(msg);
}
