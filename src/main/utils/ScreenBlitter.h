#ifndef SCREENBLITTER_H_
#define SCREENBLITTER_H_

#include <assert.h>
#include <cstddef>

#include "../gameobjects/ScreenResolution.h"

#include "allegro.h"

class ScreenBlitter {
	public:
		ScreenBlitter(ScreenResolution * screenResolution, BITMAP * bufferScreen) {
			assert(screenResolution);
			this->screenResolution = screenResolution;
			this->bufferScreen = bufferScreen;
		}

		~ScreenBlitter() {
			screenResolution = NULL;
		}

		void blitScreen();

	protected:

	private:
		ScreenResolution * screenResolution;

		// TODO: use the BITMAP * bmp_screen, double buffer within this class.
		BITMAP * bufferScreen;
};

#endif /* SCREENBLITTER_H_ */
