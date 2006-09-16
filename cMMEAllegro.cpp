#include "d2tmh.h"

/** Allegro draw_sprite */
void cMMEAllegro::drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y) {
	draw_sprite(dest, source, x, y);
}
