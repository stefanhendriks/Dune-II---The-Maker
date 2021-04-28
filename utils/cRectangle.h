//
// Created by shendriks on 9/3/2020.
//

#ifndef D2TM_CRECTANGLE_H
#define D2TM_CRECTANGLE_H


class cRectangle {
public:
    cRectangle(int x, int y, int width, int height) : x(x), y(y), height(height), width(width) {
    }

    ~cRectangle();

    bool isWithin(int pointX, int pointY) {
        return (pointX >= x && pointX <= (x + width)) &&
               (pointY >= y && pointY <= (y + height));
    }

    static bool isWithin(int pointX, int pointY, int x, int y, int width, int height) {
        return (pointX >= x && pointX <= (x + width)) &&
               (pointY >= y && pointY <= (y + height));
    }

    bool isOverlapping(cRectangle *other);

    bool isMouseOver(int mouseX, int mouseY);

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
