/**
 * @file Gamepad.cpp
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

#include "controls/Gamepad.hpp"

#include "utils/Log.h"

Gamepad::Gamepad(float deadzone)
    : m_deadzone(deadzone)
{
    // pads that were already plugged in before the game started
    openFirstConnected();
}

Gamepad::~Gamepad()
{
    close();
}

void Gamepad::handleEvent(const SDL_Event &event)
{
    switch (event.type) {
        case SDL_EVENT_GAMEPAD_ADDED:
            if (!m_gamepad) {
                open(event.gdevice.which);
            }
            break;
        case SDL_EVENT_GAMEPAD_REMOVED:
            if (m_gamepad && event.gdevice.which == m_id) {
                close();
                openFirstConnected(); // another pad may still be connected
            }
            break;
        default:
            break;
    }
}

void Gamepad::update()
{
    m_previousDown = m_down;
    if (!m_gamepad) {
        // after a disconnect: report releases once, then stay quiet
        m_down.fill(false);
        return;
    }

    for (size_t i = 0; i < BUTTON_COUNT; i++) {
        m_down[i] = SDL_GetGamepadButton(m_gamepad, static_cast<SDL_GamepadButton>(i));
    }
    m_leftStick = readStick(SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY);
    m_rightStick = readStick(SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY);
}

void Gamepad::openFirstConnected()
{
    int count = 0;
    SDL_JoystickID *ids = SDL_GetGamepads(&count);
    if (!ids) return;
    if (count > 0) {
        open(ids[0]);
    }
    SDL_free(ids);
}

void Gamepad::open(SDL_JoystickID id)
{
    m_gamepad = SDL_OpenGamepad(id);
    if (!m_gamepad) {
        Logger::warn(COMP_INIT, "Gamepad", "Could not open gamepad: {}", SDL_GetError());
        return;
    }
    m_id = id;
    Logger::info(COMP_INIT, "Gamepad", "Connected: {}", SDL_GetGamepadName(m_gamepad));
}

void Gamepad::close()
{
    if (!m_gamepad) return;
    Logger::info(COMP_INIT, "Gamepad", "Disconnected: {}", SDL_GetGamepadName(m_gamepad));
    SDL_CloseGamepad(m_gamepad);
    m_gamepad = nullptr;
    m_id = 0;
    m_leftStick = {};
    m_rightStick = {};
}

GamepadMath::s_Stick Gamepad::readStick(SDL_GamepadAxis xAxis, SDL_GamepadAxis yAxis) const
{
    const float x = GamepadMath::normalizeAxis(SDL_GetGamepadAxis(m_gamepad, xAxis));
    const float y = GamepadMath::normalizeAxis(SDL_GetGamepadAxis(m_gamepad, yAxis));
    return GamepadMath::applyRadialDeadzone(x, y, m_deadzone);
}
