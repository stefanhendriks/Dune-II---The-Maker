/*
 * TextDrawer.cpp
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cTextDrawer::cTextDrawer() {
}

cTextDrawer::~cTextDrawer() {
}

void cTextDrawer::drawTextWithTwoIntegers(int x, int y, char * msg, int var1, int var2) {
	alfont_textprintf(bmp_screen, small_font, x + 1,y + 1, makecol(0,0,0), msg, var1, var2);
	alfont_textprintf(bmp_screen, small_font, x,y, makecol(255,255,255), msg, var1, var2);
}

void cTextDrawer::drawTextWithOneInteger(int x, int y, char * msg, int var) {
	alfont_textprintf(bmp_screen, small_font, x + 1,y + 1, makecol(0,0,0), msg, var);
	alfont_textprintf(bmp_screen, small_font, x,y, makecol(255,255,255), msg, var);
}

void cTextDrawer::drawText(int x, int y, char * msg) {
	alfont_textprintf(bmp_screen, small_font, x + 1,y + 1, makecol(0,0,0), msg);
	alfont_textprintf(bmp_screen, small_font, x,y, makecol(255,255,255), msg);
}
