/**
 * @file PadBindings.hpp
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

#include "controls/eKeyAction.h"
#include "utils/cIniFile.h"

#include <SDL3/SDL.h>

#include <map>
#include <optional>
#include <string>

/**
 * Which gamepad button triggers which key action, and which buttons act as virtual
 * Shift/Ctrl/Alt while held. Read from the [PAD] section of settings.ini.
 * Values are SDL gamepad button names (a, b, x, y, back, start, leftstick, rightstick,
 * leftshoulder, rightshoulder, dpup, dpdown, dpleft, dpright, ...), or "none" to unbind.
 */
class PadBindings {
public:
    // Default bindings, used when there is no [PAD] section or for actions it does not mention
    void loadDefaults();

    // Overrides the defaults with whatever the section mentions
    void loadFromSection(const cSection &section);

    std::optional<SDL_GamepadButton> getButton(eKeyAction action) const;
    const std::map<eKeyAction, SDL_GamepadButton> &getActionButtons() const { return m_actionButtons; }

    std::optional<SDL_GamepadButton> getShiftButton() const { return m_shiftButton; }
    std::optional<SDL_GamepadButton> getCtrlButton() const { return m_ctrlButton; }
    std::optional<SDL_GamepadButton> getAltButton() const { return m_altButton; }

private:
    std::map<eKeyAction, SDL_GamepadButton> m_actionButtons;
    std::optional<SDL_GamepadButton> m_shiftButton;
    std::optional<SDL_GamepadButton> m_ctrlButton;
    std::optional<SDL_GamepadButton> m_altButton;

    void readModifier(const cSection &section, const char *key, std::optional<SDL_GamepadButton> &target);
};
