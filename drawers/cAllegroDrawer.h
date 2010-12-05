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

		void drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y);
		void drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x);
		void drawCenteredSprite(BITMAP *dest, BITMAP *src);

		void drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage);

	protected:
		int getCenteredXPosForBitmap(BITMAP *bmp);
		int getCenteredYPosForBitmap(BITMAP *bmp);
};

#endif /* CALLEGRODRAWER_H_ */
