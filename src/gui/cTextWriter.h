/*
 * cTextWriter.h
 *
 *  Created on: 1-nov-2010
 *      Author: Stefan
 */

#ifndef CTEXTWRITER_H_
#define CTEXTWRITER_H_

// a text writer has state, meaning with every command to 'write' something, it knows where to draw it
// every new command it will start on a new line.

class cTextWriter {
	public:
		cTextWriter(int x, int y, ALFONT_FONT *theFont, int theFontSize);
		~cTextWriter();

		void write(char *msg);
		void write(char *msg, int color);
		void writeWithOneInteger(char * msg, int value1);
		void writeWithTwoIntegers(char * msg, int value1, int value2);

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

#endif /* CTEXTWRITER_H_ */
