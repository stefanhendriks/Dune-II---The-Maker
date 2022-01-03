#include "../include/d2tmh.h"
#include "cInteractionManager.h"


cInteractionManager::cInteractionManager(cPlayer * thePlayer) : cInputObserver() {
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

void cInteractionManager::onMouseClickedLeft(const s_MouseEvent &) {
}

void cInteractionManager::onMouseClickedRight(const s_MouseEvent &) {
    // not moving the map with the right mouse button, then this means it is a 'click' so act accordingly
    bool isANormalButtonClick = game.getMouse()->isNormalRightClick();
    if (isANormalButtonClick) {
        // TODO: remove this whole thing when we have a deploy mouse state
        if (player->bDeployIt) {
            player->bDeployIt = false;
        }
    }
}

void cInteractionManager::onMouseAt(const s_MouseEvent &) {
}

void cInteractionManager::onMouseScrolledUp(const s_MouseEvent &) {
}

void cInteractionManager::onMouseScrolledDown(const s_MouseEvent &) {
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
        default:
            break;
    }

    // TODO: call state instead (get rid of this interaction manager thing, so we don't need to do this)
    if (game.isState(GAME_PLAYING)) {
        // now call all its other interested listeners
        cGameControlsContext *pContext = player->getGameControlsContext();
        pContext->onNotifyMouseEvent(mouseEvent); // must be first because other classes rely on this context

        sidebar->onNotifyMouseEvent(mouseEvent);
        mapCamera->onNotify(mouseEvent);
        miniMapDrawer->onNotify(mouseEvent);
        orderDrawer->onNotify(mouseEvent);
    }

    mouseDrawer->onNotify(mouseEvent);

    // LAST FOR NOW, as this can change states and thus break things. Hence, if you put this
    // somewhere above this function, the lines after this onNotify might end up pointing to invalid memory addresses
    // and cause a SIGSEV
    game.onNotifyMouseEvent(mouseEvent);
}

void cInteractionManager::onNotifyKeyboardEvent(const s_KeyboardEvent &event) {
    if (game.isState(GAME_PLAYING)) {
        cGameControlsContext *pContext = player->getGameControlsContext();
        pContext->onNotifyKeyboardEvent(event);
    }

    game.onNotifyKeyboardEvent(event);
}
