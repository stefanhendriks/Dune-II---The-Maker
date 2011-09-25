/*
 * cMessageBar.cpp
 *
 *  Created on: 9-nov-2010
 *      Author: Stefan
 */
#include "../include/d2tmh.h"

cMessageBar::cMessageBar(int theX, int theY, int theWidth) {
	x = theX;
	y = theY;
	width = theWidth;
	alpha = 255;
	resetTimerMessage();
}

cMessageBar::~cMessageBar() {
}


