#pragma once
#ifndef MAP_H
#define MAP_H

#include "../exceptions/exceptions.h"

#include "Screen.h"
#include "Vector2D.h"

class Map {

	public:
		Map(Bitmap * mapBitmap) {
			if (mapBitmap == NULL) {
				throw NullArgumentException;
			}
			this->mapBitmap = mapBitmap;
		}
		
		~Map() { /* do not delete mapBitmap, as it is not its owner */ };
	
		Bitmap * getBitmap() {
			return mapBitmap;
		}

		Bitmap * getSubBitmap(Vector2D &vector, int width, int height) {
			return this->mapBitmap->getSubBitmap(vector, width, height);
		}

		Rectangle getRectangle() {
			return mapBitmap->getRectangle(0, 0);
		}

	private:
		Bitmap * mapBitmap;

};

#endif