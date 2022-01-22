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
cBestScreenResolutionFinder::cBestScreenResolutionFinder(int desiredColorDepth) {
    resolutionIndex = 0;
    this->desiredColorDepth = desiredColorDepth;
    init();
    // tries them in this order (1st come first serve)
    // do note, when DPI is > 100% in OS (atleast in Windows) things seem to get wonky.
    // see also: https://github.com/stefanhendriks/Dune-II---The-Maker/issues/314
    addScreenResolution(3840, 2160); // 4k
    addScreenResolution(2048, 1536); // 4:3 aspect ratio
    addScreenResolution(1920, 1440); // 4:3 aspect ratio
    addScreenResolution(1920, 1080); // Full HD
    addScreenResolution(1600, 900);
    addScreenResolution(1280, 1024);
    addScreenResolution(1366, 768);  // widescreen
    addScreenResolution(1024, 768);
    addScreenResolution(800, 600);
}

cBestScreenResolutionFinder::~cBestScreenResolutionFinder() {
	for (auto & screenResolution : screenResolutions) {
	    delete screenResolution;
		screenResolution = nullptr;
	}
}

void cBestScreenResolutionFinder::init() {
    memset(screenResolutions, 0, sizeof(screenResolutions));
}

bool cBestScreenResolutionFinder::isGfxModeListSet(GFX_MODE_LIST *mode_list) {
    return mode_list != nullptr;
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

    GFX_MODE_LIST *modeList = nullptr;

#ifdef UNIX
    modeList = get_gfx_mode_list(GFX_XWINDOWS_FULLSCREEN);
#else
    modeList = get_gfx_mode_list(GFX_DIRECTX_ACCEL);
#endif

    if (!isGfxModeListSet(modeList)) {
        logbook("Must detect resolutions by testing them out.");
        detectScreenResolutionsByTestingThemOut();
        return;
    }

    setMatchingScreenResolutionsToTestedAndUsable(modeList);

    destroy_gfx_mode_list(modeList);
}

void cBestScreenResolutionFinder::setMatchingScreenResolutionsToTestedAndUsable(GFX_MODE_LIST * modeList) {
	assert(modeList);

	GFX_MODE * modes = modeList->mode;
	assert(modes);

	for (int iMode=0; iMode < modeList->num_modes; iMode++) {
		GFX_MODE mode = modes[iMode];

        cScreenResolution * screenResolution = findMatchingScreenResolution(mode.width, mode.height);
        if (mode.bpp != desiredColorDepth) {
            continue;
        }

        if (screenResolution) {
            screenResolution->setUsable(true);
            screenResolution->setTested(true);
            screenResolution->printLog();
        }
	}
}

cScreenResolution * cBestScreenResolutionFinder::findMatchingScreenResolution(int width, int height) {
	for (auto & screenResolution : screenResolutions) {
        if (screenResolution) {
            if (screenResolution->getWidth() == width && screenResolution->getHeight() == height) {
                return screenResolution;
            }
        }
	}
	return nullptr;
}

void cBestScreenResolutionFinder::detectScreenResolutionsByTestingThemOut() {
    for (auto screenResolution : screenResolutions) {
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
    for (auto screenResolution : screenResolutions) {
        if (screenResolution) {
            if (screenResolution->isTested()) {
                if (screenResolution->isUsable()) {
                    game.m_screenX = screenResolution->getWidth();
                    game.m_screenY = screenResolution->getHeight();

                    int r;
#ifdef UNIX
                    r = set_gfx_mode(GFX_AUTODETECT, game.m_screenX, game.m_screenY, game.m_screenX, game.m_screenY);
#else
                    r = set_gfx_mode(GFX_DIRECTX_ACCEL, game.m_screenX, game.m_screenY, game.m_screenX, game.m_screenY);
#endif
                    char msg[255];
                    sprintf(msg, "setting up full screen mode with resolution %dx%d, result code: %d", game.m_screenX, game.m_screenY, r);
                    logbook(msg);
                    return (r == 0); // return true only if r==0
                }
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

