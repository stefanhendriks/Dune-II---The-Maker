#ifndef D2TM_CPOINT_H
#define D2TM_CPOINT_H

// forward declr
class cRectangle;

/**
 * A point in 2D space (vector 2d-ish)
 */
class cPoint {
public:
    cPoint() : cPoint(0,0) { }

    cPoint(int x, int y) : x(x), y(y) {}

    int x;
    int y;

    bool isWithinRectangle(const cRectangle *pRectangle) const;
};


#endif //D2TM_CPOINT_H
