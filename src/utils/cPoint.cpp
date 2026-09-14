/**
 * @file cPoint.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#include "cPoint.h"
#include "cRectangle.h"

bool cPoint::isWithinRectangle(const cRectangle *pRectangle) const
{
    if (pRectangle == nullptr) return false;
    return pRectangle->isPointWithin(this);
}

