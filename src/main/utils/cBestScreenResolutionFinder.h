/*
 * cBestScreenResolutionFinder.h
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

#ifndef CBESTSCREENRESOLUTIONFINDER_H_
#define CBESTSCREENRESOLUTIONFINDER_H_

#define MAX_SCREENRESOLUTIONS 10

#include "../gameobjects/cScreenResolution.h"

class cBestScreenResolutionFinder {
	public:
		cBestScreenResolutionFinder();
		virtual ~cBestScreenResolutionFinder();

		cScreenResolution * aquireBestScreenResolutionFullScreen();

		void checkResolutions();

	private:
		cScreenResolution * screenResolutions[MAX_SCREENRESOLUTIONS];
		bool isGfxModeListSet(GFX_MODE_LIST *mode_list);
		void detectScreenResolutionsByTestingThemOut();
		void setMatchingScreenResolutionsToTestedAndUsable(GFX_MODE_LIST * modeList);
		cScreenResolution * findMatchingScreenResolution(int width, int height);
};

#endif /* CBESTSCREENRESOLUTIONFINDER_H_ */
