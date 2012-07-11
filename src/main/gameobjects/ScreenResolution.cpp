#include "../include/d2tmh.h"

ScreenResolution::ScreenResolution(int theWidth, int theHeight) {
	width = theWidth;
	height = theHeight;
	tested = false;
	canBeUsed = false;
}

ScreenResolution::ScreenResolution() {
	tested = false;
	canBeUsed = false;
	width = -1;
	height = -1;
}

ScreenResolution::~ScreenResolution() {
}

void ScreenResolution::printLog() {
	char msg[255];
	sprintf(msg, "cBestScreenResolutionFinder:: width = %d, height = %d, tested = %d, usable = %d", width, height, tested, canBeUsed);
	logbook(msg);
}
