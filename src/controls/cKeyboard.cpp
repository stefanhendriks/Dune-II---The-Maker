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
    }
    else if (event.type == SDL_KEYUP) {
        keysPressed.erase(scancode);
        keysReleased.insert(scancode);
    }

    // Read physical key state directly - more reliable on macOS
    // where the Option key may not generate KEYDOWN events due to OS-level interception
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    currentCombo.altPressed = keyState[SDL_SCANCODE_LALT] || keyState[SDL_SCANCODE_RALT];
    currentCombo.ctrlPressed = keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL];
    currentCombo.shiftPressed = keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT];
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
