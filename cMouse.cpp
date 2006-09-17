#include "d2tmh.h"

cMouse::cMouse() {
	setPoll(0,0,0);

	memset(mouseSelectX, 0, sizeof(mouseSelectX));
	memset(mouseSelectY, 0, sizeof(mouseSelectY));
}

void cMouse::setPoll(int x, int y, int z) {
	mouseX = x;
	mouseY = y;
	mouseZ = z;
}