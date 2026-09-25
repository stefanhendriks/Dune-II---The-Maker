/**
 * @file cBarracks.cpp
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

#include "cBarracks.h"

#include "definitions.h"

cBarracks::cBarracks()
{
}

void cBarracks::thinkFast()
{
    cAbstractStructure::thinkFast();
}

int cBarracks::getType() const
{
    return BARRACKS;
}

void cBarracks::think_animation()
{
    // a barracks does not animate, so when set, set it back to false
    if (isAnimating()) {
        setAnimating(false);
    }
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cBarracks::think_guard()
{
// do nothing
}
