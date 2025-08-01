/*
 * cMouseToolTip.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "MouseToolTip.h"

#include <cassert>

cMouseToolTip::cMouseToolTip(cPlayer *thePlayer)
{
    assert(thePlayer);
    player = thePlayer;
}

cMouseToolTip::~cMouseToolTip()
{
}
