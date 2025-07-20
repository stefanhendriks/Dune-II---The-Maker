#pragma once

// #include "data/cAllegroDataRepository.h"
#include "utils/cRectangle.h"
#include "utils/Color.hpp"

#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include <stack>

class Texture;

class SDLDrawer {
public:
    SDLDrawer(/*cAllegroDataRepository *dataRepository,*/ SDL_Renderer *_renderer);
    virtual ~SDLDrawer();

    // void drawCenteredSpriteHorizontal(SDL_Surface *dest, SDL_Surface *src, int y, int totalWidth, int xOffset);
    // void drawCenteredSpriteVertical(SDL_Surface *dest, SDL_Surface *src, int x);
    // void drawCenteredSprite(SDL_Surface *dest, SDL_Surface *src);
    // void drawTransSprite(SDL_Surface *sprite, SDL_Surface *dest, int x, int y);

    void renderSprite(Texture *src,int x, int y,Uint8 opacity = 255);
    void renderStrechSprite(Texture *src, cRectangle src_pos, cRectangle dest_pos, Uint8 opacity = 255);

    // [[deprecated]] void drawSprite(SDL_Surface *src, int x, int y,Uint8 opacity = 255);
    // [[deprecated]] void drawSprite(SDL_Surface *dest, SDL_Surface *src, int x, int y,Uint8 opacity = 255);
    // [[deprecated]] void drawSprite(SDL_Surface *dest, int index, int x, int y, Uint8 opacity = 255);
    void renderFromSurface(SDL_Surface *src, int x, int y,Uint8 opacity = 255);

    void renderRectFillColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, unsigned char opacity=255);
    void renderRectFillColor(int x, int y, int width, int height, Color color);
    void renderRectFillColor(const cRectangle &rect, Color color, Uint8 opacity = 255);
    void renderRectColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, unsigned char opacity=255);
    void renderRectColor(int x, int y, int width, int height, Color color);
    void renderRectColor(const cRectangle &rect, Uint8 r, Uint8 g, Uint8 b, unsigned char opacity=255);
    void renderRectColor(const cRectangle &rect, Color color, unsigned char opacity=255);

    void resetClippingFor();
    void setClippingFor(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);

    // void drawSpriteCenteredRelativelyVertical(SDL_Surface *dest, SDL_Surface *src, float percentage);

    // void blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
    // void blitFromGfxData(int index, int src_x, int src_y, int width, int height, int pos_x, int pos_y);
    // void blitSprite(SDL_Surface *src, const cRectangle *rectangle);

    // void stretchSprite(SDL_Surface *src, SDL_Surface *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight, Uint8 opacity=255);

    // void stretchBlit(SDL_Surface *src, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
    // void stretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    // void maskedBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);
    // [[deprecated]] void maskedBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height);

    // void maskedStretchBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);
    // [[deprecated]] void maskedStretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight);

    // void drawRect(SDL_Surface *dest, int x, int y, int width, int height, Color color);
    // void drawRect(SDL_Surface *dest, const cRectangle &pRectangle, Color color);
    // void drawRectFilled(SDL_Surface *dest, int x, int y, int width, int height, Color color);
    // void drawRectFilled(SDL_Surface *dest, const cRectangle &pRectangle, Color color);
    // void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, Color color, int alpha);
    // void drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, Color color);
    void setPixel(SDL_Surface *bmp, int x, int y, Color color);
    // Color getColor_BLACK() {
    //     return Color::black();
    // }
    void renderClearToColor(Color color = Color{0,0,0,255});
    // void bitmap_replace_color(SDL_Surface *bmp, int colorToReplace, int newColor);
    // void bitmap_replace_color(SDL_Surface *bmp, Color colorToReplace, Color newColor);

    // void setTransBlender(int red, int green, int blue, int alpha);

    // Color getColorByNormValue(int r, int g, int b, float norm);

    void gui_DrawRect(const cRectangle &rectangle);
    void gui_DrawRect(const cRectangle &rectangle, Color gui_colorWindow, Color gui_colorBorderLight, Color gui_colorBorderDark);
    void gui_DrawRectBorder(const cRectangle &rectangle, Color gui_colorBorderLight, Color gui_colorBorderDark);

    void renderLine(int x1, int y1, int x2, int y2, Color color);
    void renderDot(int x, int y, Color color, int size);

    // void shimmer(SDL_Surface *src, int r, int x, int y, float cameraZoom);

    void FillWithColor(SDL_Surface *src, Color color);
    Color getPixel(SDL_Surface *surface, int x, int y);

    SDL_Renderer *getRenderer() {
        return renderer;
    }

    Texture *createRenderTargetTexture(int width, int height);
    void beginDrawingToTexture(Texture* targetTexture);
    void endDrawingToTexture();
private:
    // int getCenteredXPosForBitmap(SDL_Surface *bmp, int totalWidth);
    // int getCenteredYPosForBitmap(SDL_Surface *bmp);
    void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_color);

    Uint32 get_pixel(SDL_Surface *surface, int x, int y);

    Color colorBlack;
    Color gui_colorWindow;
    Color gui_colorBorderLight;
    Color gui_colorBorderDark;


    // cAllegroDataRepository *m_dataRepository;
    // struct sSize {
    //     int width;
    //     int height;

    //     bool operator<(sSize rhs) const {
    //         return width == rhs.width ? height < rhs.height : width < rhs.width;
    //     }
    // };
    // std::map<sSize, SDL_Surface *> bitmapCache;
    SDL_Renderer *renderer=nullptr;
    Uint32 transparentColorKey;
    void renderChangeColor(Color color);
    std::stack<SDL_Texture*> renderTargetStack;
};
