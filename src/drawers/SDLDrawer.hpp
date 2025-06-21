#pragma once

#include "data/cAllegroDataRepository.h"
#include "utils/cRectangle.h"

#include <map>
#include <memory>
#include <SDL2/SDL.h>


class SDLDrawer {
public:
    SDLDrawer(cAllegroDataRepository *dataRepository, SDL_Renderer* _renderer);
    virtual ~SDLDrawer();

    void drawCenteredSpriteHorizontal(SDL_Surface *dest, SDL_Surface *src, int y, int totalWidth, int xOffset);
    void drawCenteredSpriteVertical(SDL_Surface *dest, SDL_Surface *src, int x);
    void drawCenteredSprite(SDL_Surface *dest, SDL_Surface *src);
    void drawTransSprite(SDL_Surface *sprite, SDL_Surface *dest, int x, int y);

    void drawSprite(SDL_Surface *dest, SDL_Surface *src, int x, int y);
    void drawSprite(SDL_Surface *dest, int index, int x, int y);

    void resetClippingFor(SDL_Surface *bmp);
    void setClippingFor(SDL_Surface *bmp, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);

    void drawSpriteCenteredRelativelyVertical(SDL_Surface *dest, SDL_Surface *src, float percentage);

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

    void drawRect(SDL_Surface *dest, int x, int y, int width, int height, SDL_Color color);
    void drawRect(SDL_Surface *dest, const cRectangle &pRectangle, SDL_Color color);
    void drawRectFilled(SDL_Surface *dest, int x, int y, int width, int height, SDL_Color color);
    void drawRectFilled(SDL_Surface *dest, const cRectangle &pRectangle, SDL_Color color);
    void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, SDL_Color color, int alpha);
    void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, SDL_Color color);
    void setPixel(SDL_Surface *bmp, int x, int y, SDL_Color color);
    SDL_Color getColor_BLACK() {
        return colorBlack;
    }

    void bitmap_replace_color(SDL_Surface *bmp, int colorToReplace, int newColor);
    void bitmap_replace_color(SDL_Surface *bmp, SDL_Color colorToReplace, SDL_Color newColor);

    void setTransBlender(int red, int green, int blue, int alpha);

    SDL_Color getColorByNormValue(int r, int g, int b, float norm);

    void gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle);
    void gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle, SDL_Color gui_colorWindow, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark);
    void gui_DrawRectBorder(SDL_Surface *dest, const cRectangle &rectangle, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark);

    void drawLine(SDL_Surface *bmp, int x1, int y1, int x2, int y2, SDL_Color color);
    void drawDot(SDL_Surface *bmp, int x, int y, SDL_Color color, int size);

    void shimmer(SDL_Surface *src, int r, int x, int y, float cameraZoom);

    void FillWithColor(SDL_Surface *src, SDL_Color color);
    SDL_Color getPixel(SDL_Surface *surface, int x, int y);

protected:
    int getCenteredXPosForBitmap(SDL_Surface *bmp, int totalWidth);
    int getCenteredYPosForBitmap(SDL_Surface *bmp);
    void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_color);
    void draw_line_surface(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 color);
    void draw_rect_outline_surface(SDL_Surface *surface, const SDL_Rect *rect, Uint32 color);
    Uint32 get_pixel(SDL_Surface *surface, int x, int y);

    SDL_Color colorBlack;
    SDL_Color gui_colorWindow;
    SDL_Color gui_colorBorderLight;
    SDL_Color gui_colorBorderDark;

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
    SDL_Renderer *renderer=nullptr;
};
