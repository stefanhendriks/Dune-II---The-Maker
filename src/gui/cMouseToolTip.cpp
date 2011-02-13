/*
 * cMouseToolTip.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMouseToolTip::cMouseToolTip(cPlayer *thePlayer, cMouse *theMouse) {
	assert(thePlayer);
	assert(theMouse);
	player = thePlayer;
	mouse = theMouse;
}

cMouseToolTip::~cMouseToolTip() {
}
