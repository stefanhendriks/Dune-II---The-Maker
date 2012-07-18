#include "Viewport.h"

void Viewport::draw(Screen * screen, Vector2D &vector) {
	this->bitmap->clear(Colors::Black); // probably not needed?

	// determine what to draw on screen

	// z-ordering (z is already known in entity)

	// draw it on bitmap (buffer)
	Bitmap * mapBitmapPiece = map->getBitmapPiece(vector, vector.getX() + this->width, vector.getY() + this->height);
	
	this->bitmap->draw(mapBitmapPiece, 0, 0);
	
	// draw bitmap (buffer) on screen
	screen->draw(this->bitmap, vector);

	// clean up memory
	delete mapBitmapPiece;
}
