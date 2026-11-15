/**
 * @file Gamepad.hpp
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

#include "controls/GamepadMath.hpp"

#include <SDL3/SDL.h>

#include <array>

/**
 * Reads the first connected gamepad. Handles hot-plugging, and is polled once per frame via update().
 * Exposes button press/release edges and normalised sticks (radial deadzone applied).
 * Nothing in the game reacts to this yet.
 */
class Gamepad {
public:
    static constexpr float DEFAULT_DEADZONE = 0.2f;

    explicit Gamepad(float deadzone = DEFAULT_DEADZONE);
    ~Gamepad();

    Gamepad(const Gamepad &) = delete;
    Gamepad &operator=(const Gamepad &) = delete;

    // To be called from the SDL event loop. Handles connect/disconnect.
    void handleEvent(const SDL_Event &event);

    // Poll the device once per frame: refreshes edges and sticks.
    void update();

    bool isConnected() const { return m_gamepad != nullptr; }

    bool isDown(SDL_GamepadButton button) const { return m_down[index(button)]; }
    bool wasPressed(SDL_GamepadButton button) const { return m_down[index(button)] && !m_previousDown[index(button)]; }
    bool wasReleased(SDL_GamepadButton button) const { return !m_down[index(button)] && m_previousDown[index(button)]; }

    GamepadMath::s_Stick getLeftStick() const { return m_leftStick; }
    GamepadMath::s_Stick getRightStick() const { return m_rightStick; }

private:
    static constexpr size_t BUTTON_COUNT = SDL_GAMEPAD_BUTTON_COUNT;
    static size_t index(SDL_GamepadButton button) { return static_cast<size_t>(button); }

    void open(SDL_JoystickID id);
    void close();
    void openFirstConnected();
    GamepadMath::s_Stick readStick(SDL_GamepadAxis xAxis, SDL_GamepadAxis yAxis) const;

    SDL_Gamepad *m_gamepad = nullptr;
    SDL_JoystickID m_id = 0;
    float m_deadzone;
    std::array<bool, BUTTON_COUNT> m_down{};
    std::array<bool, BUTTON_COUNT> m_previousDown{};
    GamepadMath::s_Stick m_leftStick;
    GamepadMath::s_Stick m_rightStick;
};
