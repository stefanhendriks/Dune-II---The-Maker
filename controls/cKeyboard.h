#ifndef CKEYBOARD_H_
#define CKEYBOARD_H_

#include "observers/cInputObserver.h"
#include <set>

class cKeyboard {
	public:
		cKeyboard();
		~cKeyboard();

        void updateState(); // updates state from Allegro, calls appropriate on* methods on gameControlContext class

        void setKeyboardObserver(cInputObserver *keyboardObserver) {
            this->_keyboardObserver = keyboardObserver;
        }

	private:
        cInputObserver *_keyboardObserver;
        std::set<int> keysPressed;
};

#endif /* CKEYBOARD_H_ */
