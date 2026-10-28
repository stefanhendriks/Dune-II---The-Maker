/**
 * @file cWinLoseState.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "gamestates/cVideoState.h"
#include "controls/cMouse.h"
#include "data/gfxdata.h"
#include "utils/Graphics.hpp"
#include "game/GameStateTransition.hpp"
#include "game/cGameSettings.h"
#include "context/GameContext.hpp"
#include "drawers/SDLDrawer.hpp"

#include "include/cAssert.h"
#include "utils/Log.h"

namespace {
// Size used to center the video on screen. NOTE: the source frames are reportedly 320x288 (drawn
// scaled up), so these match neither the source frames nor the size they are drawn at.
constexpr int VIDEO_CENTER_WIDTH = 640;
constexpr int VIDEO_CENTER_HEIGHT = 480;
// Shifts the video towards the top left of the screen after centering
constexpr int VIDEO_CENTER_SHIFT = 75;
// Added to the centering size to get the size the video is drawn at (790x630)
constexpr int VIDEO_DRAW_EXTRA = 150;
}

cVideoState::cVideoState(sGameServices* services, const std::string &filename)
    : cGameState(services)
{
    d2tm_assert(services != nullptr);
    m_settings = services->settings;
    m_stateTransitions = services->stateTransitions;
    m_mouse = services->mouse;
    d2tm_assert(m_settings != nullptr);
    d2tm_assert(m_stateTransitions != nullptr);
    d2tm_assert(m_mouse != nullptr);

    gfxmovie = std::make_unique<Graphics>(m_sdlDrawer->getRenderer(),filename);

    m_movieFrame=0;
    if (gfxmovie != nullptr) {
        Logger::info(COMP_INIT, "video", "Successful loaded video [{}]", filename);
        m_currentFrame = gfxmovie->getTexture(m_movieFrame);
        offsetX = (m_settings->getScreenW() - VIDEO_CENTER_WIDTH) / 2 - VIDEO_CENTER_SHIFT;
        offsetY = (m_settings->getScreenH() - VIDEO_CENTER_HEIGHT) / 2 - VIDEO_CENTER_SHIFT;
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
    // one frame per call: thinkNormal runs every ~100 ms, so the video plays at 10 fps
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
        cRectangle dest = {offsetX, offsetY, VIDEO_CENTER_WIDTH + VIDEO_DRAW_EXTRA, VIDEO_CENTER_HEIGHT + VIDEO_DRAW_EXTRA};
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