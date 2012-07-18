#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include "../include/allegro.h"

#include "../exceptions/exceptions.h"

#include "Rectangle.h"

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

		void draw(Bitmap * dest, int x, int y) {
			draw_sprite(dest->getBITMAP(), this->getBITMAP(), x, y);
		}

		BITMAP * getBITMAP() {
			return this->bitmap;
		}

	private:
		BITMAP * bitmap;
		bool destroyBitmap;

};

#endif