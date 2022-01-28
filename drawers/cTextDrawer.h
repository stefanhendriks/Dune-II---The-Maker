/*
 * TextDrawer.h
 *
 *  Created on: 31-okt-2010
 *      Author: Stefan
 */

#pragma once

#include "utils/cRectangle.h"

struct ALFONT_FONT;

class cTextDrawer {
	public:
		cTextDrawer();
		cTextDrawer(ALFONT_FONT *theFont);
		~cTextDrawer();

		void setFont(ALFONT_FONT *theFont);

		void drawTextWithOneInteger(int x, int y, int color, const char * msg, int var) const;
		void drawTextWithOneInteger(int x, int y, const char * msg, int var) const;
		void drawTextWithTwoIntegers(int x, int y, const char * msg, int var1, int var2) const;

        void drawText(int x, int y, const char * msg, const char *var) const;
		void drawText(int x, int y, const char * msg, int var) const;
		void drawText(int x, int y, const char * msg) const;

        void drawText(cPoint &coords, int color, const char *msg) const;

        void drawText(int x, int y, int color, const char *msg) const;
        void drawText(int x, int y, int color, const char *msg, int var) const;
        void drawText(int x, int y, const char *msg, int var1, int var2) const;
		void drawText(int x, int y, int color, const char *msg, const char *var) const;

		void drawTextCentered(const char * msg, int y) const;
		void drawTextCentered(const char * msg, int y, int color) const;
        void drawTextCentered(const char * msg, int x, int width, int y, int color) const;

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
        void drawTextCenteredInBox(const char * msg, int x, int y, int boxWidth, int boxHeight, int color) const;

        void drawTextCenteredInBox(const char * msg, const cRectangle & rect, int color) const;

        void drawTextCenteredInBox(const char * msg, const cRectangle & rect, int color, int offsetX, int offsetY) const;

		void drawTextBottomRight(const char * msg) const;
		void drawTextBottomLeft(const char * msg) const;

		void drawTextBottomRight(int color, const char * msg) const;
		void drawTextBottomLeft(int color, const char * msg) const;

		void setApplyShadow(bool value) { applyShadow = value; }

        void setTextColor(int value) { textColor = value; }
		int textLength(const char * msg) const;
        int getFontHeight() const;

        cRectangle getRect(int x, int y, const char * msg) const;
        cRectangle * getAsRectangle(int x, int y, const char * msg) const;
	protected:

	private:
		ALFONT_FONT *font;
		bool applyShadow;

        int textColor;
};
