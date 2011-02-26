/*
 * cTextWriter.cpp
 *
 *  Created on: 1-nov-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cTextWriter::cTextWriter(int x, int y, ALFONT_FONT *theFont, int theFontSize) {
	assert(theFont);
	assert(theFontSize > 0);
	originalX = x;
	drawX = x;
	originalY = y;
	drawY = y;
	font = theFont;
	fontSize = theFontSize;
	textDrawer = new cTextDrawer();
}

cTextWriter::~cTextWriter() {
	font = NULL;
	delete textDrawer;
}

void cTextWriter::write(const char * msg) {
	textDrawer->drawText(drawX, drawY, msg);
	updateDrawY();
}

void cTextWriter::write(const char * msg, int color) {
	textDrawer->drawText(drawX, drawY, msg, color);
	updateDrawY();
}

void cTextWriter::writeWithOneInteger(const char * msg, int value1) {
	textDrawer->drawTextWithOneInteger(drawX, drawY, msg, value1);
	updateDrawY();
}

void cTextWriter::writeWithTwoIntegers(const char * msg, int value1, int value2) {
	textDrawer->drawTextWithTwoIntegers(drawX, drawY, msg, value1, value2);
	updateDrawY();
}

void cTextWriter::updateDrawY() {
	drawY += fontSize;
}
