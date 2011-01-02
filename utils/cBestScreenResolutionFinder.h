/*
 * cBestScreenResolutionFinder.h
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

#ifndef CBESTSCREENRESOLUTIONFINDER_H_
#define CBESTSCREENRESOLUTIONFINDER_H_

#define MAX_SCREENRESOLUTIONS 10

class cBestScreenResolutionFinder {
	public:
		cBestScreenResolutionFinder();
		virtual ~cBestScreenResolutionFinder();

		bool aquireBestScreenResolutionFullScreen();

		void checkResolutions();

	private:
		cScreenResolution * screenResolutions[MAX_SCREENRESOLUTIONS];
};

#endif /* CBESTSCREENRESOLUTIONFINDER_H_ */
