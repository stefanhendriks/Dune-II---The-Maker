#include "cInteractionManager.h"
#include <format>

#include "building/cItemBuilder.h"
#include "cDrawManager.h"
#include "controls/cGameControlsContext.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "drawers/cMiniMapDrawer.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/map/cMapCamera.h"

cInteractionManager::cInteractionManager(cPlayer *thePlayer) : cInputObserver()
{
    assert(thePlayer!=nullptr);
    // does not own these things!
    m_player = thePlayer;
    m_sidebar = thePlayer->getSideBar();
    logbook("cInteractionManager constructor");
}

cInteractionManager::~cInteractionManager()
{
    m_player = nullptr;
    m_sidebar = nullptr;
    logbook("cInteractionManager destructor");
}

void cInteractionManager::setPlayerToInteractFor(cPlayer *thePlayer)
{
    this->m_sidebar = thePlayer->getSideBar();
    this->m_player = thePlayer;
    logbook(std::format("cInteractionManager::setPlayerToInteractFor for player [{}] [{}]", thePlayer->getId(), thePlayer->getHouseName()));
}

void cInteractionManager::onMouseClickedLeft(const s_MouseEvent &)
{
}

void cInteractionManager::onMouseClickedRight(const s_MouseEvent &)
{
}

void cInteractionManager::onMouseAt(const s_MouseEvent &)
{
}

void cInteractionManager::onMouseScrolledUp(const s_MouseEvent &)
{
}

void cInteractionManager::onMouseScrolledDown(const s_MouseEvent &)
{
}

/**
 * Called by mouse to send an event.
 * @param mouseEvent
 */
void cInteractionManager::onNotifyMouseEvent(const s_MouseEvent &mouseEvent)
{
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
        cGameControlsContext *pContext = m_player->getGameControlsContext();
        pContext->onNotifyMouseEvent(mouseEvent); // must be first because other classes rely on this context

        m_sidebar->onNotifyMouseEvent(mouseEvent);
        game.m_mapCamera->onNotifyMouseEvent(mouseEvent);

        // do like this because drawManager gets deleted/recreated
        game.m_drawManager->getMiniMapDrawer()->onNotifyMouseEvent(mouseEvent);
        game.m_drawManager->getOrderDrawer()->onNotify(mouseEvent);
        cItemBuilder *pBuilder = m_player->getItemBuilder();
        if (pBuilder) {
            pBuilder->onNotifyMouseEvent(mouseEvent);
        }
    }

    // do like this because drawManager gets deleted/recreated
    game.m_drawManager->getMouseDrawer()->onNotify(mouseEvent);

    // LAST FOR NOW, as this can change states and thus break things. Hence, if you put this
    // somewhere above this function, the lines after this onNotifyGameEvent might end up pointing to invalid memory addresses
    // and cause a SIGSEV
    game.onNotifyMouseEvent(mouseEvent);
}

void cInteractionManager::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (game.isState(GAME_PLAYING)) {
        cGameControlsContext *pContext = m_player->getGameControlsContext();
        pContext->onNotifyKeyboardEvent(event);

        game.m_mapCamera->onNotifyKeyboardEvent(event);
        cItemBuilder *pBuilder = m_player->getItemBuilder();
        if (pBuilder) {
            pBuilder->onNotifyKeyboardEvent(event);
        }
    }

    game.onNotifyKeyboardEvent(event);
}
