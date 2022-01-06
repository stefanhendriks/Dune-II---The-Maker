#include "cKeyboard.h"
#include "cKeyboardEvent.h"

#include <algorithm>
#include "../include/d2tmh.h"


cKeyboard::cKeyboard() :
    _keyboardObserver(nullptr),
    keysPressed() {

}

cKeyboard::~cKeyboard() {
}

void cKeyboard::updateState() {
    // Stefan: I made this more 'efficient', and I tried to differentiate between a HOLD and a "Released" state
    // which makes sense. However, the way the events are being sent needs to be done differently (not now, I am tired)
    //
    // so this is for future me:
    // make sure you send an event, 1 for "HOLD" and 1 for "Released". But, send the list of all key scancodes within
    // that event (and not for every keypress 1 event).
    //
    // So that you can find combinations of keypresses. Ie CTRL-1 for create group...
    //

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

    _keyboardObserver->onNotifyKeyboardEvent(cKeyboardEvent(eKeyEventType::HOLD, newKeysPressed));

    std::set<int> keysReleased;
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
}
