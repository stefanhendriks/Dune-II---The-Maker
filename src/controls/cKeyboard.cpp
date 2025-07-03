#include "cKeyboard.h"
#include "cKeyboardEvent.h"

#include <algorithm>

cKeyboard::cKeyboard() :
    _keyboardObserver(nullptr),
    keysPressed()
{

}

void cKeyboard::handleEvent(const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        keysPressed.insert(event.key.keysym.scancode);
    }
    else if (event.type == SDL_KEYUP) {
        keysPressed.erase(event.key.keysym.scancode);
        keysReleased.insert(event.key.keysym.scancode);
    }
}

void cKeyboard::updateState()
{
    if (!keysPressed.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, keysPressed));
    }
    if (!keysReleased.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, keysReleased));
        keysReleased.clear();
    }
}
