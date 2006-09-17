/**
	EventManager

	Responsible for input from keyboard, mouse clicks, etc.

*/

class cEventManager {

private:


	void pollMouse();
	void pollKeyboard();

public:
	cEventManager();

	void handleEvents();
};
