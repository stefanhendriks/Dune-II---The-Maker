#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include "../include/allegro.h"

#include "../exceptions/exceptions.h"

#include "Rectangle.h"
#include "Color.h"

class Bitmap {

	public:
		Bitmap(BITMAP * bitmap) {
			if (bitmap == NULL) {
				throw NullArgumentException;
			}
			this->bitmap = bitmap;
			this->destroyBitmap = false;
		}

		Bitmap(int width, int height) {
			this->bitmap = create_bitmap(width, height);
			clear_to_color(this->bitmap, Colors::Black.makeColor());
			this->destroyBitmap = true;
		}

		~Bitmap() {
			if (destroyBitmap) {
				destroy_bitmap(this->bitmap);
				this->bitmap = NULL;
			}
		}

		int getHeight() {
			return this->bitmap->h;
		}

		int getWidth() {
			return this->bitmap->w;
		}

		Rectangle getRectangle(int x, int y) {
			Rectangle r;
			r.setCoordinates(x, y, getWidth(), getHeight());
			return r;
		}

		void draw(Bitmap * sprite, int x, int y) {
			BITMAP * dest = this->getBITMAP();
			BITMAP * src = sprite->getBITMAP();
			draw_sprite(dest, src, x, y);
		}

		BITMAP * getBITMAP() {
			return this->bitmap;
		}

	private:
		BITMAP * bitmap;
		bool destroyBitmap;

};

#endif