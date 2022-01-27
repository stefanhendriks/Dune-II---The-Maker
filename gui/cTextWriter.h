/*
 * cTextWriter.h
 *
 *  Created on: 1-nov-2010
 *      Author: Stefan
 */

#pragma once

#include "drawers/cTextDrawer.h"

struct ALFONT_FONT;

// a text writer has state, meaning with every command to 'write' something, it knows where to draw it
// every new command it will start on a new line.


class cTextWriter {
	public:
		cTextWriter(int x, int y, ALFONT_FONT *theFont, int theFontSize);
		~cTextWriter();

		void write(const char *msg);
		void write(const char *msg, int color);
		void writeWithOneInteger(const char * msg, int value1);
		void writeWithTwoIntegers(const char * msg, int value1, int value2);

	protected:
		void updateDrawY();

	private:
		cTextDrawer *textDrawer;
		ALFONT_FONT *font;
		int fontSize;
		int originalX;
		int originalY;
		int drawY;
		int drawX;
};
