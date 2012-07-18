#include "Map.h"

void Map::draw(Screen * screen) {
	this->mapBitmap->draw(screen->getBitmap(), 0, 0);
}