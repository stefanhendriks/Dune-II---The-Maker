#include "Viewport.h"

void Viewport::draw(Screen * screen, Vector2D &drawingVector, Vector2D &viewingVector) {
	this->bitmap->clear(Colors::Black); // probably not needed?

	// determine what to draw on screen

	// z-ordering (z is already known in entity)

	// draw it on bitmap (buffer)
	Bitmap * mapBitmapPiece = map->getSubBitmap(viewingVector, viewingVector.getX() + this->width, viewingVector.getY() + this->height);
	
	this->bitmap->draw(mapBitmapPiece, 0, 0);
	
	// draw bitmap (buffer) on screen
	screen->draw(this->bitmap, drawingVector);

	// clean up memory
	delete mapBitmapPiece;
}
