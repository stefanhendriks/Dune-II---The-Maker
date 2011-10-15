#include "GuiButton.h"

#include "../../utils/Logger.h"

extern ALFONT_FONT * bene_font;
extern BITMAP * bmp_screen;

GuiButton::GuiButton(Rectangle * rect, std::string theLabel) : GuiShape(rect) {
	assert(rect);
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
	label = theLabel;
	hasBorders = true;
	pressed = false;
	textDrawer = new TextDrawer(bene_font);
	textDrawer->setApplyShaddow(true);
}

GuiButton::GuiButton(int x, int y, int width, int height, std::string theLabel) : GuiShape(x, y, width, height){
	label = theLabel;
	hasBorders = true;
	drawPressedWhenMouseHovers = false;
	pressed = false;
	bitmap = NULL;
	textDrawer = new TextDrawer(bene_font);
	textDrawer->setApplyShaddow(true);
}

GuiButton::~GuiButton() {
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
	delete textDrawer;
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

	if (isMouseOverShape()) {
		drawButtonHovered();
		return;
	}

	drawButtonUnpressed();
}

void GuiButton::drawButtonHovered() {
	drawButtonUnpressed();
	drawLabel(guiColors.getMenuYellow());
}

void GuiButton::drawLabel(int color) {
	textDrawer->setTextColor(color);
	textDrawer->drawText(rectangle->getLowestX() + 1, rectangle->getLowestY() + 1, label.c_str());
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

	drawLabel(guiColors.getWhite());
}

void GuiButton::drawButtonPressed() {
	drawBackground();
	if (shouldDrawBorders()) {
		drawDarkerBorder();

		line(bmp_screen, rectangle->getHighestX(), rectangle->getLowestY(), rectangle->getHighestX(), rectangle->getHighestY(), getLightBorderColor());
		line(bmp_screen, rectangle->getLowestX(), rectangle->getHighestY(), rectangle->getHighestX(), rectangle->getHighestY(), getLightBorderColor());
	}

	drawLabel(guiColors.getMenuDarkBorder());
}
