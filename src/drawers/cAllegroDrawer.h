#pragma once

#include "data/cAllegroDataRepository.h"
#include "utils/cRectangle.h"

#include <map>
#include <memory>

// Forward declarations to prevent including Allegro headers
struct BITMAP;

class cAllegroDrawer {
	public:
		cAllegroDrawer(cAllegroDataRepository * dataRepository);
		virtual ~cAllegroDrawer();

		// cRectangle * fromBitmap(int x, int y, BITMAP *src);

		void drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y, int totalWidth, int xOffset);
		void drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x);
		void drawCenteredSprite(BITMAP *dest, BITMAP *src);
    void drawTransSprite(BITMAP *sprite, BITMAP *dest, int x, int y);

    void drawSprite(BITMAP *dest, BITMAP *src, int x, int y);
		void drawSprite(BITMAP *dest, int index, int x, int y);

		void resetClippingFor(BITMAP *bmp);
		void setClippingFor(BITMAP *bmp, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);

		void drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage);

		// void blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
		void blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) const;
		void blitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
		void blitSprite(BITMAP *src, BITMAP *dest, const cRectangle *rectangle) const;

		void stretchSprite(BITMAP *src, BITMAP *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

		void stretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
    void stretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    void maskedBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);
    void maskedBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);

		void maskedStretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
		void maskedStretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    void drawRectangle(BITMAP *dest, int x, int y, int width, int height, int color);
    void drawRectangle(BITMAP *dest, const cRectangle &pRectangle, int color);
    void drawRectangleFilled(BITMAP *dest, int x, int y, int width, int height, int color);
    void drawRectangleFilled(BITMAP *dest, const cRectangle &pRectangle, int color);
    void drawRectangleTransparentFilled(BITMAP *dest, const cRectangle& rect, int color, int alpha);

    int getColor_BLACK() { return colorBlack; }

    void bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor);

    void setTransBlender(int red, int green, int blue, int alpha);

    int getColorByNormValue(int r, int g, int b, float norm);

    void gui_DrawRect(BITMAP *dest, const cRectangle &rectangle);
    void gui_DrawRect(BITMAP *dest, const cRectangle &rectangle, int gui_colorWindow, int gui_colorBorderLight, int gui_colorBorderDark);
    void gui_DrawRectBorder(BITMAP *dest, const cRectangle &rectangle, int gui_colorBorderLight, int gui_colorBorderDark);

    void drawLine(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);

    void drawDot(BITMAP *bmp, int x, int y, int color, int size);

protected:
    int getCenteredXPosForBitmap(BITMAP *bmp, int totalWidth);
    int getCenteredYPosForBitmap(BITMAP *bmp);

    int colorBlack;

    int gui_colorWindow;
    int gui_colorBorderLight;
    int gui_colorBorderDark;

private:
    cAllegroDataRepository * m_dataRepository;
    struct sSize{
        int width;
        int height;

        bool operator<(sSize rhs) const { return width == rhs.width ? height < rhs.height : width < rhs.width; }
    };
    std::map<sSize, BITMAP*> bitmapCache;
};
