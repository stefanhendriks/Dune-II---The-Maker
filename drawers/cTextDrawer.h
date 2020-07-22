/*
 * TextDrawer.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#ifndef TEXTDRAWER_H_
#define TEXTDRAWER_H_

class cTextDrawer {
	public:
		cTextDrawer();
		cTextDrawer(ALFONT_FONT *theFont);
		~cTextDrawer();
		void drawTextWithOneInteger(int x, int y, int color, char * msg, int var);
		void drawTextWithOneInteger(int x, int y, char * msg, int var);
		void drawTextWithTwoIntegers(int x, int y, char * msg, int var1, int var2);
		void drawText(int x, int y, char * msg);
		void drawText(int x, int y, int color, char *msg);

		void drawTextCentered(char * msg, int y);
		void drawTextCentered(char * msg, int y, int color);
        void drawTextCentered(char * msg, int x, int width, int y, int color);

		void drawTextBottomRight(char * msg);
		void drawTextBottomLeft(char * msg);

		void drawTextBottomRight(int color, char * msg);
		void drawTextBottomLeft(int color, char * msg);

		void setApplyShadow(bool value) { applyShadow = value; }

		int textLength(const char * msg);

	protected:

	private:
		ALFONT_FONT *font;
		bool applyShadow;

};

#endif /* TEXTDRAWER_H_ */
