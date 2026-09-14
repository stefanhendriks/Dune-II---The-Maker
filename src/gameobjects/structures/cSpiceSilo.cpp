/**
 * @file cSpiceSilo.cpp
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

#include "cSpiceSilo.h"
#include "context/cInfoContext.h"
#include "definitions.h"

cSpiceSilo::cSpiceSilo()
{
    // other variables (class specific)

}

int cSpiceSilo::getType() const
{
    return SILO;
}


void cSpiceSilo::thinkFast()
{
    // think like base class
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cSpiceSilo::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cSpiceSilo::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cSpiceSilo::getSpiceSiloCapacity()
{
    float percentage = ((float) getHitPoints() / (float) m_info->getStructureInfo(getType()).hp);
    return 1000 * percentage;
}
