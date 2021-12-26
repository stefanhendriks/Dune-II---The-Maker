#include <algorithm>
#include "../include/d2tmh.h"
#include "cKeyboard.h"


cKeyboard::cKeyboard() :
    _keyboardObserver(nullptr),
    keysPressed() {

}

cKeyboard::~cKeyboard() {
}

void cKeyboard::updateState() {
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
    if (key[KEY_RCONTROL]) newKeysPressed.insert(KEY_RCONTROL);
    if (key[KEY_LCONTROL]) newKeysPressed.insert(KEY_LCONTROL);
    if (key[KEY_RSHIFT]) newKeysPressed.insert(KEY_RSHIFT);
    if (key[KEY_LSHIFT]) newKeysPressed.insert(KEY_LSHIFT);

    // now emit events
    for (auto k : newKeysPressed) {
        s_KeyboardEvent event {
                eKeyboardEventType::KEY_HOLD,
                k,
        };

        _keyboardObserver->onNotifyKeyboardEvent(event);
    }

    // Check if any keys are no longer pressed
    for (auto k : keysPressed) {
        // the 'old' key is still held down in the 'new' state, so ignore
        if (std::find(newKeysPressed.begin(), newKeysPressed.end(), k) != newKeysPressed.end()) {
            // still pressed (by keyboard event)
        } else if (key[k]) {
            // still pressed "down
            // but not in the 'newKeysPressed' set, so make sure to add it still, so we don't forget it being pressed
            newKeysPressed.insert(k);
        } else {
            // the 'old' key is not present in newKeysPressed, so it has been released. This means a 'pressed' event.
            s_KeyboardEvent event{
                    eKeyboardEventType::KEY_PRESSED,
                    k,
            };

            _keyboardObserver->onNotifyKeyboardEvent(event);
        }
    }

    // finally, update the state
    keysPressed = newKeysPressed;
}
