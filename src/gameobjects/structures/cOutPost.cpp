/**
 * @file cOutPost.cpp
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

#include "cOutPost.h"

#include "definitions.h"

cOutPost::cOutPost()
{
    // other variables (class specific)
}

int cOutPost::getType() const
{
    return RADAR;
}

void cOutPost::thinkFast()
{
    // last but not least, think like our abstraction
    cAbstractStructure::thinkFast();

    // this is not really flag animation anymore, but just base animation (ie, turning around of
    // dish on outpost)
    TIMER_flag++;

    if (TIMER_flag > 54) {
        iFrame++;

        // switch between 0 and 7.
        if (iFrame > 7) {
            iFrame=0;
        }

        TIMER_flag=0;
    }
}

void cOutPost::think_animation()
{
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}


void cOutPost::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
