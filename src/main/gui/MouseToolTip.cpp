/*
 * cMouseToolTip.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

MouseToolTip::MouseToolTip(cPlayer *thePlayer, cMouse *theMouse) {
	assert(thePlayer);
	assert(theMouse);
	player = thePlayer;
	mouse = theMouse;
}

MouseToolTip::~MouseToolTip() {
	mouse = NULL;
	player = NULL;
}
