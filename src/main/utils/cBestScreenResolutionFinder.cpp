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
	screenResolutions[0] = new cScreenResolution(1920, 1200);
	screenResolutions[1] = new cScreenResolution(1920, 1080);
	screenResolutions[2] = new cScreenResolution(1600, 1050);
	screenResolutions[3] = new cScreenResolution(1600, 900);
	screenResolutions[4] = new cScreenResolution(1366, 768);
	screenResolutions[5] = new cScreenResolution(1280, 1024);
	screenResolutions[6] = new cScreenResolution(1280, 800);
	screenResolutions[7] = new cScreenResolution(1027, 768);
	screenResolutions[8] = new cScreenResolution(800, 600);
}

cBestScreenResolutionFinder::~cBestScreenResolutionFinder() {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i++) {
		delete screenResolutions[i];
		screenResolutions[i] = NULL;
	}
}

bool cBestScreenResolutionFinder::isGfxModeListSet(GFX_MODE_LIST *mode_list) {
	return mode_list != NULL;
}

void cBestScreenResolutionFinder::checkResolutions() {

	/*
	 Note from Peter Gaal:
	 This code is untested on UNIX platfrom - GFX_XWINDOWS_FULLSCREEN
	 should be the right constant to detect available video modes on UNIX
	 system. The code is tested on Windows system with GFX_DIRECTX_ACCEL and
	 GFX_DIRECTX constants and it works.
	 This part of code uses get_gfx_mode_list function, which returns
	 available video modes on system. GFX_DIRECTX_ACCEL, GFX_DIRECTX
	 are the good constants as a parameter, don't use GFX_AUTODETECT,
	 GFX_SAFE or other "magic" driver constants, otherwise function
	 will return NULL. The algorithm matches all screen modes against
	 the list in screenResolutions (using 16 bpp mode only) and in case
	 of match will set up setTested and setUsable flags.
	 */

	GFX_MODE_LIST * modeList = NULL;

#ifdef UNIX
	modeList = get_gfx_mode_list(GFX_XWINDOWS_FULLSCREEN);
#else
	modeList = get_gfx_mode_list(GFX_DIRECTX_ACCEL);
#endif

	if (!isGfxModeListSet(modeList)) {
		detectScreenResolutionsByTestingThemOut();
		return;
	}

	setMatchingScreenResolutionsToTestedAndUsable(modeList);

	destroy_gfx_mode_list(modeList);
}

void cBestScreenResolutionFinder::setMatchingScreenResolutionsToTestedAndUsable(GFX_MODE_LIST * modeList) {
	logbook("setMatchingScreenResolutionsToTestedAndUsable");
	assert(modeList);

	GFX_MODE * modes = modeList->mode;
	assert(modes);

	for (int iMode = 0; iMode < modeList->num_modes; iMode++) {
		GFX_MODE mode = modes[iMode];

		// we match only 16 bit color depth modes, as we use 16bpp in fullscreen mode
		if (mode.bpp == 16) {
			cScreenResolution * screenResolution = findMatchingScreenResolution(mode.width, mode.height);
			if (screenResolution) {
				screenResolution->setUsable(true);
				screenResolution->setTested(true);
			}
		}
	}
}

cScreenResolution * cBestScreenResolutionFinder::findMatchingScreenResolution(int width, int height) {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i++) {
		cScreenResolution * screenResolution = screenResolutions[i];
		if (screenResolution) {
			if (screenResolution->getWidth() == width && screenResolution->getHeight() == height) {
				return screenResolution;
			}
		}
	}
	return NULL;
}

void cBestScreenResolutionFinder::detectScreenResolutionsByTestingThemOut() {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i++) {
		cScreenResolution * screenResolution = screenResolutions[i];
		if (screenResolution) {
			int screen_x = screenResolution->getWidth();
			int screen_y = screenResolution->getHeight();
			int r = set_gfx_mode(GFX_AUTODETECT, screen_x, screen_y, screen_x, screen_y);
			screenResolution->setTested(true);
			screenResolution->setUsable((r > -1));
			screenResolution->printLog();
		}
	}
}

cScreenResolution * cBestScreenResolutionFinder::aquireBestScreenResolutionFullScreen() {
	for (int i = 0; i < MAX_SCREENRESOLUTIONS; i++) {
		cScreenResolution * screenResolution = screenResolutions[i];
		if (screenResolution) {
			if (screenResolution->isTested()) {
				if (screenResolution->isUsable()) {
					int r;
#ifdef UNIX
					r = set_gfx_mode(GFX_AUTODETECT, screenResolution->getWidth(), screenResolution->getHeight(), screenResolution->getWidth(), game.screen_y);
#else
					r = set_gfx_mode(GFX_DIRECTX_ACCEL, screenResolution->getWidth(), screenResolution->getHeight(),
							screenResolution->getWidth(), screenResolution->getHeight());
#endif
					char msg[255];
					sprintf(msg, "setting up full screen mode with resolution %dx%d, result code: %d", screenResolution->getWidth(), screenResolution->getHeight(), r);
					logbook(msg);
					if (r > -1) {
						return new cScreenResolution(screenResolution->getWidth(), screenResolution->getHeight());
					}
				}
			}
		}
	}
	return NULL;
}

