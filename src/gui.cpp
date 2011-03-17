// GUI functions

#include "include/d2tmh.h"

void GUI_INIT() {

}

void GUI_DRAW() {

}

void GUI_POLL() {

}

bool GUI_DRAW_FRAME_PRESSED(int x1, int y1, int width, int height) {
	// fill it up
	rectfill(bmp_screen, x1, y1, x1 + width, y1 + height, makecol(176, 176, 196));

	// rect
	rect(bmp_screen, x1, y1, x1 + width, y1 + height, makecol(84, 84, 120));

	// lines to darken the right sides
	line(bmp_screen, x1 + width, y1, x1 + width, y1 + height, makecol(252, 252, 252));
	line(bmp_screen, x1, y1 + height, x1 + width, y1 + height, makecol(252, 252, 252));

	if ((mouse_x >= x1 && mouse_x < (x1 + width)) && (mouse_y >= y1 && mouse_y <= (y1 + height)))
		return true;

	return false;
}

bool GUI_DRAW_FRAME(int x1, int y1, int width, int height) {
	// fill it up
	rectfill(bmp_screen, x1, y1, x1 + width, y1 + height, makecol(176, 176, 196));

	// rect
	rect(bmp_screen, x1, y1, x1 + width, y1 + height, makecol(252, 252, 252));

	// lines to darken the right sides
	line(bmp_screen, x1 + width, y1, x1 + width, y1 + height, makecol(84, 84, 120));
	line(bmp_screen, x1, y1 + height, x1 + width, y1 + height, makecol(84, 84, 120));

	if ((mouse_x >= x1 && mouse_x < (x1 + width)) && (mouse_y >= y1 && mouse_y <= (y1 + height))) {
		return true;
	}

	return false;
}
