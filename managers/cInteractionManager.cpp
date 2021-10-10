#include "../include/d2tmh.h"

cInteractionManager::cInteractionManager(cPlayer * thePlayer) : cMouseObserver() {
	assert(thePlayer);
	// does not own these things!
	player = thePlayer;
    sidebar = thePlayer->getSideBar();
	miniMapDrawer = drawManager->getMiniMapDrawer();
	mouseDrawer = drawManager->getMouseDrawer();
	placeItDrawer = drawManager->getPlaceItDrawer();
	orderDrawer = drawManager->getOrderDrawer();
}

cInteractionManager::~cInteractionManager() {
    player = nullptr;
	sidebar = nullptr;
	miniMapDrawer = nullptr;
}

void cInteractionManager::interactWithKeyboard(){
    keyboardManager.interact();
}

void cInteractionManager::setPlayerToInteractFor(cPlayer *thePlayer) {
    this->sidebar = thePlayer->getSideBar();
    this->player = thePlayer;
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
        if (player->bPlaceIt) {
            player->bPlaceIt = false;
        }
        if (player->bDeployIt) {
            player->bDeployIt = false;
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
void cInteractionManager::onNotifyMouseEvent(const s_MouseEvent &mouseEvent) {
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
    cGameControlsContext *pContext = player->getGameControlsContext();
    pContext->onNotify(mouseEvent); // must be first because other classes rely on this context

    // TODO: call state instead (get rid of this interaction manager thing, so we don't need to do this)
    if (game.isState(GAME_PLAYING)) {
        sidebar->onNotifyMouseEvent(mouseEvent);
        placeItDrawer->onNotify(mouseEvent);
        mapCamera->onNotify(mouseEvent);
        miniMapDrawer->onNotify(mouseEvent);
        orderDrawer->onNotify(mouseEvent);
    }

    mouseDrawer->onNotify(mouseEvent);

}
