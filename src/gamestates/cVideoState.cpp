#include "gamestates/cVideoState.h"
#include "utils/ini.h"
#include "controls/cMouse.h"
#include "data/gfxdata.h"
#include "game/cGameInterface.h"
#include "context/GameContext.hpp"

#include "include/cAssert.h"

cVideoState::cVideoState(sGameServices* services)
    : cGameState(services)
{
    d2tm_assert(services != nullptr);
    m_interface = services->ctx->getGameInterface();
    m_mouse = m_interface->getMouse();
    d2tm_assert(m_mouse != nullptr);
}

cVideoState::~cVideoState()
{}

eGameStateType cVideoState::getType()
{
     return eGameStateType::GAMESTATE_VIDEO; 
}


void cVideoState::thinkFast()
{
}

void cVideoState::thinkSlow()
{
}

void cVideoState::draw() const
{
    m_mouse->setTile(MOUSE_NORMAL);

    m_mouse->draw();
}

void cVideoState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED) {
        m_interface->setTransitionToWithFadingOut(GAME_MENU);
    }
}

void cVideoState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        m_interface->setTransitionToWithFadingOut(GAME_MENU);
    }
}