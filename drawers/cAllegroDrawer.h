/*
 * cAllegroDrawer.h
 *
 *  Created on: 5 dec. 2010
 *      Author: Stefan
 */

#ifndef CALLEGRODRAWER_H_
#define CALLEGRODRAWER_H_

class cAllegroDrawer {
	public:
		cAllegroDrawer();
		virtual ~cAllegroDrawer();

		cRectangle * fromBitmap(int x, int y, BITMAP *src);

		void drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y, int totalWidth, int xOffset);
		void drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x);
		void drawCenteredSprite(BITMAP *dest, BITMAP *src);
		void drawSprite(BITMAP *dest, BITMAP *src, int x, int y);

		void drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage);

		void blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
		void blitSprite(BITMAP *src, BITMAP *dest, cRectangle *rectangle);

		void stretchSprite(BITMAP *src, BITMAP *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

		void stretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
        void stretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

        void maskedBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);
        void maskedBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);

		void maskedStretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
		void maskedStretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

        void drawRectangle(BITMAP *dest, int x, int y, int width, int height, int color);
        void drawRectangle(BITMAP *dest, cRectangle *pRectangle, int color);
        void drawRectangleFilled(BITMAP *dest, cRectangle *pRectangle, int color);

        int getColor_BLACK() { return colorBlack; }

    void bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor);

    void setTransBlender(int red, int green, int blue, int alpha);

protected:
		int getCenteredXPosForBitmap(BITMAP *bmp, int totalWidth);
		int getCenteredYPosForBitmap(BITMAP *bmp);

		int colorBlack;
};

#endif /* CALLEGRODRAWER_H_ */
