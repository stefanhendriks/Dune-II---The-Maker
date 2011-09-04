/*
 * cRectangle.cpp
 *
 *  Created on: 22-aug-2011
 *      Author: Stefan
 */

#include "cRectangle.h"

cRectangle::cRectangle() {
	startX = startY = endX = endY = -1;
}

cRectangle::cRectangle(int startX, int startY, int endX, int endY) {
	setStartX(startX);
	setStartY(startY);
	setEndX(endX);
	setEndY(endY);
}

cRectangle::~cRectangle() {
	startX = startY = endX = endY = -1;
}

void cRectangle::setCoordinates(cRectangle * otherRectangle) {
	if (otherRectangle) {
		startX = otherRectangle->getStartX();
		startY = otherRectangle->getStartY();
		endX = otherRectangle->getEndX();
		endY = otherRectangle->getEndY();
	}
}
