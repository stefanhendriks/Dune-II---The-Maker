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
	mouseDraggingRectangle = false;
	zValuePreviousFrame = mouse_z;
	tile = MOUSE_NORMAL;
	currentRectangle.resetCoordinates();
	lastCreatedRectangle.resetCoordinates();
}

cMouse::~cMouse() {
}

cMouse *cMouse::getInstance() {
	if (instance == NULL) {
		instance = new cMouse();
	}

	return instance;
}

void cMouse::doRectangleLogic() {

    if(leftButtonPressed){

        if(currentRectangle.getStartX() < 0 || currentRectangle.getStartY() < 0){
            currentRectangle.setStartX(x);
            currentRectangle.setStartY(y);
        } else {
            if(abs(x - currentRectangle.getStartX()) > 4 && abs(y - currentRectangle.getStartY()) > 4){
                currentRectangle.setEndX(x);
                currentRectangle.setEndY(y);
            	mouseDraggingRectangle = true;
            }
        }

    }
    else{
        if(currentRectangle.hasValidCoordinates()){
            lastCreatedRectangle.setCoordinates(&currentRectangle);
            mouseDraggingRectangle = false;
        }
        currentRectangle.resetCoordinates();
    }

}

void cMouse::updateState() {
	x = mouse_x;
	y = mouse_y;
	z = mouse_z;

	leftButtonPressedInPreviousFrame = leftButtonPressed;
	rightButtonPressedInPreviousFrame = rightButtonPressed;

	leftButtonPressed = mouse_b & 1;
	rightButtonPressed = mouse_b & 2;

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
	if (z > 10 || z < -10) {
		z = 0;
		position_mouse_z(0);
	}

	doRectangleLogic();
}

bool cMouse::isOverRectangleUsingWidthAndHeight(int rectX, int rectY, int width, int height) {
	return isOverRectangleUsingCoordinates(rectX, rectY, (rectX + width), (rectY + height));
}

bool cMouse::isOverRectangleUsingCoordinates(int rectX, int rectY, int rectEndX, int rectEndY) {
	return (x >= rectX && x <= rectEndX) && (y >= rectY && y <= rectEndY);
}

bool cMouse::isOverRectangle(cRectangle * rect) {
	if (!rect) {
		return false;
	}

	int rectX = rect->getLowestX();
	int rectY = rect->getLowestY();
	int rectEndX = rect->getHighestX();
	int rectEndY = rect->getHighestY();

	return isOverRectangleUsingCoordinates(rectX, rectY, rectEndX, rectEndY);
}


void cMouse::positionMouseCursor(int x, int y) {
	position_mouse(x, y);
}

