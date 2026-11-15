/**
 * @file GamepadKeys.hpp
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

#include "controls/PadBindings.hpp"

#include <SDL3/SDL.h>

#include <map>
#include <vector>

class Gamepad;
class cKeyboard;

/**
 * Turns gamepad buttons into key actions by feeding the keyboard the keys (and modifiers) of
 * the bound action, exactly as if the player pressed them. The keyboard bindings stay the
 * single source of truth for what an action needs.
 */
class GamepadKeys {
public:
    void setBindings(const PadBindings &bindings) { m_bindings = bindings; }

    // Call once per frame, after Gamepad::update() and before cKeyboard::updateState()
    void update(const Gamepad &gamepad, cKeyboard &keyboard);

private:
    void pressModifier(cKeyboard &keyboard, SDL_Scancode modifier);
    void releaseModifier(cKeyboard &keyboard, SDL_Scancode modifier);
    void handleModifierButton(const Gamepad &gamepad, cKeyboard &keyboard, std::optional<SDL_GamepadButton> button, SDL_Scancode modifier);

    PadBindings m_bindings;
    // how many pad buttons or actions currently hold a modifier key down
    std::map<SDL_Scancode, int> m_modifierHolds;
    // modifiers an action needed: released one frame after its key, so the key-up still sees them
    std::vector<SDL_Scancode> m_modifiersToRelease;
};
