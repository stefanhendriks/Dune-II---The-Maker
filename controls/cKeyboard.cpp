#include "../include/d2tmh.h"
#include "cKeyboard.h"


cKeyboard::cKeyboard() {
    memset(keyPressed, 0, sizeof(keyPressed));
    _keyboardObserver = nullptr;
}

cKeyboard::~cKeyboard() {
}

void cKeyboard::updateState() {
    // this is the naive way, but Allegro 4 also knows "keypressed()" and "readkey()". So lets try that later.
    for (int k = KEY_A; k < KEY_MAX; k++) {
        if (key[k]) {
            keyPressed[k] = true;
            s_KeyboardEvent event {
                    eKeyboardEventType::KEY_HOLD,
                    k,
            };

            _keyboardObserver->onNotifyKeyboardEvent(event);
        } else {
            bool wasPressed = keyPressed[k];
            keyPressed[k] = false;

            if (wasPressed) {
                // it was hold, no
                s_KeyboardEvent event{
                        eKeyboardEventType::KEY_PRESSED,
                        k,
                };

                _keyboardObserver->onNotifyKeyboardEvent(event);
            }
        }
    }
}
