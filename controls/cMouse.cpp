/*
 * cMouse.cpp
 *
 *  Created on: 24-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMouse *cMouse::instance = NULL;

cMouse::cMouse() {
	x=y=z=0;
	leftButtonPressed=false;
	rightButtonPressed=false;
	leftButtonPressedInPreviousFrame=false;
	rightButtonPressedInPreviousFrame=false;
	leftButtonClicked=false;
	rightButtonClicked=false;
	mouseScrolledUp=false;
	mouseScrolledDown=false;
	zValuePreviousFrame = mouse_z;
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
    cMouse * mouse = getInstance();
    mouse->x = mouse_x;
    mouse->y = mouse_y;
    mouse->z = mouse_z;

	// check if leftButtonIsPressed=true (which is the previous frame)
    mouse->leftButtonPressedInPreviousFrame = mouse->leftButtonPressed;
    mouse->rightButtonPressedInPreviousFrame = mouse->rightButtonPressed;

    mouse->leftButtonPressed = mouse_b & 1;
    mouse->rightButtonPressed = mouse_b & 2;

	// now check if the leftButtonPressed == false, but the previous frame was true (if so, it is
	// counted as a click)
    mouse->leftButtonClicked = (mouse->leftButtonPressedInPreviousFrame == true && mouse->leftButtonPressed == false);
    mouse->rightButtonClicked = (mouse->rightButtonPressedInPreviousFrame == true && mouse->rightButtonPressed == false);

    mouse->mouseScrolledUp = mouse->mouseScrolledDown = false;

	if (mouse->z > mouse->zValuePreviousFrame) {
        mouse->mouseScrolledUp = true;
	}

	if (mouse->z < mouse->zValuePreviousFrame) {
        mouse->mouseScrolledDown = true;
	}

    mouse->zValuePreviousFrame = mouse->z;

	// cap mouse z
	if (mouse->z > 10 || mouse->z < -10) {
        mouse->z = 0;
		position_mouse_z(0); // allegro function
	}
}

void cMouse::positionMouseCursor(int x, int y) {
	position_mouse(x, y); // allegro function
}

bool cMouse::isOverRectangle(int x, int y, int width, int height) {
    return cRectangle::isWithin(cMouse::getX(), cMouse::getY(), x, y, width, height);
}