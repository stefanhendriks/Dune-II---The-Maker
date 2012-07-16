#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include "../include/allegro.h"

#include "Rectangle.h"

class Bitmap {

	public:
		Bitmap(BITMAP * bitmap) {
			this->bitmap = bitmap;
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

};

#endif