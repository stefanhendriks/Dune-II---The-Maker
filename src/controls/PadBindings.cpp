/**
 * @file PadBindings.cpp
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

#include "controls/PadBindings.hpp"

#include "controls/cKeyBindings.h"
#include "utils/Log.h"

#include <algorithm>
#include <set>

namespace {

// "none" (or an empty value) unbinds. An unknown name is reported and also leaves the button unbound.
std::optional<SDL_GamepadButton> parseButton(const std::string &value, const std::string &key)
{
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower.empty() || lower == "none") return std::nullopt;

    const SDL_GamepadButton button = SDL_GetGamepadButtonFromString(lower.c_str());
    if (button == SDL_GAMEPAD_BUTTON_INVALID) {
        Logger::warn(COMP_INIT, "[PAD]", "Unknown gamepad button '{}' for '{}'", value, key);
        return std::nullopt;
    }
    return button;
}

}

void PadBindings::loadDefaults()
{
    // South, East and the shoulders are used by the cursor mode (clicks and wheel)
    m_actionButtons.clear();
    m_actionButtons[eKeyAction::MENU_BACK] = SDL_GAMEPAD_BUTTON_START;
    m_actionButtons[eKeyAction::MENU_CONFIRM] = SDL_GAMEPAD_BUTTON_WEST;
    m_actionButtons[eKeyAction::CENTER_ON_HOME] = SDL_GAMEPAD_BUTTON_NORTH;
    m_actionButtons[eKeyAction::EDITOR_ZOOM_IN] = SDL_GAMEPAD_BUTTON_DPAD_UP;
    m_actionButtons[eKeyAction::EDITOR_ZOOM_OUT] = SDL_GAMEPAD_BUTTON_DPAD_DOWN;
    m_actionButtons[eKeyAction::EDITOR_UNDO] = SDL_GAMEPAD_BUTTON_DPAD_LEFT;
    m_actionButtons[eKeyAction::EDITOR_REDO] = SDL_GAMEPAD_BUTTON_DPAD_RIGHT;
    m_actionButtons[eKeyAction::EDITOR_SAVE] = SDL_GAMEPAD_BUTTON_BACK;

    m_shiftButton = SDL_GAMEPAD_BUTTON_LEFT_STICK;
    m_ctrlButton = SDL_GAMEPAD_BUTTON_RIGHT_STICK;
    m_altButton.reset();
}

void PadBindings::loadFromSection(const cSection &section)
{
    std::set<eKeyAction> explicitActions;
    for (const auto &[name, action] : cKeyBindings::getActionTable()) {
        if (!section.hasValue(name)) continue;
        const auto button = parseButton(section.getStringValue(name), name);
        if (!button) {
            m_actionButtons.erase(action);
            continue;
        }
        // one button triggers one action: an explicit binding takes the button from a default one
        for (auto it = m_actionButtons.begin(); it != m_actionButtons.end();) {
            if (it->first != action && it->second == *button) {
                if (explicitActions.contains(it->first)) {
                    Logger::warn(COMP_INIT, "[PAD]", "Button '{}' is bound to more than one action, '{}' wins", SDL_GetGamepadStringForButton(*button), name);
                }
                it = m_actionButtons.erase(it);
            } else {
                ++it;
            }
        }
        m_actionButtons[action] = *button;
        explicitActions.insert(action);
    }
    readModifier(section, "MODIFIER_SHIFT", m_shiftButton);
    readModifier(section, "MODIFIER_CTRL", m_ctrlButton);
    readModifier(section, "MODIFIER_ALT", m_altButton);
}

std::optional<SDL_GamepadButton> PadBindings::getButton(eKeyAction action) const
{
    const auto it = m_actionButtons.find(action);
    if (it == m_actionButtons.end()) return std::nullopt;
    return it->second;
}

void PadBindings::readModifier(const cSection &section, const char *key, std::optional<SDL_GamepadButton> &target)
{
    if (!section.hasValue(key)) return;
    target = parseButton(section.getStringValue(key), key);
    if (!target) return;

    // one button triggers one thing: an explicit modifier takes the button from actions and other modifiers
    std::erase_if(m_actionButtons, [&](const auto &entry) { return entry.second == *target; });
    for (auto *other : {&m_shiftButton, &m_ctrlButton, &m_altButton}) {
        if (other != &target && *other == target) other->reset();
    }
}
