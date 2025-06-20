#pragma once

#include "data/cAllegroDataRepository.h"
#include "utils/cRectangle.h"

#include <map>
#include <memory>
#include <SDL2/SDL.h>
// Forward declarations to prevent including Allegro headers
struct BITMAP;

class cAllegroDrawer {
public:
    cAllegroDrawer(cAllegroDataRepository *dataRepository);
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

    void drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP *src, float percentage);

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

    void drawRect(BITMAP *dest, int x, int y, int width, int height, int color);
    void drawRect(BITMAP *dest, const cRectangle &pRectangle, int color);
    void drawRectFilled(BITMAP *dest, int x, int y, int width, int height, int color);
    void drawRectFilled(BITMAP *dest, const cRectangle &pRectangle, int color);
    void drawRectTransparentFilled(BITMAP *dest, const cRectangle &rect, int color, int alpha);

    void drawRect(BITMAP *dest, int x, int y, int width, int height, SDL_Color color);
    void drawRect(BITMAP *dest, const cRectangle &pRectangle, SDL_Color color);
    void drawRectFilled(BITMAP *dest, int x, int y, int width, int height, SDL_Color color);
    void drawRectFilled(BITMAP *dest, const cRectangle &pRectangle, SDL_Color color);
    void drawRectTransparentFilled(BITMAP *dest, const cRectangle &rect, SDL_Color color, int alpha);
    void drawRectTransparentFilled(BITMAP *dest, const cRectangle &rect, SDL_Color color);


    SDL_Color getColor_BLACK() {
        return SDL_Color{0,0,0,255};
    }

    void bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor);
    void bitmap_replace_color(BITMAP *bmp, SDL_Color colorToReplace, SDL_Color newColor);

    void setTransBlender(int red, int green, int blue, int alpha);

    int getColorByNormValue(int r, int g, int b, float norm);

    void gui_DrawRect(BITMAP *dest, const cRectangle &rectangle);
    
    void gui_DrawRect(BITMAP *dest, const cRectangle &rectangle, int gui_colorWindow, int gui_colorBorderLight, int gui_colorBorderDark);
    void gui_DrawRectBorder(BITMAP *dest, const cRectangle &rectangle, int gui_colorBorderLight, int gui_colorBorderDark);
    void drawLine(BITMAP *bmp, int x1, int y1, int x2, int y2, int color);

    void gui_DrawRect(BITMAP *dest, const cRectangle &rectangle, SDL_Color gui_colorWindow, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark);
    void gui_DrawRectBorder(BITMAP *dest, const cRectangle &rectangle, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark);
    void drawLine(BITMAP *bmp, int x1, int y1, int x2, int y2, SDL_Color color);


    void drawDot(BITMAP *bmp, int x, int y, int color, int size);
    void drawDot(BITMAP *bmp, int x, int y, SDL_Color color, int size);
    //Scramble the pixels. In a radius "r" around position X,Y at the screen! (the x,y are screencoordinates!)
    void shimmer(int r, int x, int y, float cameraZoom);
    void set_pixel(BITMAP *bmp, int x, int y, SDL_Color color);
protected:
    int getCenteredXPosForBitmap(BITMAP *bmp, int totalWidth);
    int getCenteredYPosForBitmap(BITMAP *bmp);

    int colorBlack;

    int gui_colorWindow;
    int gui_colorBorderLight;
    int gui_colorBorderDark;

private:
    cAllegroDataRepository *m_dataRepository;
    struct sSize {
        int width;
        int height;

        bool operator<(sSize rhs) const {
            return width == rhs.width ? height < rhs.height : width < rhs.width;
        }
    };
    std::map<sSize, BITMAP *> bitmapCache;
};
