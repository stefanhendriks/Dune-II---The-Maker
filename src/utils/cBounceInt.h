#pragma once

#include <algorithm>

// This class is responsible for an internal value that oscillates between A and B, bouncing back when it reaches the bounds.

class cBounceInt {
public:
    // constructor take min and max value for oscillation
    explicit cBounceInt(int minVal, int maxVal);
    // update oscillator from 1
    void update();
    // return current value
    int get() const;

private:
    int minVal;
    int maxVal;
    int value;
    int direction; // +1 or -1
};
