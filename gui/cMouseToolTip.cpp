/*
 * cMouseToolTip.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMouseToolTip::cMouseToolTip(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

cMouseToolTip::~cMouseToolTip() {
}
