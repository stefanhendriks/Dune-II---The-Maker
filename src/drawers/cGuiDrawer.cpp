/*
 * cGuiDrawer.cpp
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cGuiDrawer::cGuiDrawer() {
}

cGuiDrawer::~cGuiDrawer() {
}

void cGuiDrawer::drawGuiShape(cGuiShape * shape) {
	cRectangle * rectangle = shape->getRectangle();
	int x = rectangle->getStartX();
	int y = rectangle->getStartY();
	int width = rectangle->getWidth();
	int height = rectangle->getHeight();

	int lightColor = shape->getLightBorderColor();
	int darkColor = shape->getDarkBorderColor();
	int inner = shape->getInnerColor();

	rectfill(bmp_screen, x, y, x + width, y + height, inner);
	rect(bmp_screen, x, y, x + width, y + height, lightColor);

	// lines to darken the right sides
	line(bmp_screen, x + width, y, x + width, y + height, darkColor);
	line(bmp_screen, x, y + height, x + width, y + height, darkColor);
}

void cGuiDrawer::drawButton(cGuiButton * guiButton) {
	assert(guiButton);
	cMouse * mouse = cMouse::getInstance();
	cRectangle * rect = guiButton->getRectangle();

	int x = rect->getStartX();
	int y = rect->getStartY();

	BITMAP * bitmap = guiButton->getBitmap();
	if (bitmap) {
		draw_sprite(bmp_screen, bitmap, x, y);
		return;
	} else {
		int width = rect->getWidth();
		int height = rect->getHeight();

		if (guiButton->shouldDrawPressedWhenMouseHovers()) {
			if (guiButton->isMouseOverShape()) {
				GUI_DRAW_FRAME_PRESSED(x, y, width, height);
				return;
			}
		}

		if (guiButton->isMouseOverShape()) {
			if (mouse->isRightButtonPressed()) {
				GUI_DRAW_FRAME_PRESSED(x, y, width, height);
			}
		}

		drawGuiShape(guiButton);
	}
}

void cGuiDrawer::drawShape(cGuiShape * shapeToDraw) {
	cGuiButton * guiButton = dynamic_cast<cGuiButton*> (shapeToDraw);
	if (guiButton) {
		drawButton(guiButton);
		return;
	}

	// default drawing of shape
	drawGuiShape(shapeToDraw);
}
