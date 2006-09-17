#include "d2tmh.h"

cMouse::cMouse() {
	setPoll(0,0,0);

	memset(mouseSelectX, 0, sizeof(mouseSelectX));
	memset(mouseSelectY, 0, sizeof(mouseSelectY));

	status = MOUSE_STATUS_NORMAL;
	sprite = MOUSE_NORMAL;

	holdingMouseButton[MOUSE_BTN_RIGHT] = false;
	holdingMouseButton[MOUSE_BTN_LEFT] = false;

	btnRight = false;
	btnLeft = false;
}

/** Called by lib specific function to put the right vars in this **/
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
	MMEngine->drawSprite(mouseSurface, (DrawManager->getBuffer()), drawX, drawY);
}


/** Poll mouse **/
void cMouse::poll() {
	btnLeft  = MMEngine->getMouseButton(MOUSE_BTN_LEFT);
	btnRight = MMEngine->getMouseButton(MOUSE_BTN_RIGHT);

	if (!btnLeft) {
		holdingMouseButton[MOUSE_BTN_LEFT] = false;
	}

	if (!btnRight) {
		holdingMouseButton[MOUSE_BTN_RIGHT] = false;
	}
}

bool cMouse::btnSingleClickLeft() {
	if (btnLeft) {
		if (!holdingMouseButton[MOUSE_BTN_LEFT]) {
			holdingMouseButton[MOUSE_BTN_LEFT] = true;
			return true;
		}
	} else {
		holdingMouseButton[MOUSE_BTN_LEFT] = false;
	}

	return false;
}

bool cMouse::btnSingleClickRight() {
	if (btnRight) {
		if (!holdingMouseButton[MOUSE_BTN_RIGHT]) {
			holdingMouseButton[MOUSE_BTN_RIGHT] = true;
			return true;
		} 
	} else {
		holdingMouseButton[MOUSE_BTN_RIGHT] = false;
	}

	return false;
}