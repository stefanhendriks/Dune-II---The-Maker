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

    if (other->isPointWithin(getX(), getY())) return true;
    if (other->isPointWithin(getEndX(), getY())) return true;
    if (other->isPointWithin(getX(), getEndY())) return true;
    if (other->isPointWithin(getEndX(), getEndY())) return true;

    return false;
}

bool cRectangle::isPointWithin(int pointX, int pointY) {
    return (pointX >= x && pointX <= (x + width)) &&
           (pointY >= y && pointY <= (y + height));
}

bool cRectangle::isPointWithin(const cPoint &point) {
    return (point.x >= x && point.x <= (x + width)) &&
           (point.y >= y && point.y <= (y + height));
}

cRectangle::cRectangle() : cRectangle(0,0,0,0){
}
