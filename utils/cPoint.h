#ifndef D2TM_CPOINT_H
#define D2TM_CPOINT_H

/**
 * A point in 2D space (vector 2d-ish)
 */
class cPoint {
public:
    cPoint(int x, int y) : x(x), y(y) {};

    int x;
    int y;
};


#endif //D2TM_CPOINT_H
