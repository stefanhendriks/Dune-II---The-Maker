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
	int x = shapeToDraw->getX();
	int y = shapeToDraw->getY();
	int width = shapeToDraw->getWidth();
	int height = shapeToDraw->getHeight();
	GUI_DRAW_FRAME(x, y, width, height);
}
