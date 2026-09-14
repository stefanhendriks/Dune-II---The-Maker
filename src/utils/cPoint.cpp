/**
 * @file cPoint.cpp
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
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

