#pragma once
#ifndef MAP_H
#define MAP_H

#include "../infrastructure/exceptions.h"

#include "../infrastructure/allegro/Screen.h"
#include "../infrastructure/math/Vector2D.h"

#include "Theme.h"

class Map {

	public:
		Map(int width, int height, Theme * theme) {
			mapBitmap = new Bitmap(width * theme->getTileWidth(), height * theme->getTileHeight());
			this->height = height;
			this->width = width;
		}

		Map(int width, int height, Bitmap * fillWithThisBitmap) {
			if (fillWithThisBitmap == NULL) throw NullArgumentException;
			mapBitmap = new Bitmap(width, height);
			int drawHorizontallyAmountOfTimes = (width / fillWithThisBitmap->getWidth()) + 1;
			int drawVerticallyAmountOfTimes = (height / fillWithThisBitmap->getHeight()) + 1;
			for (int x = 0; x < drawHorizontallyAmountOfTimes; x++) {
				for (int y = 0; y < drawVerticallyAmountOfTimes; y++) {
					mapBitmap->draw(fillWithThisBitmap, (x * fillWithThisBitmap->getWidth()), (y * fillWithThisBitmap->getHeight()));
				}
			}
			this->height = height;
			this->width = width;
		}
		
		~Map() { /* do not delete mapBitmap, as it is not its owner */ };
	
		Bitmap * getBitmap() {
			return mapBitmap;
		}

		Bitmap * getSubBitmap(Vector2D &vector, int width, int height) {
			return this->mapBitmap->getSubBitmap(vector, width, height);
		}

		void draw(Bitmap * bitmap, int x, int y) {
			this->mapBitmap->draw(bitmap, x, y);
		}

		int getWidth() { return this->width; }
		int getHeight() { return this->height; }

	private:
		Bitmap * mapBitmap;
		int height, width;

};

#endif