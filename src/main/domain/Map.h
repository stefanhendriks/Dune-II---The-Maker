#pragma once
#ifndef MAP_H
#define MAP_H

#include "../exceptions/exceptions.h"

#include "Screen.h"

class Map {

	public:
		Map(Bitmap * mapBitmap) {
			if (mapBitmap == NULL) {
				throw NullArgumentException;
			}
			this->mapBitmap = mapBitmap;
		}
		
		~Map() { };
	
		Bitmap * getBitmap() {
			return mapBitmap;
		}

		Bitmap * getBitmapPiece(int x, int y, int width, int height) {
			BITMAP * parent = mapBitmap->getBITMAP();
			BITMAP * subBitmap = create_sub_bitmap(parent, x, y, width, height);
			return new Bitmap(subBitmap, true);
		}

	private:
		Bitmap * mapBitmap;

};

#endif