/*
 * cMouse.h
 *
 *  Created on: 24-okt-2010
 *      Author: Stefan
 */

#ifndef CMOUSE_H_
#define CMOUSE_H_

class cMouse {

public:
	cMouse();

	static void updateState(); // updates state from Allegro

	// these functions return true when the mouse button is being hold down
//	bool isLeftButtonPressed() { return leftButtonPressed; }
//	bool isRightButtonPressed() { return rightButtonPressed; }

	static bool isLeftButtonPressed() { return getInstance()->leftButtonPressed; }
	static bool isRightButtonPressed() { return getInstance()->rightButtonPressed; }

	// these functions return true when the mouse button was pressed in the previous
	// frame, but released in the current (which is counted as a 'click')
//	bool isLeftButtonClicked() { return leftButtonClicked; }
//	bool isRightButtonClicked() { return rightButtonClicked; }
	static bool isLeftButtonClicked() { return getInstance()->leftButtonClicked; }
	static bool isRightButtonClicked() { return getInstance()->rightButtonClicked; }

	static bool isMouseScrolledUp() { return getInstance()->mouseScrolledUp; }
	static bool isMouseScrolledDown() { return getInstance()->mouseScrolledDown; }

	static int getX() { return getInstance()->x; }
	static int getY() { return getInstance()->y; }
	static int getZ() { return getInstance()->z; }

	static void positionMouseCursor(int x, int y);

	static void init() {
	    getInstance(); // this will create a new instance
	}

    static bool isOverRectangle(int x, int y, int width, int height);

    static bool isMapScrolling();

    static bool isBoxSelecting();

protected:
	~cMouse();

private:
    static cMouse *getInstance();

	bool leftButtonPressed;
	bool rightButtonPressed;

	bool leftButtonClicked;
	bool rightButtonClicked;

	bool leftButtonPressedInPreviousFrame;
	bool rightButtonPressedInPreviousFrame;

	bool mouseScrolledUp;
	bool mouseScrolledDown;

	static cMouse *instance;

	int x, y, z;	// z = scroll wheel value
	int zValuePreviousFrame;
};

#endif /* CMOUSE_H_ */
