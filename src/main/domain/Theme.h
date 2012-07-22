#pragma once
#ifndef THEME_H
#define THEME_H

#include "../exceptions/exceptions.h"

#include "Vector2D.h"
#include "Bitmap.h"

const int TILE_HEIGHT = 32;
const int TILE_WIDTH = 32;

class Theme {

	public:
		Theme(Bitmap * bitmap) {
			if (bitmap == NULL) throw NullArgumentException;
			this->bitmap = bitmap;
			this->tileHeight = TILE_HEIGHT;
			this->tileWidth = TILE_WIDTH;
		}

		int getTileWidth() {
			return this->tileHeight;
		}

		int getTileHeight() {
			return this->tileHeight;
		}

	protected:
		Bitmap * getBitmap(int x, int y) {
			int startX = x * tileWidth;
			int startY = y * tileHeight;
			Vector2D vector;
			vector.set(startX, startY);
			return bitmap->getSubBitmap(vector, tileWidth, tileHeight);
		}

	private:
		Bitmap * bitmap;
		int tileWidth, tileHeight;

};


#endif