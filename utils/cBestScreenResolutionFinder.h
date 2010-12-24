/*
 * cBestScreenResolutionFinder.h
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

#ifndef CBESTSCREENRESOLUTIONFINDER_H_
#define CBESTSCREENRESOLUTIONFINDER_H_

#define MAX_SCREEN_RESOLUTIONS      15

class cBestScreenResolutionFinder {
	public:
		cBestScreenResolutionFinder();
		virtual ~cBestScreenResolutionFinder();

		bool acquireBestScreenResolutionFullScreen();

	private:
        void addScreenResolution(int width, int height);
        void init();
		cScreenResolution * screenResolutions[15];
		int resolutionIndex;
};

#endif /* CBESTSCREENRESOLUTIONFINDER_H_ */
