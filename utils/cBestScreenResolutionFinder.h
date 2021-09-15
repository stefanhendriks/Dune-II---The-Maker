#ifndef CBESTSCREENRESOLUTIONFINDER_H_
#define CBESTSCREENRESOLUTIONFINDER_H_

#define MAX_SCREEN_RESOLUTIONS      15

class cBestScreenResolutionFinder {
	public:
		cBestScreenResolutionFinder(int desiredColorDepth);
		virtual ~cBestScreenResolutionFinder();

		bool acquireBestScreenResolutionFullScreen();

		void checkResolutions();

	private:
        cScreenResolution * screenResolutions[MAX_SCREEN_RESOLUTIONS];
        int resolutionIndex;
        int desiredColorDepth;

        void init();
        void addScreenResolution(int width, int height);
		bool isGfxModeListSet(GFX_MODE_LIST *mode_list);
		void detectScreenResolutionsByTestingThemOut();
		void setMatchingScreenResolutionsToTestedAndUsable(GFX_MODE_LIST * modeList);
		cScreenResolution * findMatchingScreenResolution(int width, int height);
};

#endif /* CBESTSCREENRESOLUTIONFINDER_H_ */
