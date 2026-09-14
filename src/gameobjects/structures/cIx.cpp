/**
 * @file cIx.cpp
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

#include "cIx.h"

#include "definitions.h"

// Constructor
cIx::cIx()
{
    // other variables (class specific)
}

int cIx::getType() const
{
    return IX;
}

void cIx::thinkFast()
{
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cIx::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cIx::think_guard()
{

}
