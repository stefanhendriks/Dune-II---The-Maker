/**
 * @file cVideoState.cpp
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
// Videos are always shown 640 wide; the height may vary but is never more than 480.
// Centering and drawing currently treat the height as always 480 (the maximum).
// NOTE: the source frames are reportedly 320x288 (drawn scaled up).
constexpr int VIDEO_WIDTH = 640;
constexpr int VIDEO_MAX_HEIGHT = 480;
// Shifts the video towards the top left of the screen after centering
constexpr int VIDEO_CENTER_SHIFT = 75;
// Added to width and max height to get the size the video is drawn at (790x630)
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

    m_gfxMovie = std::make_unique<Graphics>(m_sdlDrawer->getRenderer(), filename);

    // a missing (or invalid) pack has no frames: thinkNormal sends us back to the main menu
    if (m_gfxMovie->getNumberOfFiles() > 0) {
        Logger::info(COMP_INIT, "video", "Successful loaded video [{}]", filename);
        m_currentFrame = m_gfxMovie->getTexture(m_movieFrame);
        m_offsetX = (m_settings->getScreenW() - VIDEO_WIDTH) / 2 - VIDEO_CENTER_SHIFT;
        m_offsetY = (m_settings->getScreenH() - VIDEO_MAX_HEIGHT) / 2 - VIDEO_CENTER_SHIFT;
    } else {
        Logger::error(COMP_INIT, "video", "Failed to load video [{}], returning to main menu", filename);
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
    if (m_movieFrame < m_gfxMovie->getNumberOfFiles()) {
        m_currentFrame = m_gfxMovie->getTexture(m_movieFrame);
        m_movieFrame++;
    } else {
        m_stateTransitions->setNextStateWithFade(GAME_MENU);
    }
}

void cVideoState::draw() const
{
    m_mouse->setTile(MOUSE_NORMAL);
    if (m_currentFrame != nullptr) {
        cRectangle dest = {m_offsetX, m_offsetY, VIDEO_WIDTH + VIDEO_DRAW_EXTRA, VIDEO_MAX_HEIGHT + VIDEO_DRAW_EXTRA};
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