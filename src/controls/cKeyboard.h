/**
 * @file cKeyboard.h
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

#include "controls/eKeyboardEnum.h"
#include "observers/cInputObserver.h"
#include <SDL3/SDL.h>
#include <bitset>
#include <memory>
#include <vector>

class cKeyBindings;
class cSection;

class cKeyboard {
public:
    cKeyboard();
    ~cKeyboard();
    void handleEvent(const SDL_Event &event);
    void updateState();

    void setKeyboardObserver(cInputObserver *keyboardObserver) {
        m_keyboardObserver = keyboardObserver;
    }

    /**
     * Load default key bindings, then optionally override from an INI section.
     * Pass nullptr to use defaults only.
     */
    void loadKeyBindings(const cSection *section = nullptr);

private:
    cInputObserver *m_keyboardObserver;
    std::unique_ptr<cKeyBindings> m_keyBindings;
    std::bitset<SDL_SCANCODE_COUNT> m_keysPressed;
    std::bitset<SDL_SCANCODE_COUNT> m_keysReleased;
    std::vector<std::string> m_textInputs;
    s_KeysCombo m_currentCombo;
};
