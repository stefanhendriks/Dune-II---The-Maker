#include "cKeyboard.h"
#include "cKeyboardEvent.h"

#include <algorithm>

cKeyboard::cKeyboard() :
    _keyboardObserver(nullptr),
    keysPressed()
{

}

void cKeyboard::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        keysPressed.insert(event.key.keysym.scancode);
    } else if (event.type == SDL_KEYUP) {
        keysPressed.erase(event.key.keysym.scancode);
        keysReleased.insert(event.key.keysym.scancode);
    }
}

void cKeyboard::updateState()
{
    if (!keysPressed.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, keysPressed));
        //keysPressed.clear();
    }
    if (!keysReleased.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, keysReleased));
        keysReleased.clear();
    }
    // Stefan: I made this more 'efficient', and I tried to differentiate between a HOLD and a "Released" state
    // which makes sense. However, the way the events are being sent needs to be done differently (not now, I am tired)
    //
    // so this is for future me:
    // make sure you send an event, 1 for "HOLD" and 1 for "Released". But, send the list of all key scancodes within
    // that event (and not for every keypress 1 event).
    //
    // So that you can find combinations of keypresses. Ie CTRL-1 for create group...
    //
/*
    std::set<int> newKeysPressed = std::set<int>();

    // capture all the pressed keys here, use a set so we don't capture multiple times the same key (which
    // can happen with readkey())
    while (keypressed()) {
        int theKeyPressed = readkey();
        int scanCode = (theKeyPressed >> 8);
        newKeysPressed.insert(scanCode);
    }

    // these keys are not detected as a separate keypress by allegro (by keypressed(), and hence not readkey()).
    // so check these via the key[] way.
    if (key[SDL_SCANCODE_RCTRL]) newKeysPressed.insert(SDL_SCANCODE_RCTRL);
    if (key[SDL_SCANCODE_LCTRL]) newKeysPressed.insert(SDL_SCANCODE_LCTRL);
    if (key[SDL_SCANCODE_RSHIFT]) newKeysPressed.insert(SDL_SCANCODE_RSHIFT);
    if (key[SDL_SCANCODE_LSHIFT]) newKeysPressed.insert(SDL_SCANCODE_LSHIFT);
    if (key[SDL_SCANCODE_TAB]) newKeysPressed.insert(SDL_SCANCODE_TAB);

    if (!newKeysPressed.empty()) {
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, newKeysPressed));
    }

    std::set<int> keysReleased;
    // Check if any keys are no longer pressed
    for (auto k : keysPressed) {
        // the 'old' key is still held down in the 'new' state, so ignore
        if (std::find(newKeysPressed.begin(), newKeysPressed.end(), k) != newKeysPressed.end()) {
            // still pressed (by keyboard event)
        }
        else if (key[k]) {
            // still pressed "down
            // but not in the 'newKeysPressed' set, so make sure to add it still, so we don't forget it being pressed
            newKeysPressed.insert(k);
        }
        else {
            // the 'old' key is not present in newKeysPressed, so it has been released.
            keysReleased.insert(k);
        }
    }

    if (!keysReleased.empty()) {
        // "keys released" mean a "pressed" event (ie down and then released).
        _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::PRESSED, keysReleased));
    }

    // finally, update the state
    keysPressed = newKeysPressed;
    */
}
