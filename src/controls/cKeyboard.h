#pragma once

#include "controls/eKeyboardEnum.h"
#include "observers/cInputObserver.h"
#include <SDL2/SDL.h>
#include <set>

class cKeyboard {
public:
    cKeyboard();
    void handleEvent(const SDL_Event &event);
    void updateState();

    void setKeyboardObserver(cInputObserver *keyboardObserver) {
        m_keyboardObserver = keyboardObserver;
    }

private:
    cInputObserver *m_keyboardObserver;
    std::set<SDL_Scancode> m_keysPressed;
    std::set<SDL_Scancode> m_keysReleased;
    s_KeysCombo m_currentCombo;
};
