#pragma once

#include "observers/cInputObserver.h"

#include <set>

class cKeyboard {
	public:
		cKeyboard();

        void updateState(); // updates state from Allegro, calls appropriate on* methods on gameControlContext class

        void setKeyboardObserver(cInputObserver *keyboardObserver) {
            _keyboardObserver = keyboardObserver;
        }

	private:
        cInputObserver *_keyboardObserver;
        std::set<int> keysPressed;
};
