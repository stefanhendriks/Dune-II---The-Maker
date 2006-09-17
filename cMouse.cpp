#include "d2tmh.h"

cMouse::cMouse() {
	setPoll(0,0,0);

	memset(mouseSelectX, 0, sizeof(mouseSelectX));
	memset(mouseSelectY, 0, sizeof(mouseSelectY));

	status = MOUSE_STATUS_NORMAL;
	
	sprite = MOUSE_NORMAL;
}

void cMouse::setPoll(int x, int y, int z) {
	mouseX = x;
	mouseY = y;
	mouseZ = z;
}

void cMouse::draw() {
	MME_SURFACE mouseSurface;

#ifdef ALLEGRO_H
	/** TODO: This should be generic **/
	mouseSurface = (BITMAP *)gfxdata[mouse_tile].dat; // <-- direct acces through Allegro dat files should dissapear
#endif

	int drawX = mouseX;
	int drawY = mouseY;
	/** todo: depending on status and such fix the x, y coordinates for drawing **/
	
	MMEngine->drawSprite(mouseSurface, bmp_screen, 0, 0);
//	MMEngine->drawSprite(mouseSurface, DrawManager->getBuffer(), drawX, drawY);
}