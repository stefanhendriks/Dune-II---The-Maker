#include "../include/d2tmh.h"

cInteractionManager::cInteractionManager(cPlayer * thePlayer) {
	assert(thePlayer);
	sidebar = thePlayer->getSideBar();
	m_Player = thePlayer;
	miniMapDrawer = drawManager->getMiniMapDrawer();
}

cInteractionManager::~cInteractionManager() {
	sidebar = NULL;
	miniMapDrawer = NULL;
}

void cInteractionManager::interact() {
    if (m_Player->getGameControlsContext()->isMouseOnSidebarOrMinimap()) {
        mouseInteractWithSidebarOrMinimap();
    }
    if (m_Player->getGameControlsContext()->isMouseOnBattleField()) {
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
        if (mouse_mv_x2 < -1 && mouse_mv_y2 < -1) {
            game.bPlaceIt = false;
        }
    }
}
