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

	static cMouse *getInstance();

	void updateState(); // updates state from Allegro

	// these functions return true when the mouse button is being hold down
	bool isLeftButtonPressed() { return leftButtonPressed; }
	bool isRightButtonPressed() { return rightButtonPressed; }

	// these functions return true when the mouse button was pressed in the previous
	// frame, but released in the current (which is counted as a 'click')
	bool isLeftButtonClicked() { return leftButtonClicked; }
	bool isRightButtonClicked() { return rightButtonClicked; }

	int getX() { return x; }
	int getY() { return y; }
	int getZ() { return z; }

	void positionMouseCursor(int x, int y);

protected:
	~cMouse();

private:

	bool leftButtonPressed;
	bool rightButtonPressed;

	bool leftButtonClicked;
	bool rightButtonClicked;

	bool leftButtonPressedInPreviousFrame;
	bool rightButtonPressedInPreviousFrame;

	static cMouse *instance;

	int x, y, z;	// z = scroll wheel value
};

#endif /* CMOUSE_H_ */
