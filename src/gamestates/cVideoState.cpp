#include "gamestates/cVideoState.h"
#include "utils/ini.h"
#include "controls/cMouse.h"
#include "data/gfxdata.h"
#include "utils/Graphics.hpp"
#include "game/GameStateTransition.hpp"
#include "game/cGameSettings.h"
#include "context/GameContext.hpp"
#include "drawers/SDLDrawer.hpp"

#include "include/cAssert.h"
#include "utils/Log.h"

cVideoState::cVideoState(sGameServices* services)
    : cGameState(services)
{
    d2tm_assert(services != nullptr);
    m_settings = services->settings;
    m_stateTransitions = services->stateTransitions;
    m_mouse = services->mouse;
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_stateTransitions != nullptr);
    d2tm_assert(m_mouse != nullptr);

    const auto filename = "intro.pak";
    gfxmovie = std::make_unique<Graphics>(m_sdlDrawer->getRenderer(),filename);

    m_movieFrame=0;
    if (gfxmovie != nullptr) {
        Logger::info(COMP_INIT, "video", "Successful loaded video [{}]", filename);
        m_currentFrame = gfxmovie->getTexture(m_movieFrame);
        offsetX = (m_settings->getScreenW() - 640) / 2-75;
        offsetY = (m_settings->getScreenH() - 480) / 2-75;
        return;
    } else {
        Logger::error(COMP_INIT, "video", "Failed to load video [{}]", filename);
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
            m_stateTransitions->setNextStateWithFade(GAME_MENU);
        }
    } else {
        m_stateTransitions->setNextStateWithFade(GAME_MENU);
    }
}

void cVideoState::draw() const
{
    m_mouse->setTile(MOUSE_NORMAL);
    if (m_currentFrame != nullptr) {
        cRectangle dest = {offsetX, offsetY,640+150, 480+150};
        m_sdlDrawer->renderStrechFullSprite(m_currentFrame, dest);
    }
    m_mouse->draw();
}

void cVideoState::onNotifyMouseEvent(const s_MouseEvent &event)
{
    if (event.eventType == eMouseEventType::MOUSE_LEFT_BUTTON_PRESSED) {
        m_stateTransitions->setNextStateWithFade(GAME_MENU);
    }
}

void cVideoState::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    if (event.isType(eKeyEventType::PRESSED)) {
        m_stateTransitions->setNextStateWithFade(GAME_MENU);
    }
}