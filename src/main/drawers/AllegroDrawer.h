#ifndef CALLEGRODRAWER_H_
#define CALLEGRODRAWER_H_

#include "../gameobjects/cScreenResolution.h"

class AllegroDrawer {
	public:
		AllegroDrawer(cScreenResolution * theScreenResolution);
		AllegroDrawer();
		virtual ~AllegroDrawer();

		void drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y);
		void drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x);
		void drawCenteredSprite(BITMAP *dest, BITMAP *src);

		void drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage);

	protected:
		int getCenteredXPosForBitmap(BITMAP *bmp);
		int getCenteredYPosForBitmap(BITMAP *bmp);

	private:
		cScreenResolution * screenResolution;
};

#endif /* CALLEGRODRAWER_H_ */