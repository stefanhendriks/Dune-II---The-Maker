#include "Rectangle.h"

Rectangle::Rectangle() {
	startX = startY = endX = endY = -1;
}

Rectangle::Rectangle(int startX, int startY, int endX, int endY) {
	setStartX(startX);
	setStartY(startY);
	setEndX(endX);
	setEndY(endY);
}

// copy constructor
Rectangle::Rectangle(Rectangle * other) {
	assert(other);
	this->startX = other->getStartX();
	this->startY = other->getStartY();
	this->endX = other->getEndX();
	this->endY = other->getEndY();
}

Rectangle::~Rectangle() {
	startX = startY = endX = endY = -1;
}

void Rectangle::setCoordinates(Rectangle * otherRectangle) {
	if (otherRectangle) {
		startX = otherRectangle->getStartX();
		startY = otherRectangle->getStartY();
		endX = otherRectangle->getEndX();
		endY = otherRectangle->getEndY();
	}
}

bool Rectangle::isCoordinateWithinRectangle(int x, int y) {
	return ((x >= getLowestX() && x <= getHighestX()) && (y >= getLowestY() && y <= getHighestY()));
}
