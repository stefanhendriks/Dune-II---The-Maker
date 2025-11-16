#include "utils/cBounceInt.h"

cBounceInt::cBounceInt(int minVal, int maxVal)
    : minVal(minVal), maxVal(maxVal), value(minVal), direction(1)
{
    if (minVal > maxVal)
        std::swap(this->minVal, this->maxVal);
}

void cBounceInt::update()
{
    value += direction;
    if (value >= maxVal) {
        value = maxVal;
        direction = -1;
    } 
    else if (value <= minVal) {
        value = minVal;
        direction = +1;
    }
}

int cBounceInt::get() const
{
    return value;
}
