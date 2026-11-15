/**
 * @file GamepadKeys.cpp
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

#include "controls/GamepadKeys.hpp"

#include "controls/Gamepad.hpp"
#include "controls/cKeyBindings.h"
#include "controls/cKeyboard.h"

void GamepadKeys::pressModifier(cKeyboard &keyboard, SDL_Scancode modifier)
{
    if (m_modifierHolds[modifier]++ == 0) {
        keyboard.injectKey(modifier, true);
    }
}

void GamepadKeys::releaseModifier(cKeyboard &keyboard, SDL_Scancode modifier)
{
    if (--m_modifierHolds[modifier] == 0) {
        keyboard.injectKey(modifier, false);
    }
}

void GamepadKeys::handleModifierButton(const Gamepad &gamepad, cKeyboard &keyboard, std::optional<SDL_GamepadButton> button, SDL_Scancode modifier)
{
    if (!button) return;
    if (gamepad.wasPressed(*button)) {
        pressModifier(keyboard, modifier);
    } else if (gamepad.wasReleased(*button)) {
        releaseModifier(keyboard, modifier);
    }
}

void GamepadKeys::update(const Gamepad &gamepad, cKeyboard &keyboard)
{
    // modifiers of actions that were released last frame: their key-up has been handled by now
    for (SDL_Scancode modifier : m_modifiersToRelease) {
        releaseModifier(keyboard, modifier);
    }
    m_modifiersToRelease.clear();

    handleModifierButton(gamepad, keyboard, m_bindings.getShiftButton(), SDL_SCANCODE_LSHIFT);
    handleModifierButton(gamepad, keyboard, m_bindings.getCtrlButton(), SDL_SCANCODE_LCTRL);
    handleModifierButton(gamepad, keyboard, m_bindings.getAltButton(), SDL_SCANCODE_LALT);

    const cKeyBindings *keyBindings = keyboard.getKeyBindings();
    if (!keyBindings) return;

    for (const auto &[action, button] : m_bindings.getActionButtons()) {
        const bool pressed = gamepad.wasPressed(button);
        const bool released = gamepad.wasReleased(button);
        if (!pressed && !released) continue;

        const s_KeyBinding *binding = keyBindings->getBinding(action);
        if (!binding || binding->keys.empty()) continue;

        const SDL_Scancode key = binding->keys.front();
        std::vector<SDL_Scancode> required;
        if (binding->requireCtrl) required.push_back(SDL_SCANCODE_LCTRL);
        if (binding->requireAlt) required.push_back(SDL_SCANCODE_LALT);
        if (binding->requireShift) required.push_back(SDL_SCANCODE_LSHIFT);
        if (binding->requireTab) required.push_back(SDL_SCANCODE_TAB);

        if (pressed) {
            for (SDL_Scancode modifier : required) {
                pressModifier(keyboard, modifier);
            }
            keyboard.injectKey(key, true);
        } else {
            keyboard.injectKey(key, false);
            m_modifiersToRelease.insert(m_modifiersToRelease.end(), required.begin(), required.end());
        }
    }
}
