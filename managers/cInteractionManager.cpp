#include "../include/d2tmh.h"
#include "cInteractionManager.h"


cInteractionManager::cInteractionManager(cPlayer * thePlayer) : cMouseObserver() {
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

void cInteractionManager::onMouseClickedLeft(const s_MouseEvent &event) {
}

void cInteractionManager::onMouseClickedRight(const s_MouseEvent &event) {
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
}

void cInteractionManager::onMouseAt(const s_MouseEvent &mouseEvent) {
}

void cInteractionManager::onMouseScrolledUp(const s_MouseEvent &mouseEvent) {
}

void cInteractionManager::onMouseScrolledDown(const s_MouseEvent &mouseEvent) {
}

/**
 * Called by mouse to send an event.
 * @param mouseEvent
 */
void cInteractionManager::onNotify(const s_MouseEvent &mouseEvent) {
    char msg[255];
    sprintf(msg, "cInteractionManager::onNotify %s x=%d, y=%d, z=%d", mouseEvent.toString(mouseEvent.eventType), mouseEvent.x, mouseEvent.y, mouseEvent.z);
    logbook(msg);

    // process these events by itself (if any implementation is present)...
    switch (mouseEvent.eventType) {
        case eMouseEventType::MOUSE_MOVED_TO:
            onMouseAt(mouseEvent);
            break;
        case eMouseEventType::MOUSE_SCROLLED_UP:
            onMouseScrolledUp(mouseEvent);
            break;
        case eMouseEventType::MOUSE_SCROLLED_DOWN:
            onMouseScrolledDown(mouseEvent);
            break;
        case eMouseEventType::MOUSE_LEFT_BUTTON_CLICKED:
            onMouseClickedLeft(mouseEvent);
            break;
        case eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseClickedRight(mouseEvent);
            break;
    }

    // now call all its other interested listeners
    cGameControlsContext *pContext = m_Player->getGameControlsContext();
    pContext->onNotify(mouseEvent); // must be first because other classes rely on this context

    sidebar->onNotify(mouseEvent);
    placeItDrawer->onNotify(mouseEvent);
    mapCamera->onNotify(mouseEvent);
    mouseDrawer->onNotify(mouseEvent);
    miniMapDrawer->onNotify(mouseEvent);
    orderDrawer->onNotify(mouseEvent);

}
