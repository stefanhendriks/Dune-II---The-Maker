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
		
		~Map() { };
	
		Bitmap * getBitmap() {
			return mapBitmap;
		}

		Bitmap * getBitmapPiece(Vector2D &vector, int width, int height) {
			BITMAP * parent = mapBitmap->getBITMAP();
			BITMAP * subBitmap = create_sub_bitmap(parent, vector.getX(), vector.getY(), width, height);
			return new Bitmap(subBitmap, true);
		}

		Rectangle getRectangle() {
			return mapBitmap->getRectangle(0, 0);
		}

	private:
		Bitmap * mapBitmap;

};

#endif