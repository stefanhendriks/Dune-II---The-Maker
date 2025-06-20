#pragma once

#include "observers/cInputObserver.h"
#include <SDL2/SDL.h>
#include <set>

class cKeyboard {
public:
    cKeyboard();
    void handleEvent(const SDL_Event& event);
    void updateState(); // updates state from Allegro, calls appropriate on* methods on gameControlContext class

    void setKeyboardObserver(cInputObserver *keyboardObserver) {
        _keyboardObserver = keyboardObserver;
    }

private:
    cInputObserver *_keyboardObserver;
    std::set<int> keysPressed;
    std::set<int> keysReleased;
};
