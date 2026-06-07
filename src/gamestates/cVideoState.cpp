#include "gamestates/cVideoState.h"
#include "utils/ini.h"
#include "controls/cMouse.h"
#include "data/gfxdata.h"
#include "utils/Graphics.hpp"
#include "game/cGameInterface.h"
#include "context/GameContext.hpp"
#include "drawers/SDLDrawer.hpp"

#include "include/cAssert.h"
#include "utils/cLog.h"

cVideoState::cVideoState(sGameServices* services)
    : cGameState(services)
{
    d2tm_assert(services != nullptr);
    m_interface = services->ctx->getGameInterface();
    m_mouse = m_interface->getMouse();
    d2tm_assert(m_mouse != nullptr);

    const auto filename = "intro.pak";
    gfxmovie = std::make_unique<Graphics>(m_renderDrawer->getRenderer(),filename);

    m_movieFrame=0;
    if (gfxmovie != nullptr) {
        cLogger::getInstance()->log(LOG_INFO, COMP_INIT, "video", std::format("Successful loaded video [{}]", filename));
        m_currentFrame = gfxmovie->getTexture(m_movieFrame);
        m_timerFrame = 0;
        offsetX = (m_interface->getGameSettings()->getScreenW() - 640) / 2-75;
        offsetY = (m_interface->getGameSettings()->getScreenH() - 480) / 2-75;
        return;
    } else {
        cLogger::getInstance()->log(LOG_ERROR, COMP_INIT, "video", std::format("Failed to load video [{}]", filename));
        return;
    }
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

void cVideoState::thinkNormal()
{
    if (gfxmovie != nullptr) {
        if (m_movieFrame < gfxmovie->getNumberOfFiles()) {
            m_currentFrame = gfxmovie->getTexture(m_movieFrame);
            m_movieFrame++;
        } else {
            m_interface->setTransitionToWithFadingOut(GAME_MENU);
        }
    } else {
        m_interface->setTransitionToWithFadingOut(GAME_MENU);
    }
}

void cVideoState::draw() const
{
    m_mouse->setTile(MOUSE_NORMAL);
    if (m_currentFrame != nullptr) {
        cRectangle dest = {offsetX, offsetY,640+150, 480+150};
        m_renderDrawer->renderStrechFullSprite(m_currentFrame, dest);
    }
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