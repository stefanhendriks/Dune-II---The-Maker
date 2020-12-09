//
// Created by shendriks on 9/3/2020.
//
#include <controls/cMouse.h>

#include "cRectangle.h"

cRectangle::~cRectangle() {

}

/**
 * scans each edge of the rectangle if it is within the other rectangle, returns true if so, or false if not.
 * @param other
 * @return
 */
bool cRectangle::isOverlapping(cRectangle *other) {
    if (other == nullptr) return false;

    if (other->isWithin(getX(), getY())) return true;
    if (other->isWithin(getEndX(), getY())) return true;
    if (other->isWithin(getX(), getEndY())) return true;
    if (other->isWithin(getEndX(), getEndY())) return true;

    return false;
}

bool cRectangle::isMouseOver() {
    return cMouse::isOverRectangle(this->x, this->y, this->width, this->height);
}