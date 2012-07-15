#ifndef CBESTSCREENRESOLUTIONFINDER_H_
#define CBESTSCREENRESOLUTIONFINDER_H_

#define MAX_SCREENRESOLUTIONS 10

#include <sstream>
#include <string>

#include "allegro.h"
#include "../utils/common.h"
#include "../gameobjects/ScreenResolution.h"

class BestScreenResolutionFinder {
	public:
		BestScreenResolutionFinder();
		virtual ~BestScreenResolutionFinder();

		ScreenResolution * aquireBestScreenResolutionFullScreen();

		void checkResolutions();

	private:
		ScreenResolution * screenResolutions[MAX_SCREENRESOLUTIONS];
		bool isGfxModeListSet(GFX_MODE_LIST *mode_list);
		void detectScreenResolutionsByTestingThemOut();
		void setMatchingScreenResolutionsToTestedAndUsable(GFX_MODE_LIST * modeList);
		ScreenResolution * findMatchingScreenResolution(int width, int height);
};

#endif /* CBESTSCREENRESOLUTIONFINDER_H_ */
