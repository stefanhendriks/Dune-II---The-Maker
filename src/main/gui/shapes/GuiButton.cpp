#include "GuiButton.h"

#include "../../utils/Logger.h"

extern ALFONT_FONT * bene_font;
extern BITMAP * bmp_screen;

GuiButton::GuiButton(Rectangle * rect, std::string theLabel) : GuiShape(rect) {
	assert(rect);
	enabled = true;
	drawPressedWhenMouseHovers = false;
	bitmap = NULL;
	label = theLabel;
	hasBorders = true;
	pressed = false;
	textDrawer = new TextDrawer(bene_font);
	textDrawer->setApplyShaddow(true);
}

GuiButton::GuiButton(GuiButton * other, std::string theLabel) : GuiShape(new Rectangle(other->getRectangle())) {
	assert(other);
	this->setBitmap(other->getBitmap());
	this->setColors(other->getDarkBorderColor(), other->getLightBorderColor(), other->getInnerColor());
	this->setDrawPressedWhenMouseHovers(other->shouldDrawPressedWhenMouseHovers());
	this->setHasBorders(other->shouldDrawBorders());
	rectangle = new Rectangle(other->getRectangle());
	this->enabled = other->isEnabled();
	label = theLabel;
	pressed = false;
	textDrawer = new TextDrawer(bene_font);
	textDrawer->setApplyShaddow(true);
}

GuiButton::GuiButton(int x, int y, int width, int height, std::string theLabel) : GuiShape(x, y, width, height){
	label = theLabel;
	hasBorders = true;
	drawPressedWhenMouseHovers = false;
	pressed = false;
	enabled = true;
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

	if (!isEnabled()) {
		drawDisabled();
		return;
	}

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

void GuiButton::drawDisabled() {
	if (bitmap) {
		set_trans_blender(0, 0, 0, 128);
		//rectfill(bmp_screen, rectangle->getStartX(), rectangle->getStartY(), rectangle->getEndX(), rectangle->getEndY(), guiColors.getBlack());
		drawBackground();
		// this is not giving us expected results, look at the UpgradeDrawer, line 67
		draw_trans_sprite(bmp_screen, bitmap, rectangle->getStartX(), rectangle->getStartY());
		return;
	}

	drawBackground();
	textDrawer->setShadowColor(guiColors.getWhite());
	drawLabel(guiColors.getDarkGrey());
	textDrawer->setShadowColor(guiColors.getBlack());
}

void GuiButton::drawButtonHovered() {
	drawButtonUnpressed();
	drawLabel(guiColors.getRed());
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

void GuiButton::moveButtonDownExactlyOneButtonHeight() {
	int height = 0;
	if (bitmap) {
		height = bitmap->h;
	} else {
		height = textDrawer->getHeightInPixelsForFont() + 2;
	}
	rectangle->setStartY(rectangle->getLowestY() + height);
	rectangle->setEndY(rectangle->getHighestY() + height);
}

void GuiButton::adjustHeightOfButtonToFontHeight() {
	int lowestY = rectangle->getLowestY();
	int highestY = lowestY + textDrawer->getHeightInPixelsForFont() + 1;
	rectangle->setStartY(lowestY);
	rectangle->setEndY(highestY);
}
