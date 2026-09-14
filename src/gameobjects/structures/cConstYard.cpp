/**
 * @file cConstYard.cpp
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

#include "cConstYard.h"

#include "definitions.h"

cConstYard::cConstYard()
{
    frames = 0;
}

int cConstYard::getType() const
{
    return CONSTYARD;
}

void cConstYard::thinkFast()
{
    cAbstractStructure::thinkFast();
}

void cConstYard::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cConstYard::think_guard()
{
// do nothing
}