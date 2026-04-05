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
        _keyboardObserver = keyboardObserver;
    }

private:
    cInputObserver *_keyboardObserver;
    std::set<SDL_Scancode> keysPressed;
    std::set<SDL_Scancode> keysReleased;
    s_KeysCombo currentCombo;
};
