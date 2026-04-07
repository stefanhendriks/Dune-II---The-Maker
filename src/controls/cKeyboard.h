#pragma once

#include "controls/eKeyboardEnum.h"
#include "observers/cInputObserver.h"
#include <SDL2/SDL.h>
#include <set>

class cKeyBindings;

class cKeyboard {
public:
    cKeyboard();
    void handleEvent(const SDL_Event &event);
    void updateState();

    void setKeyboardObserver(cInputObserver *keyboardObserver) {
        m_keyboardObserver = keyboardObserver;
    }

    void setKeyBindings(const cKeyBindings *keyBindings) {
        m_keyBindings = keyBindings;
    }

private:
    cInputObserver *m_keyboardObserver;
    const cKeyBindings *m_keyBindings = nullptr;
    std::set<SDL_Scancode> m_keysPressed;
    std::set<SDL_Scancode> m_keysReleased;
    s_KeysCombo m_currentCombo;
};
