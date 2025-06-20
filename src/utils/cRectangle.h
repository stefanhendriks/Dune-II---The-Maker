#pragma once

#include "cPoint.h"

class cRectangle {
public:
    cRectangle();

    cRectangle(int x, int y, int width, int height) : topLeft(cPoint(x,y)), width(width), height(height) {
    }

    cRectangle(cPoint &coord, cPoint &dimensions) : topLeft(cPoint(coord.x, coord.y)), width(dimensions.y), height(dimensions.x) {
    }

    static bool isWithin(int pointX, int pointY, int x, int y, int width, int height) {
        return (pointX >= x && pointX <= (x + width)) &&
               (pointY >= y && pointY <= (y + height));
    }

    bool isOverlapping(const cRectangle *other) const;

    bool isOverlapping(const cRectangle &other) const;

    bool isPointWithin(int x, int y) const;

    bool isPointWithin(const cPoint &point);

    bool isPointWithin(const cPoint *point) const;

    cPoint &getTopLeft() {
        return topLeft;
    }

    int getX() const {
        return topLeft.x;
    }
    int getEndX() const {
        return topLeft.x + width;
    }
    int getY() const {
        return topLeft.y;
    }
    int getEndY() const {
        return topLeft.y + height;
    }
    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }

    /**
     * Moves rectangle around, updates x and y coordinate.
     *
     * @param newX
     * @param newY
     */
    void move(int newX, int newY) {
        topLeft.x = newX;
        topLeft.y = newY;
    }

    void resize(int newWidth, int newHeight) {
        this->width = newWidth;
        this->height = newHeight;
    }

private:
    cPoint topLeft;
    int width;
    int height;

};
