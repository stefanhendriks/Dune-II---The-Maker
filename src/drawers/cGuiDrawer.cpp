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

void cGuiDrawer::drawShape(cGuiShape * shapeToDraw) {
	cRectangle * rect = shapeToDraw->getRectangle();
	int x = rect->getStartX();
	int y = rect->getStartY();
	int width = rect->getWidth();
	int height = rect->getHeight();

	if (shapeToDraw->isMouseOverShape()) {
		GUI_DRAW_FRAME_PRESSED(x, y, width, height);
	} else {
		GUI_DRAW_FRAME(x, y, width, height);
	}
}
