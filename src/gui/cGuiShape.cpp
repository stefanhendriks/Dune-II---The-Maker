/*
 * Rectangle.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cGuiShape::cGuiShape() {
	cGuiShape(0, 0, 100, 100);
}

cGuiShape::~cGuiShape() {
}

cGuiShape::cGuiShape(int theX, int theY, int theHeight, int theWidth) {
	x = theX;
	y = theY;
	height = theHeight;
	width = theWidth;
}

bool cGuiShape::isMouseOverShape() {
	cMouse * mouse = cMouse::getInstance();
	int mouseX = mouse->getX();
	int mouseY = mouse->getY();

	return (
			(mouseX >= x && mouseX <= (x + width)) &&
			(mouseY >= y && mouseY <= (y + height))
			);
}
