#pragma once
#ifndef SCREEN_H_
#define SCREEN_H_

#include <assert.h>
#include <cstddef>

#include "allegro.h"

#include "../math/Vector2D.h"
#include "../allegro/Bitmap.h"
#include "../allegro/Color.h"

class Screen {
	public:
		Screen(int width, int height, BITMAP * bufferScreen) {
			this->width = width;
			this->height = height;
			this->buffer = bufferScreen;
			this->bitmap = new Bitmap(bufferScreen);
		}

		~Screen() {
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

		void draw(Bitmap * bitmap, Vector2D &vector);

		void drawRectangle(Vector2D &vector, int width, int height, Color color);

protected:

	private:
		int width, height;

		BITMAP * buffer;
		Bitmap * bitmap;
};

#endif /* SCREENBLITTER_H_ */
