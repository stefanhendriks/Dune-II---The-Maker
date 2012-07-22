#include "Mouse.h"

#include "../include/data/gfxdata.h"

Mouse *Mouse::instance = NULL;

Mouse::Mouse(Bitmap * mouseBitmap) {
	this->mouseBitmap = mouseBitmap;
	x = y = z = 0;
}

Mouse::~Mouse() {
	mouseBitmap = NULL;
}

void Mouse::updateState() {
	x = mouse_x;
	y = mouse_y;
	z = mouse_z;
}

