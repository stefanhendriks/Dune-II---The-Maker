#include "../include/d2tmh.h"

MessageBar::MessageBar(int theX, int theY, int theWidth) {
	x = theX;
	y = theY;
	width = theWidth;
	alpha = 255;
	resetTimerMessage();
}

MessageBar::~MessageBar() {
}


