#ifndef SCREENBLITTER_H_
#define SCREENBLITTER_H_

#include <assert.h>
#include <cstddef>

#include "allegro.h"
extern DATAFILE * gfxdata; // temporarily!

#include "../gameobjects/ScreenResolution.h"
#include "../controls/Mouse.h"

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

		void blitScreenBufferToScreen();

		void blitMouseToScreenBuffer();

		void clearBuffer() {
			clear(bufferScreen);
		}

	protected:

	private:
		ScreenResolution * screenResolution;

		BITMAP * bufferScreen;
};

#endif /* SCREENBLITTER_H_ */
