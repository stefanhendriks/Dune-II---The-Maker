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

void cTextDrawer::drawTextWithTwoIntegers(int x, int y, char * msg, int var1, int var2) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var1, var2);
	}
	alfont_textprintf(bmp_screen, font, x,y, makecol(255,255,255), msg, var1, var2);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, char * msg, int var) {
    drawTextWithOneInteger(x, y, makecol(255, 255, 255), msg, var);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, int color, char * msg, int var) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg, var);
	}
	alfont_textprintf(bmp_screen, font, x,y, color, msg, var);
}

void cTextDrawer::drawText(int x, int y, int color, char *msg) {
	if (applyShadow) {
		alfont_textprintf(bmp_screen, font, x + 1,y + 1, makecol(0,0,0), msg);
	}
	alfont_textprintf(bmp_screen, font, x,y, color, msg);
}

void cTextDrawer::drawText(int x, int y, char * msg) {
    drawText(x, y, makecol(255, 255, 255), msg);
}

void cTextDrawer::drawTextCentered(char *msg, int y) {
	drawTextCentered(msg, y, makecol(255, 255, 255));
}

void cTextDrawer::drawTextCentered(char * msg, int y, int color) {
	int lenghtInPixels = alfont_text_length(font, msg);
	int half = lenghtInPixels / 2;
	int xPos = (game.screen_x / 2) - half;
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextCentered(char * msg, int x, int width, int y, int color) {
	int lenghtInPixels = alfont_text_length(font, msg);
	int half = lenghtInPixels / 2;
	int xPos = x + ((width / 2) - half);
    drawText(xPos, y, color, msg);
}

void cTextDrawer::drawTextBottomRight(char * msg) {
    drawTextBottomRight(makecol(255, 255, 255), msg);
}

void cTextDrawer::drawTextBottomLeft(char * msg) {
    drawTextBottomLeft(makecol(255,255,255), msg);
}

void cTextDrawer::drawTextBottomRight(int color, char * msg) {
	int lenghtInPixels = alfont_text_length(font, msg);
	int x = game.screen_x - lenghtInPixels;
	int y = game.screen_y - alfont_text_height(font);
	drawText(x, y, color, msg);
}

void cTextDrawer::drawTextBottomLeft(int color, char * msg) {
	int y = game.screen_y - alfont_text_height(font);
	drawText(0, y, color, msg);
}

int cTextDrawer::textLength(const char * msg) {
    return alfont_text_length(font, msg);
}