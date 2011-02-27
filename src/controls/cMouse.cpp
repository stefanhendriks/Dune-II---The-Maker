/*
 * cMouse.cpp
 *
 *  Created on: 24-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMouse *cMouse::instance = NULL;

cMouse::cMouse() {
	x = y = z = 0;
	leftButtonPressed = false;
	rightButtonPressed = false;
	leftButtonPressedInPreviousFrame = false;
	rightButtonPressedInPreviousFrame = false;
	leftButtonClicked = false;
	rightButtonClicked = false;
	mouseScrolledUp = false;
	mouseScrolledDown = false;
	zValuePreviousFrame = mouse_z;
	tile = MOUSE_NORMAL;
}

cMouse::~cMouse() {
}

cMouse *cMouse::getInstance() {
	if (instance == NULL) {
		instance = new cMouse();
	}

	return instance;
}

void cMouse::updateState() {
	x = mouse_x;
	y = mouse_y;
	z = mouse_z;

	// check if leftButtonIsPressed=true (which is the previous frame)
	leftButtonPressedInPreviousFrame = leftButtonPressed;
	rightButtonPressedInPreviousFrame = rightButtonPressed;

	leftButtonPressed = mouse_b & 1;
	rightButtonPressed = mouse_b & 2;

	// now check if the leftButtonPressed == false, but the previous frame was true (if so, it is
	// counted as a click)
	leftButtonClicked = (leftButtonPressedInPreviousFrame == true && leftButtonPressed == false);
	rightButtonClicked = (rightButtonPressedInPreviousFrame == true && rightButtonPressed == false);

	mouseScrolledUp = mouseScrolledDown = false;

	if (z > zValuePreviousFrame) {
		mouseScrolledUp = true;
	}

	if (z < zValuePreviousFrame) {
		mouseScrolledDown = true;
	}

	zValuePreviousFrame = z;

	// cap mouse z
	if (z > 10 || z < -10) {
		z = 0;
		position_mouse_z(0);
	}
}

void cMouse::positionMouseCursor(int x, int y) {
	position_mouse(x, y);
}

