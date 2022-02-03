#pragma once
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

    struct split {
        split(int& x, int&y) : m_x(x), m_y(y) {}

        void operator=(cPoint p) {
            m_x = p.x;
            m_y = p.y;
        }

        int& m_x, m_y;
    };
};
