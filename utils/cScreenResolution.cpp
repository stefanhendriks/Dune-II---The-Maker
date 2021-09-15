#include "../include/d2tmh.h"

cScreenResolution::cScreenResolution(int theWidth, int theHeight) {
	width = theWidth;
	height = theHeight;
	tested = false;
	usable = false;
}

cScreenResolution::~cScreenResolution() {
}

void cScreenResolution::printLog() {
	char msg[255];
	sprintf(msg, "cScreenResolution:: width = %d, height = %d, tested = %d, usable = %d", width, height, tested, usable);
	logbook(msg);
}
