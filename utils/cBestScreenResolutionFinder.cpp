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
    resolutionIndex = 0;
    init();
//    addScreenResolution(3840, 2160); // 4k (seems to break?)
    addScreenResolution(2048, 1536); // 4:3 aspect ratio
    addScreenResolution(1920, 1440); // 4:3 aspect ratio
    addScreenResolution(1920, 1080); // Full HD
    addScreenResolution(1600, 900);
    addScreenResolution(1280, 1024);
    addScreenResolution(1024, 768);
    addScreenResolution(800, 600);
    addScreenResolution(640, 480);
}

cBestScreenResolutionFinder::~cBestScreenResolutionFinder() {
	for (int i = 0; i < MAX_SCREEN_RESOLUTIONS; i ++) {
	    if (screenResolutions[i]) delete screenResolutions[i];
		screenResolutions[i] = NULL;
	}
}

void cBestScreenResolutionFinder::init() {
    memset(screenResolutions, 0, sizeof(screenResolutions));
}


void cBestScreenResolutionFinder::checkResolutions() {
    for (int i = 0; i < MAX_SCREEN_RESOLUTIONS; i++) {
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

bool cBestScreenResolutionFinder::acquireBestScreenResolutionFullScreen() {
    for (int i = 0; i < MAX_SCREEN_RESOLUTIONS; i++) {
        cScreenResolution * screenResolution = screenResolutions[i];
        if (screenResolution->isTested()) {
            if (screenResolution->isUsable()) {
                game.screen_x = screenResolution->getWidth();
                game.screen_y = screenResolution->getHeight();
                set_gfx_mode(GFX_AUTODETECT, game.screen_x, game.screen_y, game.screen_x, game.screen_y);
                return true;
            }
        }
    }
    return false;
}

void cBestScreenResolutionFinder::addScreenResolution(int width, int height) {
    if (resolutionIndex >= MAX_SCREEN_RESOLUTIONS) return;
    screenResolutions[resolutionIndex] = new cScreenResolution(width, height);
    resolutionIndex++;
}

