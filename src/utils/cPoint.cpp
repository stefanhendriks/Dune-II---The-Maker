#include "cPoint.h"
#include "cRectangle.h"

bool cPoint::isWithinRectangle(const cRectangle *pRectangle) const
{
    if (pRectangle == nullptr) return false;
    return pRectangle->isPointWithin(this);
};

