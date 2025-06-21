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

    // cRectangle * fromBitmap(int x, int y, SDL_Surface *src);

    void drawCenteredSpriteHorizontal(SDL_Surface *dest, SDL_Surface *src, int y, int totalWidth, int xOffset);
    void drawCenteredSpriteVertical(SDL_Surface *dest, SDL_Surface *src, int x);
    void drawCenteredSprite(SDL_Surface *dest, SDL_Surface *src);
    void drawTransSprite(SDL_Surface *sprite, SDL_Surface *dest, int x, int y);

    void drawSprite(SDL_Surface *dest, SDL_Surface *src, int x, int y);
    void drawSprite(SDL_Surface *dest, int index, int x, int y);

    void resetClippingFor(SDL_Surface *bmp);
    void setClippingFor(SDL_Surface *bmp, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);

    void drawSpriteCenteredRelativelyVertical(SDL_Surface *dest, SDL_Surface *src, float percentage);

    // void blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
    void blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) const;
    void blitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
    void blitSprite(SDL_Surface *src, SDL_Surface *dest, const cRectangle *rectangle) const;

    void stretchSprite(SDL_Surface *src, SDL_Surface *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    void stretchBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
    void stretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    void maskedBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);
    void maskedBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);

    void maskedStretchBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
    void maskedStretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    void drawRect(SDL_Surface *dest, int x, int y, int width, int height, int color);
    void drawRect(SDL_Surface *dest, const cRectangle &pRectangle, int color);
    void drawRectFilled(SDL_Surface *dest, int x, int y, int width, int height, int color);
    void drawRectFilled(SDL_Surface *dest, const cRectangle &pRectangle, int color);
    void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, int color, int alpha);

    void drawRect(SDL_Surface *dest, int x, int y, int width, int height, SDL_Color color);
    void drawRect(SDL_Surface *dest, const cRectangle &pRectangle, SDL_Color color);
    void drawRectFilled(SDL_Surface *dest, int x, int y, int width, int height, SDL_Color color);
    void drawRectFilled(SDL_Surface *dest, const cRectangle &pRectangle, SDL_Color color);
    void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, SDL_Color color, int alpha);
    void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, SDL_Color color);


    SDL_Color getColor_BLACK() {
        return SDL_Color{0,0,0,255};
    }

    void bitmap_replace_color(SDL_Surface *bmp, int colorToReplace, int newColor);
    void bitmap_replace_color(SDL_Surface *bmp, SDL_Color colorToReplace, SDL_Color newColor);

    void setTransBlender(int red, int green, int blue, int alpha);

    int getColorByNormValue(int r, int g, int b, float norm);

    void gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle);
    
    void gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle, int gui_colorWindow, int gui_colorBorderLight, int gui_colorBorderDark);
    void gui_DrawRectBorder(SDL_Surface *dest, const cRectangle &rectangle, int gui_colorBorderLight, int gui_colorBorderDark);
    void drawLine(SDL_Surface *bmp, int x1, int y1, int x2, int y2, int color);

    void gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle, SDL_Color gui_colorWindow, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark);
    void gui_DrawRectBorder(SDL_Surface *dest, const cRectangle &rectangle, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark);
    void drawLine(SDL_Surface *bmp, int x1, int y1, int x2, int y2, SDL_Color color);


    void drawDot(SDL_Surface *bmp, int x, int y, int color, int size);
    void drawDot(SDL_Surface *bmp, int x, int y, SDL_Color color, int size);
    //Scramble the pixels. In a radius "r" around position X,Y at the screen! (the x,y are screencoordinates!)
    void shimmer(int r, int x, int y, float cameraZoom);
    void set_pixel(SDL_Surface *bmp, int x, int y, SDL_Color color);
protected:
    int getCenteredXPosForBitmap(SDL_Surface *bmp, int totalWidth);
    int getCenteredYPosForBitmap(SDL_Surface *bmp);

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
    std::map<sSize, SDL_Surface *> bitmapCache;
};
