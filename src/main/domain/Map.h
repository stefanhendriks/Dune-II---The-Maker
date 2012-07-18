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

	private:
		Bitmap * mapBitmap;

};

#endif