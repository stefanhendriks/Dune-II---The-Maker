#ifndef SCREEN_H_
#define SCREEN_H_

#include <assert.h>
#include <cstddef>

#include "allegro.h"
extern DATAFILE * gfxdata; // temporarily!

#include "../gameobjects/ScreenResolution.h"
#include "../controls/Mouse.h"

class Screen {
	public:
		Screen(ScreenResolution * screenResolution, BITMAP * bufferScreen) {
			assert(screenResolution);
			this->screenResolution = screenResolution;
			this->buffer = bufferScreen;
		}

		~Screen() {
			screenResolution = NULL;
		}

		void blitScreenBufferToScreen();

		void clearBuffer() {
			clear(buffer);
		}

		BITMAP * getBuffer();
		ScreenResolution * getScreenResolution();
protected:

	private:
		ScreenResolution * screenResolution;

		BITMAP * buffer;
};

#endif /* SCREENBLITTER_H_ */
