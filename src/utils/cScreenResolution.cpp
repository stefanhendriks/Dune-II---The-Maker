/*
 * cScreenResolution.cpp
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

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
	//cLogger::getInstance()->log
	char msg[255];
	sprintf(msg, "cBestScreenResolutionFinder:: width = %d, height = %d, tested = %d, usable = %d", width, height, tested, usable);
	logbook(msg);
}
