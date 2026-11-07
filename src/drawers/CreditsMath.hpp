/**
 * @file CreditsMath.hpp
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

#pragma once

#include "data/gfxdata.h"

// Pure (SDL free) logic of the CreditsDrawer, so it can be unit tested.
namespace CreditsMath {

// sprite index of a digit character, CREDITS_NONE for anything else
inline int getCreditDrawId(char c)
{
    if (c >= '0' && c <= '9') return CREDITS_0 + (c - '0');
    return CREDITS_NONE;
}

// index of the first of the 6 digit slots that is drawn, so the number is right aligned
inline int getXDrawingOffset(int amount)
{
    int offset = 1;
    if (amount > 0)     offset = 4;
    if (amount < 10)    offset = 5;
    if (amount > 99)    offset = 3;
    if (amount > 999)   offset = 2;
    if (amount > 9999)  offset = 1;
    if (amount > 99999) offset = 0;
    return offset;
}

// The displayed credits step towards the target: big differences are sliced into pieces
// for a more fluent transition.
inline int nextDisplayedCredits(int displayed, int target)
{
    if (target == displayed) return displayed;
    int diff = target - displayed;
    if (diff > 200) return displayed + (diff / 8);
    if (diff > 20) return displayed + (diff / 4);
    return target;
}

}
