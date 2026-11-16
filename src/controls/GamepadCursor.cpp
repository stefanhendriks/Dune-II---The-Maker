/**
 * @file GamepadCursor.cpp
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

#include "controls/GamepadCursor.hpp"

#include "controls/Gamepad.hpp"
#include "controls/cMouse.h"

#include <algorithm>

void GamepadCursor::update(const Gamepad &gamepad, cMouse &mouse, int screenW, int screenH, float speed, float curve)
{
    const Uint64 now = SDL_GetTicks();
    // cap the step so a long frame (or the first call) does not make the cursor jump
    const float dt = m_lastTicks == 0 ? 0.0f : std::min(static_cast<float>(now - m_lastTicks) / 1000.0f, 0.1f);
    m_lastTicks = now;

    if (!gamepad.isConnected()) {
        // pad unplugged while a button was held: release, or the game keeps seeing it held
        mouse.setLeftButton(false);
        mouse.setRightButton(false);
        return;
    }

    // the real mouse may have moved the cursor since last frame: continue from where it is
    if (static_cast<int>(m_x) != mouse.getX() || static_cast<int>(m_y) != mouse.getY()) {
        m_x = static_cast<float>(mouse.getX());
        m_y = static_cast<float>(mouse.getY());
    }

    const auto delta = GamepadMath::cursorDelta(gamepad.getLeftStick(), speed, curve, dt);
    m_x = std::clamp(m_x + delta.x, 0.0f, static_cast<float>(screenW - 1));
    m_y = std::clamp(m_y + delta.y, 0.0f, static_cast<float>(screenH - 1));
    mouse.moveTo(static_cast<int>(m_x), static_cast<int>(m_y));

    // after moving, so a press and a move in the same frame click at the new position
    mouse.setLeftButton(gamepad.isDown(SDL_GAMEPAD_BUTTON_SOUTH));
    mouse.setRightButton(gamepad.isDown(SDL_GAMEPAD_BUTTON_EAST));

    if (gamepad.wasPressed(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) {
        mouse.scroll(true);
    }
    if (gamepad.wasPressed(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) {
        mouse.scroll(false);
    }
}
