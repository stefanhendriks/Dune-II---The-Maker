#ifndef TEXTDRAWER_H_
#define TEXTDRAWER_H_

#include <alfont.h>

class TextDrawer {
	public:
		TextDrawer();
		TextDrawer(ALFONT_FONT *theFont);
		~TextDrawer();
		void drawTextWithOneInteger(int x, int y, const char * msg, int var);
		void drawTextWithTwoIntegers(int x, int y, const char * msg, int var1, int var2);
		void drawText(int x, int y, const char * msg);
		void drawText(int x, int y, const char * msg, int color);

		void drawTextCentered(const char * msg, int y);
		void drawTextCentered(const char * msg, int y, int color);

		void drawTextBottomRight(const char * msg);
		void drawTextBottomLeft(const char * msg);

		void setApplyShaddow(bool value) {
			applyShadow = value;
		}

		void setTextColor(int value) {
			textColor = value;
		}

		int getYCoordinateForBottomRight();
		int getXCoordinateForBottomRight(const char * msg);

	protected:
		int getLenghtInPixelsForChar(const char * msg);

	private:
		bool applyShadow;
		int textColor;
		ALFONT_FONT * font;
};

#endif /* TEXTDRAWER_H_ */
