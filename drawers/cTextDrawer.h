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

		void setFont(ALFONT_FONT *theFont);

		void drawTextWithOneFloat(int x, int y, int color, const char * msg, float var);
		void drawTextWithOneInteger(int x, int y, int color, const char * msg, int var);
		void drawTextWithOneInteger(int x, int y, const char * msg, int var);
		void drawTextWithTwoIntegers(int x, int y, const char * msg, int var1, int var2);

        void drawText(int x, int y, const char * msg, const char *var);
		void drawText(int x, int y, const char * msg, int var);
		void drawText(int x, int y, const char * msg);

        void drawText(int x, int y, int color, const char *msg);
        void drawText(int x, int y, int color, const char *msg, int var);
		void drawText(int x, int y, int color, const char *msg, const char *var);

		void drawTextCentered(const char * msg, int y);
		void drawTextCentered(const char * msg, int y, int color);
        void drawTextCentered(const char * msg, int x, int width, int y, int color);

        /**
         * Draws the text within a 'box' (rectangle), positioned at X,Y. The width/height of the box determine
         * how to center the text. When centering text, the height/width of the font is taken into account.
         *
         * @param msg
         * @param x
         * @param y
         * @param boxWidth
         * @param boxHeight
         * @param color
         */
        void drawTextCenteredInBox(const char * msg, int x, int y, int boxWidth, int boxHeight, int color);

		void drawTextBottomRight(const char * msg);
		void drawTextBottomLeft(const char * msg);

		void drawTextBottomRight(int color, const char * msg);
		void drawTextBottomLeft(int color, const char * msg);

		void setApplyShadow(bool value) { applyShadow = value; }

		int textLength(const char * msg);
        int getFontHeight() const;

        cRectangle * getAsRectangle(int x, int y, const char * msg);
	protected:

	private:
		ALFONT_FONT *font;
		bool applyShadow;

};

#endif /* TEXTDRAWER_H_ */
