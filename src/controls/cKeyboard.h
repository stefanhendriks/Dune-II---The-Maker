#pragma once

#include "controls/cKeyBindings.h"
#include "controls/eKeyboardEnum.h"
#include "observers/cInputObserver.h"
#include <SDL2/SDL.h>
#include <set>

class cSection;

class cKeyboard {
public:
    cKeyboard();
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
    cKeyBindings m_keyBindings;
    std::set<SDL_Scancode> m_keysPressed;
    std::set<SDL_Scancode> m_keysReleased;
    s_KeysCombo m_currentCombo;
};
