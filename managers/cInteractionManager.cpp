#include "cInteractionManager.h"
#include <fmt/core.h>

#include "building/cItemBuilder.h"
#include "cDrawManager.h"
#include "controls/cGameControlsContext.h"
#include "d2tmc.h"
#include "drawers/cMiniMapDrawer.h"
#include "player/cPlayer.h"

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

void cInteractionManager::setPlayerToInteractFor(cPlayer *thePlayer) {
    this->sidebar = thePlayer->getSideBar();
    this->player = thePlayer;
    logbook(fmt::format("cInteractionManager::setPlayerToInteractFor for player [{}] [{}]", thePlayer->getId(), thePlayer->getHouseName()));
}

void cInteractionManager::onMouseClickedLeft(const s_MouseEvent &) {
}

void cInteractionManager::onMouseClickedRight(const s_MouseEvent &) {
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
        mapCamera->onNotifyMouseEvent(mouseEvent);
        miniMapDrawer->onNotifyMouseEvent(mouseEvent);
        orderDrawer->onNotify(mouseEvent);
        cItemBuilder *pBuilder = player->getItemBuilder();
        if (pBuilder) {
            pBuilder->onNotifyMouseEvent(mouseEvent);
        }
    }

    mouseDrawer->onNotify(mouseEvent);

    // LAST FOR NOW, as this can change states and thus break things. Hence, if you put this
    // somewhere above this function, the lines after this onNotifyGameEvent might end up pointing to invalid memory addresses
    // and cause a SIGSEV
    game.onNotifyMouseEvent(mouseEvent);
}

void cInteractionManager::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    if (game.isState(GAME_PLAYING)) {
        cGameControlsContext *pContext = player->getGameControlsContext();
        pContext->onNotifyKeyboardEvent(event);

        mapCamera->onNotifyKeyboardEvent(event);
        cItemBuilder *pBuilder = player->getItemBuilder();
        if (pBuilder) {
            pBuilder->onNotifyKeyboardEvent(event);
        }
    }

    game.onNotifyKeyboardEvent(event);
}
