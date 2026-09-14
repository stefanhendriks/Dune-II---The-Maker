/**
 * @file cPalace.cpp
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

#include "cPalace.h"

#include "definitions.h"

cPalace::cPalace()
{
    // other variables (class specific)

}

int cPalace::getType() const
{
    return PALACE;
}

void cPalace::thinkFast()
{
    cAbstractStructure::thinkFast();
}

void cPalace::think_animation()
{
    // a palace does not animate, so when set, set it back to false
    if (isAnimating()) {
        setAnimating(false);
    }
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cPalace::think_guard()
{

}
