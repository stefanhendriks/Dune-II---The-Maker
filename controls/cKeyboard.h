#ifndef CKEYBOARD_H_
#define CKEYBOARD_H_

class cKeyboard {
	public:
		cKeyboard();
		~cKeyboard();

        void updateState(); // updates state from Allegro, calls appropriate on* methods on gameControlContext class

        void setKeyboardObserver(cInputObserver *keyboardObserver) {
            this->_keyboardObserver = keyboardObserver;
        }
	protected:
        bool keyPressed[KEY_MAX]; // is it pressed right now?

	private:
        cInputObserver *_keyboardObserver;
};

#endif /* CKEYBOARD_H_ */
