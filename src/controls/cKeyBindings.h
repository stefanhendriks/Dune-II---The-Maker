/**
 * @file cKeyBindings.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include <bitset>
#include <map>
#include <vector>
#include <string>
#include <initializer_list>
#include <SDL3/SDL.h>

#include "controls/eKeyAction.h"
#include "controls/eKeyboardEnum.h"
#include "utils/cIniFile.h"

struct s_KeyBinding {
    std::vector<SDL_Scancode> keys;
    bool requireCtrl  = false;
    bool requireAlt   = false;
    bool requireShift = false;
};

class cKeyBindings {
public:
    void loadDefaults();
    void loadFromSection(const cSection &section);

    /**
     * Returns true when the given key set and modifier combo match the binding for the given action.
     * Called internally by cKeyboardEvent::isAction().
     */
    bool matches(const std::bitset<SDL_SCANCODE_COUNT> &keys, const s_KeysCombo &combo, eKeyAction action) const;

private:
    std::map<eKeyAction, s_KeyBinding> m_bindings;

    void bind(eKeyAction action, std::initializer_list<SDL_Scancode> keys,
              bool requireCtrl = false, bool requireAlt = false, bool requireShift = false);

    static s_KeyBinding parseBinding(const std::string &value);
    static const std::vector<std::pair<std::string, eKeyAction>>& getActionTable();
    void checkForClashes() const;
};
