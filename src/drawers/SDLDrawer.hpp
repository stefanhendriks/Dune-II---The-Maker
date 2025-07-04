#pragma once

#include "utils/cRectangle.h"
#include "utils/Color.hpp"

#include <map>
#include <memory>
#include <SDL2/SDL.h>
#include <stack>

class Texture;

class SDLDrawer {
public:
    SDLDrawer(SDL_Renderer *_renderer);
    virtual ~SDLDrawer();
    void renderSprite(Texture *src,int x, int y,Uint8 opacity = 255);
    void renderStrechSprite(Texture *src, cRectangle src_pos, cRectangle dest_pos, Uint8 opacity = 255);
    void renderFromSurface(SDL_Surface *src, int x, int y,Uint8 opacity = 255);

    void renderRectFillColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, unsigned char opacyty=255);
    void renderRectFillColor(int x, int y, int width, int height, Color color);
    void renderRectColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, unsigned char opacyty=255);
    void renderRectColor(int x, int y, int width, int height, Color color);
    void renderRectColor(const cRectangle &pRectangle, Uint8 r, Uint8 g, Uint8 b, unsigned char opacyty=255);

    void resetClippingFor();
    void setClippingFor(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY);

    void setPixel(SDL_Surface *bmp, int x, int y, Color color);
    // Color getColor_BLACK() {
    //     return Color::black();
    // }
    void renderClearToColor(Color color = Color{0,0,0,255});

    void gui_DrawRect(const cRectangle &rectangle);
    void gui_DrawRect(const cRectangle &rectangle, Color gui_colorWindow, Color gui_colorBorderLight, Color gui_colorBorderDark);
    void gui_DrawRectBorder(const cRectangle &rectangle, Color gui_colorBorderLight, Color gui_colorBorderDark);

    void renderLine(int x1, int y1, int x2, int y2, Color color);
    void renderDot(int x, int y, Color color, int size);

    void shimmer(SDL_Surface *src, int r, int x, int y, float cameraZoom);

    void FillWithColor(SDL_Surface *src, Color color);
    Color getPixel(SDL_Surface *surface, int x, int y);

    SDL_Renderer *getRenderer() {
        return renderer;
    }

    Texture *createRenderTargetTexture(int width, int height);
    void beginDrawingToTexture(Texture* targetTexture);
    void endDrawingToTexture();
private:
    void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_color);

    Uint32 get_pixel(SDL_Surface *surface, int x, int y);

    Color colorBlack;
    Color gui_colorWindow;
    Color gui_colorBorderLight;
    Color gui_colorBorderDark;

    SDL_Renderer *renderer=nullptr;
    Uint32 transparentColorKey;
    void renderChangeColor(Color color);
    std::stack<SDL_Texture*> renderTargetStack;
};
