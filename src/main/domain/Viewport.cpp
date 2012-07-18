#include "Viewport.h"

void Viewport::draw(Screen * screen, int x, int y) {
	
	// determine what to draw on screen

	// z-ordering (z is already known in entity)

	// draw it on bitmap (buffer)
	Bitmap * mapBitmapPiece = map->getBitmapPiece(x, y, x + this->width, y + this->height);
	
	this->bitmap->draw(mapBitmapPiece, x, y);
	
	// draw bitmap (buffer) on screen
	screen->draw(this->bitmap, x, y);

	// clean up memory
	delete mapBitmapPiece;
}