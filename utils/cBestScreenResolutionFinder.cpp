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
	memset(screenResolutions, 0, sizeof(screenResolutions));

	// order is from 'biggest' to smallest best resolution
	screenResolutions[0] = new cScreenResolution(1920, 1080);
	screenResolutions[1] = new cScreenResolution(1600, 1050);
	screenResolutions[2] = new cScreenResolution(1600, 900);
	screenResolutions[3] = new cScreenResolution(1280, 1024);
	screenResolutions[4] = new cScreenResolution(1280, 800);
	screenResolutions[5] = new cScreenResolution(1366, 768);
	screenResolutions[6] = new cScreenResolution(1027, 768);
	screenResolutions[7] = new cScreenResolution(800, 600);
}

cBestScreenResolutionFinder::~cBestScreenResolutionFinder() {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i ++) {
		if (screenResolutions[i]) {
			delete screenResolutions[i];
			screenResolutions[i] = NULL;
		}
	}
}

void cBestScreenResolutionFinder::checkResolutions() {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i++) {
		cScreenResolution * screenResolution = screenResolutions[i];
		if (screenResolution) {
			int screen_x = screenResolution->getWidth();
			int screen_y = screenResolution->getHeight();
			int result = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, screen_x, screen_y, screen_x, screen_y);
			screenResolution->setTested(true);
			screenResolution->setUsable((result > -1));
			screenResolution->printLog();
		}
	}
}

bool cBestScreenResolutionFinder::aquireBestScreenResolutionFullScreen() {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i++) {
		cScreenResolution * screenResolution = screenResolutions[i];
		if (screenResolution) {
			if (screenResolution->isTested()) {
				if (screenResolution->isUsable()) {
					game.screen_x = screenResolution->getWidth();
					game.screen_y = screenResolution->getHeight();
					set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
					return true;
				}
			}
		}
	}
	return false;
}

