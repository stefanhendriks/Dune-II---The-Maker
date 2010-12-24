/*
 * cBestScreenResolutionFinder.cpp
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

/**
 *
 */
cBestScreenResolutionFinder::cBestScreenResolutionFinder() {
	screenResolutions[0] = new cScreenResolution(1920, 1080);
	screenResolutions[1] = new cScreenResolution(1600, 900);
	screenResolutions[2] = new cScreenResolution(1280, 1024);
	screenResolutions[3] = new cScreenResolution(1027, 768);
	screenResolutions[4] = new cScreenResolution(800, 600);
}

cBestScreenResolutionFinder::~cBestScreenResolutionFinder() {
	for (int i = 0; i < 5; i ++) {
		delete screenResolutions[i];
		screenResolutions[i] = NULL;
	}
}


bool cBestScreenResolutionFinder::aquireBestScreenResolutionFullScreen() {
	for (int i = 0; i < 5; i++) {
		game.screen_x = screenResolutions[i]->getWidth();
		game.screen_y = screenResolutions[i]->getHeight();
		int r = set_gfx_mode(GFX_AUTODETECT, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
		char msg[255];
		sprintf(msg, "Trying to aquire screen resolution %d X %d", game.screen_x, game.screen_y);
		logbook(msg);
		if (r > -1) {
			logbook("Succes");
			// success
			return true;
		}
		logbook("Failed");
	}
	return false;
}
