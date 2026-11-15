/**
 * @file GamepadCursor.hpp
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

#pragma once

#include <SDL3/SDL.h>

class Gamepad;
class cMouse;

/**
 * Drives the mouse cursor from the gamepad: left stick moves it, South is left click,
 * East is right click, shoulder buttons are the mouse wheel. Mouse and keyboard stay active.
 */
class GamepadCursor {
public:
    static constexpr float RESPONSE_EXPONENT = 2.0f;

    void update(const Gamepad &gamepad, cMouse &mouse, int screenW, int screenH, float speed);

private:
    float m_x = 0.0f;
    float m_y = 0.0f;
    Uint64 m_lastTicks = 0;
};
