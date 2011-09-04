/*
 * Rectangle.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cGuiShape::cGuiShape(cRectangle * rect) {
	assert(rect);
	rectangle = rect;
	rectangleOwner = false;
	setColors(makecol(32, 32, 32), makecol(255,255,255), makecol(128,128,128));
}

cGuiShape::cGuiShape(int x, int y, int width, int height) {
	int x2 = (x + width);
	int y2 = (y + height);
	rectangle = new cRectangle(x, y, x2, y2);
	rectangleOwner = true;
	setColors(makecol(32, 32, 32), makecol(255,255,255), makecol(128,128,128));
}


cGuiShape::~cGuiShape() {
	// this boolean holds if we (this class) created an instance, and thus is responsible
	// for deleting it. In other cases, the cRectangle is created outside this class
	// and the responsibility to delete it lays also outside this class!
	if (rectangleOwner) {
		if (rectangle) {
			delete rectangle;
		}
		rectangle = NULL;
	}
}

bool cGuiShape::isMouseOverShape() {
	cMouse * mouse = cMouse::getInstance();
	return mouse->isOverRectangle(rectangle);
}

void cGuiShape::setColors(int darkBorder, int lightBorder, int inner) {
	darkBorderColor = darkBorder;
	lightBorderColor = lightBorder;
	innerColor = inner;
}
