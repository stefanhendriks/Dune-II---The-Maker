/*
 * cGuiDrawer.cpp
 *
 *  Created on: 27 feb. 2011
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

GuiDrawer::GuiDrawer() {
}

GuiDrawer::~GuiDrawer() {
}

void GuiDrawer::drawGuiShape(GuiShape * shape) {
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

void GuiDrawer::drawButton(GuiButton * guiButton) {
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

		if (guiButton->shouldDrawPressedWhenMouseHovers()) {
			if (guiButton->isMouseOverShape()) {
				drawButtonPressed(guiButton);
				return;
			}
		}

		if (guiButton->isMouseOverShape()) {
			if (mouse->isRightButtonPressed()) {
				drawButtonPressed(guiButton);
				return;
			}
		}

		drawButtonUnpressed(guiButton);
	}
}

void GuiDrawer::drawButtonPressed(GuiButton * guiButton) {

	drawBackground(guiButton);
	if (guiButton->shouldDrawBorders()) {
		drawDarkerBorder(guiButton);

		cRectangle * rct = guiButton->getRectangle();
		line(bmp_screen, rct->getHighestX(), rct->getLowestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getLightBorderColor());
		line(bmp_screen, rct->getLowestX(), rct->getHighestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getLightBorderColor());
	}
}

void GuiDrawer::drawLighterBorder(GuiButton * guiButton) {
	cRectangle * rct = guiButton->getRectangle();
	rect(bmp_screen, rct->getLowestX(), rct->getLowestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getLightBorderColor());
}

void GuiDrawer::drawDarkerBorder(GuiButton * guiButton) {
	cRectangle * rct = guiButton->getRectangle();
	rect(bmp_screen, rct->getLowestX(), rct->getLowestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getDarkBorderColor());
}

void GuiDrawer::drawButtonUnpressed(GuiButton * guiButton) {

	drawBackground(guiButton);
	if (guiButton->shouldDrawBorders()) {
		drawLighterBorder(guiButton);

		cRectangle * rct = guiButton->getRectangle();
		line(bmp_screen, rct->getHighestX(), rct->getLowestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getDarkBorderColor());
		line(bmp_screen, rct->getLowestX(), rct->getHighestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getDarkBorderColor());
	}
}

void GuiDrawer::drawBackground(GuiButton * guiButton) {
	cRectangle * rct = guiButton->getRectangle();
	rectfill(bmp_screen, rct->getLowestX(), rct->getLowestY(), rct->getHighestX(), rct->getHighestY(), guiButton->getInnerColor());
}

void GuiDrawer::drawShape(GuiShape * shapeToDraw) {
	GuiButton * guiButton = dynamic_cast<GuiButton*> (shapeToDraw);
	if (guiButton) {
		drawButton(guiButton);
		return;
	}

	MainMenuDialog * mainMenuDialog = dynamic_cast<MainMenuDialog*> (shapeToDraw);
	if (mainMenuDialog) {

	}

	// default drawing of shape
	drawGuiShape(shapeToDraw);
}
