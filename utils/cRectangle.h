#ifndef D2TM_CRECTANGLE_H
#define D2TM_CRECTANGLE_H

#include "cPoint.h"

class cRectangle {
public:
    cRectangle();

    cRectangle(int x, int y, int width, int height) : x(x), y(y), height(height), width(width) {
    }

    cRectangle(cPoint &coord, cPoint &dimensions) : x(coord.x), y(coord.y), height(dimensions.x), width(dimensions.y) {
    }

    ~cRectangle();

    static bool isWithin(int pointX, int pointY, int x, int y, int width, int height) {
        return (pointX >= x && pointX <= (x + width)) &&
               (pointY >= y && pointY <= (y + height));
    }

    bool isOverlapping(cRectangle *other);

    bool isPointWithin(int x, int y);

    bool isPointWithin(const cPoint &point);

    int getX() { return x; }
    int getEndX() { return x + width; }
    int getY() { return y; }
    int getEndY() { return y + height; }
    int getWidth() { return width; }
    int getHeight() { return height; }

    /**
     * Moves rectangle around, updates x and y coordinate.
     *
     * @param newX
     * @param newY
     */
    void move(int newX, int newY) {
        this->x = newX;
        this->y = newY;
    }

    void resize(int newWidth, int newHeight) {
        this->width = newWidth;
        this->height = newHeight;
    }

private:
    int x;
    int y;
    int width;
    int height;

};


#endif //D2TM_CRECTANGLE_H
