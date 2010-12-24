/*
 * cBestScreenResolutionFinder.h
 *
 *  Created on: 24 dec. 2010
 *      Author: Stefan
 */

#ifndef CBESTSCREENRESOLUTIONFINDER_H_
#define CBESTSCREENRESOLUTIONFINDER_H_

class cBestScreenResolutionFinder {
	public:
		cBestScreenResolutionFinder();
		virtual ~cBestScreenResolutionFinder();

		bool aquireBestScreenResolutionFullScreen();

	private:
		cScreenResolution * screenResolutions[5];
};

#endif /* CBESTSCREENRESOLUTIONFINDER_H_ */
