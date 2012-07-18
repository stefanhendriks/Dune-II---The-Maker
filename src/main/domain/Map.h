#pragma once
#ifndef MAP_H
#define MAP_H

#include "../exceptions/exceptions.h"
	
class Map {

	public:
		Map(Bitmap * mapBitmap) {
			if (mapBitmap == NULL) {
				throw NullArgumentException;
			}
			this->mapBitmap = mapBitmap;
		}
		
		~Map();

		void draw();
	
	private:
		Bitmap * mapBitmap;

};

#endif