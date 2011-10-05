#include "GuiButton.h"

extern BITMAP *bmp_screen;

GuiButton::GuiButton(cRectangle * rect, std::string theLabel) : GuiShape(rect) {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
	label = &theLabel;
	hasBorders = true;
	pressed = false;
}

GuiButton::GuiButton(int x, int y, int width, int height, std::string theLabel) : GuiShape(x, y, width, height){
	label = &theLabel;
	hasBorders = true;
	drawPressedWhenMouseHovers = false;
	pressed = false;
}

GuiButton::~GuiButton() {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
}

void GuiButton::draw() {
	int x = rectangle->getStartX();
	int y = rectangle->getStartY();

	if (bitmap) {
		draw_sprite(bmp_screen, bitmap, x, y);
		return;
	}

	if (isPressed()) {
		drawButtonPressed();
		return;
	}

	drawButtonUnpressed();
}

void GuiButton::drawBackground() {
	rectfill(bmp_screen, rectangle->getLowestX(), rectangle->getLowestY(), rectangle->getHighestX(), rectangle->getHighestY(), getInnerColor());
}

void GuiButton::drawLighterBorder() {
	rect(bmp_screen, rectangle->getLowestX(), rectangle->getLowestY(), rectangle->getHighestX(), rectangle->getHighestY(), getLightBorderColor());
}

void GuiButton::drawDarkerBorder() {
	rect(bmp_screen, rectangle->getLowestX(), rectangle->getLowestY(), rectangle->getHighestX(), rectangle->getHighestY(), getDarkBorderColor());
}

void GuiButton::drawButtonUnpressed() {
	drawBackground();
	if (shouldDrawBorders()) {
		drawLighterBorder();

		line(bmp_screen, rectangle->getHighestX(), rectangle->getLowestY(), rectangle->getHighestX(), rectangle->getHighestY(), getDarkBorderColor());
		line(bmp_screen, rectangle->getLowestX(), rectangle->getHighestY(), rectangle->getHighestX(), rectangle->getHighestY(), getDarkBorderColor());
	}
}

void GuiButton::drawButtonPressed() {
	drawBackground();
	if (shouldDrawBorders()) {
		drawDarkerBorder();

		line(bmp_screen, rectangle->getHighestX(), rectangle->getLowestY(), rectangle->getHighestX(), rectangle->getHighestY(), getLightBorderColor());
		line(bmp_screen, rectangle->getLowestX(), rectangle->getHighestY(), rectangle->getHighestX(), rectangle->getHighestY(), getLightBorderColor());
	}
}
