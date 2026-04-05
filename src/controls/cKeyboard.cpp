#include "cKeyboard.h"
#include "cKeyboardEvent.h"

#include <algorithm>

cKeyboard::cKeyboard() :
    _keyboardObserver(nullptr),
    keysPressed(),
    currentCombo()
{
}

void cKeyboard::handleEvent(const SDL_Event &event)
{
    SDL_Scancode scancode = event.key.keysym.scancode;
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        keysPressed.insert(scancode);

        // Update combo
        if (scancode == SDL_SCANCODE_LALT || scancode == SDL_SCANCODE_RALT) {
            currentCombo.altPressed = true;
        } else if (scancode == SDL_SCANCODE_LCTRL || scancode == SDL_SCANCODE_RCTRL) {
            currentCombo.ctrlPressed = true;
        } else if (scancode == SDL_SCANCODE_LSHIFT || scancode == SDL_SCANCODE_RSHIFT) {
            currentCombo.shiftPressed = true;
        }
    }
    else if (event.type == SDL_KEYUP) {
        keysPressed.erase(scancode);
        keysReleased.insert(scancode);

        // Update combo
        if (scancode == SDL_SCANCODE_LALT || scancode == SDL_SCANCODE_RALT) {
            currentCombo.altPressed = false;
        } else if (scancode == SDL_SCANCODE_LCTRL || scancode == SDL_SCANCODE_RCTRL) {
            currentCombo.ctrlPressed = false;
        } else if (scancode == SDL_SCANCODE_LSHIFT || scancode == SDL_SCANCODE_RSHIFT) {
            currentCombo.shiftPressed = false;
        }
    }
}

void cKeyboard::updateState()
{
    if (!keysPressed.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, keysPressed, currentCombo));
    }
    if (!keysReleased.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, keysReleased, currentCombo));
        keysReleased.clear();
    }
}
