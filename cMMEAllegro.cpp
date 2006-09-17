#include "d2tmh.h"

cMMEAllegro::cMMEAllegro() {

}

int cMMEAllegro::getMouseX() {
	return mouse_x;
}

int cMMEAllegro::getMouseY() {
	return mouse_y;
}

int cMMEAllegro::getMouseZ() {
	return mouse_z;
}


/** Allegro draw_sprite */
void cMMEAllegro::drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y) {
	draw_sprite(dest, source, x, y);
}

/** Allegro blitting routine **/
void cMMEAllegro::doBlit(MME_SURFACE source, MME_SURFACE dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) {
	blit(source, dest, source_x, source_y, dest_x, dest_y, width, height);	
}

/** Simple blitting **/
void cMMEAllegro::doBlit(BITMAP *source, BITMAP *dest, int x, int y, int width, int height) {
	doBlit(source, dest, 0, 0, x, y, width, height);	
}

/** Primitive blitting, take source sizes **/
void cMMEAllegro::doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y) {
	doBlit(source, dest, x, y, source->w, source->h);
}

MME_SURFACE cMMEAllegro::createSurface(int width, int height) {
	return create_bitmap(width, height);	
}

MME_SURFACE cMMEAllegro::createSurface(int width, int height, int bitdepth) {
	if (bitdepth == 8 || bitdepth == 16 || bitdepth == 32) {
		return create_bitmap_ex(bitdepth, width, height); 
	}

	return createSurface(width, height);
}

void cMMEAllegro::colorSurface(MME_SURFACE surface, int color) {
	if (color < 0) {
		clear(surface);
	} else {
		clear_to_color(surface, color);
	}
}

void cMMEAllegro::clearSurface(MME_SURFACE surface) {
	colorSurface(surface, -1);
}