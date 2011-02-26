/*
 * cMouseUtils.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */
#include "../include/d2tmh.h"

cMouseUtils::cMouseUtils() {
	mouse = cMouse::getInstance();
}

cMouseUtils::cMouseUtils(cMouse *theMouse) {
	assert(theMouse);
	mouse = theMouse;
}

cMouseUtils::~cMouseUtils() {
}

bool cMouseUtils::isMouseOverRectangle(int x, int y, int width, int height) {
	return (mouse->getX() >= x && mouse->getX() <= (x + width)) && (mouse->getY() >= y && mouse->getY() <= (y + height));
}
