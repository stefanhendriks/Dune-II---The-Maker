/*
 * cMouse.h
 *
 *  Created on: 24-okt-2010
 *      Author: Stefan
 */

#ifndef CMOUSE_H_
#define CMOUSE_H_

#include <managers/cInteractionManager.h>

class cRectangle;
class cInteractionManager; // some kind of observer thingy

class cMouse {

public:
	cMouse();
	~cMouse();

	void updateState(); // updates state from Allegro, calls appropiate on* methods on gameControlContext class

	void setInteractionManager(cInteractionManager *pInteractionManager) {
	    this->interactionManager = pInteractionManager;
	}

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

    bool isOverRectangle(int x, int y, int width, int height);

	bool isOverRectangle(cRectangle *rectangle);

    bool isMapScrolling();

    bool isBoxSelecting();

private:
    cInteractionManager * interactionManager;

    bool leftButtonPressed;
	bool rightButtonPressed;

	bool leftButtonClicked;
	bool rightButtonClicked;

	bool leftButtonPressedInPreviousFrame;
	bool rightButtonPressedInPreviousFrame;

	bool mouseScrolledUp;
	bool mouseScrolledDown;

	int x, y, z;	// z = scroll wheel value
	int zValuePreviousFrame;
};

#endif /* CMOUSE_H_ */
