/*
 * TextDrawer.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cTextDrawer::cTextDrawer(ALFONT_FONT *theFont) {
	assert(theFont);
	font = theFont;
	applyShadow=true;
}

cTextDrawer::cTextDrawer() {
	font = small_font;
}

cTextDrawer::~cTextDrawer() {
	font = NULL;
}

void cTextDrawer::drawTextWithTwoIntegers(int x, int y, const char * msg, int var1, int var2) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var1, var2);
	}
	alfont_textprintf(bmp_screen, font, x,y, makecol(255,255,255), msg, var1, var2);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, const char * msg, int var) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var);
	}
	alfont_textprintf(bmp_screen, font, x,y, makecol(255,255,255), msg, var);
}

void cTextDrawer::drawText(int x, int y, const char * msg, int color) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg);
	}
	alfont_textprintf(bmp_screen, font, x,y, color, msg);
}

void cTextDrawer::drawText(int x, int y, const char * msg) {
	drawText(x, y, msg, makecol(255,255,255));
}

void cTextDrawer::drawTextCentered(const char *msg, int y) {
	drawTextCentered(msg, y, makecol(255, 255, 255));
}

void cTextDrawer::drawTextCentered(const char * msg, int y, int color) {
	int lenghtInPixels = alfont_text_length(font, msg);
	int half = lenghtInPixels / 2;
	int xPos = (game.getScreenResolution()->getWidth() / 2) - half;
	drawText(xPos, y, msg, color);
}

void cTextDrawer::drawTextBottomRight(const char * msg) {
	int lenghtInPixels = alfont_text_length(font, msg);
	int x = game.getScreenResolution()->getWidth() - lenghtInPixels;
	int y = game.getScreenResolution()->getHeight() - alfont_text_height(font);
	drawText(x, y, msg);
}

void cTextDrawer::drawTextBottomLeft(const char * msg) {
	int y = game.getScreenResolution()->getHeight() - alfont_text_height(font);
	drawText(0, y, msg);
}
