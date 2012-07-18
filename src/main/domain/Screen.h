#pragma once
#ifndef SCREEN_H_
#define SCREEN_H_

#include <assert.h>
#include <cstddef>

#include "allegro.h"
#include "ScreenResolution.h"

#include "Bitmap.h"
#include "Color.h"

class Screen {
	public:
		Screen(ScreenResolution * screenResolution, BITMAP * bufferScreen) {
			assert(screenResolution);
			this->screenResolution = screenResolution;
			this->buffer = bufferScreen;
			this->bitmap = new Bitmap(bufferScreen);
		}

		~Screen() {
			screenResolution = NULL;
		}

		void blitScreenBufferToScreen();

		void clearBuffer() {
			clear(buffer);
		}

		BITMAP * Screen::getBuffer() {
			return this->buffer;
		}
		
		Bitmap * getBitmap() {
			return this->bitmap;
		}

		ScreenResolution * getScreenResolution();
		
		void draw(Bitmap * bitmap, int x, int y);

		void drawRectangle(int x, int y, int width, int height, Color color);

protected:

	private:
		ScreenResolution * screenResolution;

		BITMAP * buffer;
		Bitmap * bitmap;
};

#endif /* SCREENBLITTER_H_ */
