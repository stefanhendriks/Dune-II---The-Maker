/*
 * TextDrawer.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

TextDrawer::TextDrawer(ALFONT_FONT *theFont) {
	assert(theFont);
	font = theFont;
	applyShadow = true;
	textColor = guiColors.getWhite();
	shadowColor = guiColors.getBlack();
}

TextDrawer::TextDrawer() {
	font = small_font;
	applyShadow = true;
	textColor = guiColors.getWhite();
	shadowColor = guiColors.getBlack();
}

TextDrawer::~TextDrawer() {
	font = NULL;
}

void TextDrawer::drawTextWithTwoIntegers(int x, int y, const char * msg, int var1, int var2) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1, y + 1, shadowColor, msg, var1, var2);
	}
	alfont_textprintf(bmp_screen, font, x, y, textColor, msg, var1, var2);
}

void TextDrawer::drawTextWithOneInteger(int x, int y, const char * msg, int var) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1, y + 1, shadowColor, msg, var);
	}
	alfont_textprintf(bmp_screen, font, x, y, textColor, msg, var);
}

void TextDrawer::drawText(int x, int y, const char * msg, int color) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1, y + 1, shadowColor, msg);
	}
	alfont_textprintf(bmp_screen, font, x, y, color, msg);
}

void TextDrawer::drawText(int x, int y, const char * msg) {
	drawText(x, y, msg, textColor);
}

void TextDrawer::drawTextCentered(const char *msg, int y) {
	drawTextCentered(msg, y, textColor);
}

void TextDrawer::drawTextCentered(const char * msg, int y, int color) {
	int lenghtInPixels = getLenghtInPixelsForChar(msg);
	int half = lenghtInPixels / 2;
	int xPos = (game.getScreenResolution()->getWidth() / 2) - half;
	drawText(xPos, y, msg, color);
}



void TextDrawer::drawTextBottomRight(const char * msg) {
	int x = getXCoordinateForBottomRight(msg);
	int y = getYCoordinateForBottomRight();
	drawText(x, y, msg);
}

int TextDrawer::getXCoordinateForBottomRight(const char * msg) {
	return game.getScreenResolution()->getWidth() - getLenghtInPixelsForChar(msg);
}

int TextDrawer::getHeightInPixelsForFont() {
	return alfont_text_height(font);
}

int TextDrawer::getLenghtInPixelsForChar(const char * msg) {
	return alfont_text_length(font, msg);
}

int TextDrawer::getYCoordinateForBottomRight() {
	return game.getScreenResolution()->getHeight() - alfont_text_height(font);
}

void TextDrawer::drawTextBottomLeft(const char * msg) {
	int y = game.getScreenResolution()->getHeight() - alfont_text_height(font);
	drawText(0, y, msg);
}
