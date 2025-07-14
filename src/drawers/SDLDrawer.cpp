#include "SDLDrawer.hpp"
#include "utils/Graphics.hpp"
#include "d2tmc.h"

#include <iostream>
#include <algorithm>
#include <memory>


SDLDrawer::SDLDrawer(/*cAllegroDataRepository *dataRepository,*/ SDL_Renderer *_renderer) //: m_dataRepository(dataRepository)
{
    colorBlack= Color{0,0,0,255};
    gui_colorWindow = Color{176,176,196,255};
    gui_colorBorderDark = Color{84,84,120,255};
    gui_colorBorderLight = Color{252,252,252,255};
    renderer = _renderer;
}

SDLDrawer::~SDLDrawer()
{
    // do not delete data repository, we do not own it!
    // m_dataRepository = nullptr;

    // for (auto &p : bitmapCache) {
    //     SDL_FreeSurface(p.second);
    // }
    // bitmapCache.clear();
}

void SDLDrawer::renderSprite(Texture *src,int x, int y,Uint8 opacity)
{
    if (src == nullptr) return;
    SDL_Rect tmp = {x,y, src->w, src->h };
    SDL_SetTextureBlendMode(src->tex, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (SDL_SetTextureAlphaMod(src->tex, opacity)<0) {
        std::cerr << "no alpha mod "<< SDL_GetError() << std::endl;
    }
    SDL_RenderCopy(renderer, src->tex, NULL, &tmp);
}

// void SDLDrawer::stretchSprite(SDL_Surface *src, SDL_Surface *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight, Uint8 opacity)
// {
// if (src == nullptr) return;
// if (dest == nullptr) return;
//
// no use drawing outside of bounds
// if (pos_x + desiredWidth < 0) return;
// if (pos_y + desiredHeight < 0) return;
// if (pos_x > dest->w) return;
// if (pos_y > dest->h) return;
// no use drawing a desired image with 0 width or height
// if (desiredHeight <= 0) return;
// if (desiredWidth <= 0) return;
// cleaned coords for drawing
// SDL_Rect srcRect = { 0, 0, src->w, src->h };
// SDL_Rect dstRect = {pos_x, pos_y, desiredWidth, desiredHeight};
// Uint32 magicPink = SDL_MapRGB(src->format, 255, 0, 255);
// SDL_SetColorKey(src, SDL_TRUE, magicPink);
// SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
// if (!texture) {
// std::cerr << "error maskedBlit : " << SDL_GetError();
// return;
// }
// SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
// SDL_SetTextureAlphaMod(texture, opacity);
// SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
// SDL_DestroyTexture(texture);
//
//
// int result;
// if (src->format->BitsPerPixel != dest->format->BitsPerPixel) {
//     SDL_Surface* converted = SDL_ConvertSurface(src, dest->format, 0);
//     result = SDL_BlitScaled(converted, &srcRect, dest, &dstRect);
//     SDL_FreeSurface(converted);
// } else {
//     result = SDL_BlitScaled(src, &srcRect, dest, &dstRect);
// }

// if (result < 0) {
//     std::cerr << "SDL_BlitScaledSprite failed: " << SDL_GetError() << std::endl;
// }
// }

// void SDLDrawer::stretchBlit(SDL_Surface *src, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
// {
//     if (src == nullptr) return;
//     //if (dest == nullptr) return;

//     if (src_x + width > src->w) width = src->w-src_x;
//     if (src_y + height > src->h) height = src->h-src_y;

//     // could happen due to 'fix' above, you can't have a original width or height of 0
//     if (width < 0) return;
//     if (height < 0) return;

//     // no use drawing outside of bounds
//     if (pos_x + desiredWidth < 0) return;
//     if (pos_y + desiredHeight < 0) return;
//     // if (pos_x > dest->w) return;
//     // if (pos_y > dest->h) return;

//     // no use drawing a desired image with 0 width or height
//     if (desiredHeight <= 0) return;
//     if (desiredWidth <= 0) return;

//     //stretch_blit(src, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
//     Uint32 magicPink = SDL_MapRGB(src->format, 255, 0, 255);
//     SDL_SetColorKey(src, SDL_TRUE, magicPink);
//     SDL_Rect srcRect = {src_x, src_y, width, height };
//     SDL_Rect dstRect = {pos_x, pos_y, desiredWidth, desiredHeight};

//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
//     if (!texture) {
//         std::cerr << "error maskedBlit : " << SDL_GetError();
//         return;
//     }
//     SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
//     SDL_DestroyTexture(texture);
//     /*
//         int result;
//         if (src->format->BitsPerPixel != dest->format->BitsPerPixel) {
//             SDL_Surface* converted = SDL_ConvertSurface(src, dest->format, 0);
//             result = SDL_BlitScaled(converted, &srcRect, dest, &dstRect);
//             SDL_FreeSurface(converted);
//         } else {
//             result = SDL_BlitScaled(src, &srcRect, dest, &dstRect);
//         }
//         if (result < 0) {
//             std::cerr << "SDL_BlitScaled failed: " << SDL_GetError() << std::endl;
//         }*/
// }

// void SDLDrawer::stretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
// {
//     stretchBlit(gfxdata->getSurface(index), dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
// }

// void SDLDrawer::maskedBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height)
// {
//     if (src == nullptr) return;
//     if (dest == nullptr) return;

//     if (src_x + width > src->w) width = src->w-src_x;
//     if (src_y + height > src->h) height = src->h-src_y;

//     // could happen due to 'fix' above, you can't have a original width or height of 0
//     if (width <= 0) return;
//     if (height <= 0) return;

//     // no use drawing outside of bounds
//     if (pos_x + width < 0) return;
//     if (pos_y + height < 0) return;
//     if (pos_x > dest->w) return;
//     if (pos_y > dest->h) return;

//     Uint32 magicPink = SDL_MapRGB(src->format, 255, 0, 255);
//     SDL_SetColorKey(src, SDL_TRUE, magicPink);
//     SDL_Rect srcRect = { src_x, src_y, width, height }; // Zone à copier de la source
//     SDL_Rect dstRect = { pos_x, pos_y, width, height };     // Destination sur la surface cible

//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
//     if (!texture) {
//         std::cerr << "error maskedBlit : " << SDL_GetError();
//         return;
//     }
//     SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
//     SDL_DestroyTexture(texture);
//     //SDL_BlitSurface(src, &srcRect, dest, &dstRect);
//     //masked_blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
// }

// void SDLDrawer::maskedBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height)
// {
//     std::cout << "maskedBlitFromGfxData deprecated" << std::endl;
//     // maskedBlit(gfxdata->getSurface(index), dest, src_x, src_y, pos_x, pos_y, width, height);
// }

// void SDLDrawer::maskedStretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
// {
//     std::cout << "maskedBlitFromGfxData deprecated" << std::endl;
//     // maskedStretchBlit(gfxdata->getSurface(index), dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
// }

// void SDLDrawer::maskedStretchBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight)
// {
//     if (src == nullptr) return;
//     if (dest == nullptr) return;
//     if (src_x + width > src->w) width = src->w-src_x;
//     if (src_y + height > src->h) height = src->h-src_y;

//     // could happen due to 'fix' above, you can't have a original width or height of 0
//     if (width <= 0) return;
//     if (height <= 0) return;

//     // no use drawing outside of bounds
//     if (pos_x + desiredWidth < 0) return;
//     if (pos_y + desiredHeight < 0) return;
//     if (pos_x > dest->w) return;
//     if (pos_y > dest->h) return;

//     // no use drawing a desired image with 0 width or height
//     if (desiredHeight <= 0) return;
//     if (desiredWidth <= 0) return;

//     Uint32 magicPink = SDL_MapRGB(src->format, 255, 0, 255);
//     SDL_SetColorKey(src, SDL_TRUE, magicPink);
//     SDL_Rect srcRect = {src_x, src_y, width, height };
//     SDL_Rect dstRect = {pos_x, pos_y, desiredWidth, desiredHeight};

//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
//     if (!texture) {
//         std::cerr << "error maskedStretchBlit : " << SDL_GetError();
//         return;
//     }
//     SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
//     SDL_DestroyTexture(texture);
//     /*
//     int result;
//     if (src->format->BitsPerPixel != dest->format->BitsPerPixel) {
//         SDL_Surface* converted = SDL_ConvertSurface(src, dest->format, 0);
//         result = SDL_BlitScaled(converted, &srcRect, dest, &dstRect);
//         SDL_FreeSurface(converted);
//     } else {
//         result = SDL_BlitScaled(src, &srcRect, dest, &dstRect);
//     }
//     if (result < 0) {
//         std::cerr << "SDL_BlitScaled failed: " << SDL_GetError() << std::endl;
//     }
//     */
//     //masked_stretch_blit(src, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
// }

// void SDLDrawer::drawCenteredSprite(SDL_Surface *dest, SDL_Surface *src)
// {
//     assert(src);
//     assert(dest);
//     int xPos = getCenteredXPosForBitmap(src, game.m_screenW);
//     int yPos = getCenteredYPosForBitmap(src);
//     this->drawSprite(dest, src, xPos, yPos);
// }

// void SDLDrawer::drawSpriteCenteredRelativelyVertical(SDL_Surface *dest, SDL_Surface *src, float percentage)
// {
//     int xPos = getCenteredXPosForBitmap(src, game.m_screenW);

//     // we want to know the 'center' first. This is done in the percentage
//     int wantedYPos = ((float)game.m_screenH * percentage);

//     // we need to know the height of the src
//     int height = src->h;
//     int halfOfHeight = height / 2;
//     int yPos = wantedYPos - halfOfHeight;
//     this->drawSprite(dest, src, xPos, yPos);
// }


// void SDLDrawer::drawCenteredSpriteHorizontal(SDL_Surface *dest, SDL_Surface *src, int y, int totalWidth, int xOffset)
// {
//     assert(src);
//     assert(dest);
//     int xPos = getCenteredXPosForBitmap(src, totalWidth) + xOffset;
//     this->drawSprite(dest, src, xPos, y);
// }

// void SDLDrawer::drawCenteredSpriteVertical(SDL_Surface *dest, SDL_Surface *src, int x)
// {
//     assert(src);
//     assert(dest);
//     int yPos = getCenteredXPosForBitmap(src, game.m_screenW);
//     this->drawSprite(dest, src, x, yPos);
// }

// int SDLDrawer::getCenteredXPosForBitmap(SDL_Surface *bmp, int totalWidth)
// {
//     assert(bmp);
//     int width = bmp->w;
//     int halfOfWidth = width / 2;
//     return (totalWidth / 2) - halfOfWidth;
// }

// int SDLDrawer::getCenteredYPosForBitmap(SDL_Surface *bmp)
// {
//     assert(bmp);
//     int height = bmp->h;
//     int halfOfHeight = height / 2;
//     return (game.m_screenH / 2) - halfOfHeight;
// }

// void SDLDrawer::blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) {
//     if (src == nullptr) return;
//     blit(src, dest, src_x, src_y, width, height, pos_x, pos_y);
// }

// void SDLDrawer::blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y)
// {
//     // use :: so we use global scope Allegro blitSprite
//     if (src == nullptr) return;
//     //::blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
//     const SDL_Rect src_pos = {src_x, src_y,width, height};
//     SDL_Rect dest_pos = {pos_x, pos_y,width, height};
//     //SDL_BlitSurface(src, &src_pos, dest, &dest_pos);

//     transparentColorKey = SDL_MapRGB(src->format, 255, 0, 255);
//     SDL_SetColorKey(src, SDL_TRUE, transparentColorKey);
//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
//     if (!texture) {
//         std::cerr << "error drawSprite : " << SDL_GetError();
//         return;
//     }
//     SDL_RenderCopy(renderer, texture, &src_pos, &dest_pos);
//     SDL_DestroyTexture(texture);
// }

// void SDLDrawer::blitFromGfxData(int index, int src_x, int src_y, int width, int height, int pos_x, int pos_y)
// {
//     SDL_Surface *src = gfxdata->getSurface(index);
//     this->blit(src, nullptr, src_x, src_y, width, height, pos_x, pos_y);
// }

// void SDLDrawer::blitSprite(SDL_Surface *src, const cRectangle *rectangle)
// {
//     if (rectangle == nullptr) return;
//     if (src == nullptr) return;
//     blit(src, nullptr, 0, 0, rectangle->getWidth(), rectangle->getHeight(), rectangle->getX(), rectangle->getY());
// }

// void SDLDrawer::drawRect(SDL_Surface *dest, const cRectangle &pRectangle, Color color)
// {
//     drawRect(dest, pRectangle.getX(), pRectangle.getY(), pRectangle.getWidth(), pRectangle.getHeight(), color);
// }

// void SDLDrawer::drawRect(SDL_Surface *dest, int x, int y, int width, int height, Color color)
// {
//     renderChangeColor(color);
//     const SDL_Rect tmp = {x, y, width, height};
//     SDL_RenderDrawRect(renderer, &tmp);
// }

// void SDLDrawer::drawRectFilled(SDL_Surface *dest, const cRectangle &pRectangle, Color color)
// {
//     drawRectFilled(dest, pRectangle.getX(), pRectangle.getY(), pRectangle.getWidth(), pRectangle.getHeight(), color);
// }

// void SDLDrawer::drawRectFilled(SDL_Surface *dest, int x, int y, int width, int height, Color color)
// {
//     const SDL_Rect tmp = {x, y, width, height};
//     //std::cout <<"::" << x << " " << y << " " << width << " " << height << std::endl;
//     renderChangeColor(color);
//     SDL_RenderFillRect(renderer, &tmp);
// }

// void SDLDrawer::drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, Color color, int alpha)
// {
//     assert(alpha >= 0);
//     assert(alpha <= 255);

//     // auto bitmap = bitmapCache[sSize{.width = rect.getWidth(), .height = rect.getHeight()}];
//     // if (bitmap == nullptr) {
//     // bitmap = SDL_CreateRGBSurface(0, rect.getWidth(), rect.getHeight(),32,0,0,0,alpha );
//     // bitmapCache[sSize{.width = rect.getWidth(), .height = rect.getHeight()}] = bitmap;
//     // }
//     // auto bitmap = SDL_CreateRGBSurface(0, rect.getWidth(), rect.getHeight(),32,0,0,0,alpha );
//     drawRectFilled(nullptr, rect.getX(), rect.getX(), rect.getWidth(), rect.getHeight(), color);

//     // // @Mira fix trasnparency set_trans_blender(0, 0, 0, alpha);
//     renderDrawer->drawTransSprite(dest, dest, rect.getX(),rect.getY());
// }

// void SDLDrawer::drawRectTransparentFilled(SDL_Surface *dest, const cRectangle &rect, Color color)
// {
//     drawRectTransparentFilled(dest, rect, color, color.a);
// }

// cRectangle *SDLDrawer::fromBitmap(int x, int y, SDL_Surface *src) {
//     return new cRectangle(x, y, src->w, src->h);
// }

// void SDLDrawer::setTransBlender(int red, int green, int blue, int alpha)
// {
    //Mira NoFix
    // // @Mira fix trasnparency set_trans_blender(std::clamp(red, 0, 255),
    //                   std::clamp(green, 0, 255),
    //                   std::clamp(blue, 0, 255),
    //                   std::clamp(alpha, 0, 255));
// }

// void SDLDrawer::drawSprite(SDL_Surface *dest, SDL_Surface *src, int x, int y, Uint8 opacity)
// {
//     drawSprite(src,x,y, opacity);
// }

// void SDLDrawer::drawSprite(SDL_Surface *src, int x, int y,Uint8 opacity)
// {
//     //_draw_sprite(dest, src, x, y);
//     SDL_Rect tmp = {x,y,src->w, src->h};
//     transparentColorKey = SDL_MapRGB(src->format, 255, 0, 255);
//     SDL_SetColorKey(src, SDL_TRUE, transparentColorKey);
//     SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
//     if (!texture) {
//         std::cerr << "error drawSprite : " << SDL_GetError();
//         return;
//     }
//     SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//     SDL_SetTextureAlphaMod(texture, opacity);
//     SDL_RenderCopy(renderer, texture, NULL, &tmp);
//     SDL_DestroyTexture(texture);
// }

void SDLDrawer::renderFromSurface(SDL_Surface *src, int x, int y,Uint8 opacity)
{
    SDL_Rect tmp = {x,y,src->w, src->h};
    transparentColorKey = SDL_MapRGB(src->format, 255, 0, 255);
    SDL_SetColorKey(src, SDL_TRUE, transparentColorKey);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, src);
    if (!texture) {
        std::cerr << "error drawSprite : " << SDL_GetError();
        return;
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opacity);
    SDL_RenderCopy(renderer, texture, NULL, &tmp);
    SDL_DestroyTexture(texture);
}

// void SDLDrawer::drawSprite(SDL_Surface *dest, int index, int x, int y, Uint8 opacity)
// {
//     SDL_Surface *sBitmap = gfxdata->getSurface(index);
//     if (!sBitmap) return; // failed, bail!
//     drawSprite(dest, sBitmap, x, y, opacity);
// }

void SDLDrawer::renderStrechSprite(Texture *src, cRectangle src_pos, cRectangle dest_pos, Uint8 opacity)
{
    if (SDL_SetTextureBlendMode(src->tex, SDL_BLENDMODE_BLEND) < 0) {
        std::cerr << "Error SDL_SetTextureBlendMode : " << SDL_GetError() << std::endl;
    }
    SDL_SetTextureAlphaMod(src->tex, opacity);
    SDL_Rect srcRect = src_pos.toSDL();
    SDL_Rect destRect = dest_pos.toSDL();
    SDL_RenderCopy(renderer, src->tex, &srcRect, &destRect);
}


void SDLDrawer::renderRectFillColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, opacity);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect carre = {x, y, width, height};
    SDL_RenderFillRect(renderer, &carre);
}

void SDLDrawer::renderRectFillColor(int x, int y, int width, int height, Color color)
{
    renderRectFillColor(x,y,width,height,color.r, color.g, color.b,color.a);
}

void SDLDrawer::renderRectColor(int x, int y, int width, int height, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    SDL_SetRenderDrawColor(renderer, r,g,b, opacity);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect carre = {x, y, width, height};
    SDL_RenderDrawRect(renderer, &carre);
}

void SDLDrawer::renderRectColor(const cRectangle &rect, Uint8 r, Uint8 g, Uint8 b, Uint8 opacity)
{
    renderRectColor(rect.getX(),rect.getY(), rect.getWidth(), rect.getHeight(),r,g,b,opacity);
}

void SDLDrawer::renderRectColor(int x, int y, int width, int height, Color color)
{
    renderRectColor(x,y,width,height,color.r, color.g, color.b,color.a);
}

void SDLDrawer::resetClippingFor()
{
    // if (!bmp) return;
    // setClippingFor(bmp, 0, 0, bmp->w, bmp->h);
    // SDL_SetClipRect(bmp, nullptr);
    SDL_RenderSetClipRect(renderer, nullptr);
}

void SDLDrawer::setClippingFor(int topLeftX, int topLeftY, int bottomRightX, int bottomRightY)
{
    // if (!bmp) return;
    auto tmp = SDL_Rect{topLeftX,topLeftY, bottomRightX-topLeftX, bottomRightY - topLeftY};
    // SDL_SetClipRect(bmp_screen, &tmp);
    SDL_RenderSetClipRect(renderer, &tmp);

}

// Color SDLDrawer::getColorByNormValue(int r, int g, int b, float norm)
// {
//     return Color{(Uint8)(r * norm), (Uint8)(g * norm), (Uint8)(b * norm), 255};
// }

void SDLDrawer::gui_DrawRect(const cRectangle &rectangle)
{
    gui_DrawRect(rectangle, gui_colorWindow, gui_colorBorderLight, gui_colorBorderDark);
}

void SDLDrawer::gui_DrawRect(const cRectangle &rectangle, Color gui_colorWindow, Color gui_colorBorderLight, Color gui_colorBorderDark)
{
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();
    //std::cout << x1 << " " << y1 << " " << width << " " << height << std::endl;
    SDL_Rect tmp = {x1,y1, width, height};
    renderChangeColor(gui_colorWindow);
    SDL_RenderFillRect(renderer, &tmp );
    renderChangeColor(gui_colorBorderLight);
    SDL_RenderDrawRect(renderer, &tmp );

    // lines to darken the right sides
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1+width, y1, x1+width, y1+height);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1, y1+height, x1+width, y1+height);
}

void SDLDrawer::gui_DrawRectBorder(const cRectangle &rectangle, Color gui_colorBorderLight, Color gui_colorBorderDark)
{
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();
    //std::cout <<":" << x1 << " " << y1 << " " << width << " " << height << std::endl;
    SDL_Rect tmp = {x1,y1,width,height};
    renderChangeColor(gui_colorBorderLight);
    SDL_RenderDrawRect(renderer, &tmp);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1+width, y1, x1+width, y1+height);
    renderChangeColor(gui_colorBorderDark);
    SDL_RenderDrawLine(renderer, x1, y1+height, x1+width, y1+height);
}

// void SDLDrawer::drawTransSprite(SDL_Surface *sprite, SDL_Surface *dest, int x, int y)
// {
//     drawSprite(dest, sprite, x, y);
// }

void SDLDrawer::renderClearToColor(Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void SDLDrawer::renderLine(int x1, int y1, int x2, int y2, Color color)
{
    renderChangeColor(color);
    SDL_RenderDrawLine(renderer,x1, y1, x2, y2);
}

void SDLDrawer::renderDot(int x, int y, Color color, int size)
{
    if (size < 1) return;
    renderChangeColor(color);
    if (size == 1) {
        SDL_RenderDrawPoint(renderer,x, y);
        return;
    }

    int endX = x + size;
    int endY = y + size;
    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            SDL_RenderDrawPoint(renderer,sx, sy);
        }
    }
}

// void SDLDrawer::shimmer(SDL_Surface *src, int r, int x, int y, float cameraZoom)
// {
//     int x1, y1;
//     int nx, ny;
//     int gp = 0;     // Get Pixel Result
//     int tc = 0;

//     // go around 360 fDegrees (twice as fast now)
//     float step = std::fmax(1.0f, cameraZoom);

//     for (float dr = 0; dr < r; dr += step) {
//         for (double d = 0; d < 360; d++) {
//             x1 = (x + (cos(d) * (dr)));
//             y1 = (y + (sin(d) * (dr)));

//             if (x1 < 0) x1 = 0;
//             if (y1 < 0) y1 = 0;
//             if (x1 >= game.m_screenW) x1 = game.m_screenW - 1;
//             if (y1 >= game.m_screenH) y1 = game.m_screenH - 1;

//             gp = get_pixel(src, x1, y1); //ALLEGRO // use this inline function to speed up things.
//             // Now choose random spot to 'switch' with.
//             nx = (x1 - 1) + rnd(2);
//             ny = (y1 - 1) + rnd(2);

//             if (nx < 0) nx = 0;
//             if (ny < 0) ny = 0;
//             if (nx >= game.m_screenW) nx = game.m_screenW - 1;
//             if (ny >= game.m_screenH) ny = game.m_screenH - 1;

//             tc = get_pixel(src, nx, ny); //ALLEGRO

//             if (gp > -1 && tc > -1) {
//                 // Now switch colors
//                 set_pixel(src, nx, ny, gp);
//                 set_pixel(src, x1, y1, tc);
//             }
//         }
//     }
// }

// Fonction utilitaire pour dessiner un pixel sur une SDL_Surface
// Assurez-vous que la surface est verrouillée avant d'appeler cette fonction !
void SDLDrawer::set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel_color)
{
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        return; // Hors des limites de la surface
    }

    int bpp = surface->format->BytesPerPixel;
    // Pointeur vers le début de la ligne y
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch;

    // Déplacer le pointeur jusqu'au pixel x
    // On convertit Uint32 en la taille appropriée (1, 2, 3 ou 4 octets)
    switch (bpp) {
        case 1: // 8 bits par pixel
            *((Uint8 *)p + x) = (Uint8)pixel_color;
            break;
        case 2: // 16 bits par pixel
            *((Uint16 *)p + x) = (Uint16)pixel_color;
            break;
        case 3: // 24 bits par pixel
            // C'est un peu plus complexe car 3 octets ne s'alignent pas directement avec les types C
            // Il faut copier octet par octet
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                *((Uint8 *)p + x * bpp + 0) = (pixel_color >> 16) & 0xFF;
                *((Uint8 *)p + x * bpp + 1) = (pixel_color >> 8) & 0xFF;
                *((Uint8 *)p + x * bpp + 2) = pixel_color & 0xFF;
            }
            else {
                *((Uint8 *)p + x * bpp + 0) = pixel_color & 0xFF;
                *((Uint8 *)p + x * bpp + 1) = (pixel_color >> 8) & 0xFF;
                *((Uint8 *)p + x * bpp + 2) = (pixel_color >> 16) & 0xFF;
            }
            break;
        case 4: // 32 bits par pixel
            *((Uint32 *)p + x) = pixel_color;
            break;
    }
}

// Fonction utilitaire pour obtenir la couleur d'un pixel sur une SDL_Surface.
// Retourne la valeur du pixel (Uint32) ou 0 si hors limites/erreur.
// Assurez-vous que la surface est verrouillée AVANT d'appeler cette fonction si nécessaire !
Uint32 SDLDrawer::get_pixel(SDL_Surface *surface, int x, int y)
{
    // Vérifier les limites de la surface
    if (x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
        // Pixel hors des limites, retourner 0 ou une valeur d'erreur définie
        // Dans un vrai programme, vous pourriez logger une erreur.
        return 0;
    }

    int bpp = surface->format->BytesPerPixel; // Bytes Per Pixel

    // Calculer l'adresse du pixel
    // p pointe vers le début de la ligne 'y'
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch;
    // Déplacer p à la position 'x' du pixel en tenant compte du BPP
    p += x * bpp;

    // Lire la valeur du pixel en fonction du nombre d'octets par pixel
    switch (bpp) {
        case 1: // 8 bits par pixel
            return *p;
        case 2: // 16 bits par pixel
            return *(Uint16 *)p;
        case 3: // 24 bits par pixel (spécial car non aligné sur 4 octets)
            // L'ordre des octets dépend de l'endianness du système
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                return (p[0] << 16) | (p[1] << 8) | p[2];
            }
            else {
                return p[0] | (p[1] << 8) | (p[2] << 16);
            }
        case 4: // 32 bits par pixel
            return *(Uint32 *)p;
        default:
            fprintf(stderr, "Format de pixel non supporté pour get_pixel: %d BPP\n", bpp);
            return 0; // Ou une valeur d'erreur
    }
}


void SDLDrawer::FillWithColor(SDL_Surface *src, Color color)
{
    Uint32 mappedColor = SDL_MapRGBA(src->format,
                                     color.r,
                                     color.g,
                                     color.b,
                                     color.a);
    int result = SDL_FillRect(src, NULL, mappedColor);
    if (result < 0) {
        SDL_Log("SDL_FillRect failed to clear surface: %s", SDL_GetError());
    }
}


void SDLDrawer::setPixel(SDL_Surface *bmp, int x, int y, Color color)
{
    Uint32 mappedColor = SDL_MapRGBA(bmp->format,
                                     color.r, color.g, color.b, color.a);

    // Vérrouiller la surface avant de modifier les pixels
    if (SDL_LockSurface(bmp) < 0) {
        fprintf(stderr, "Erreur lors du verrouillage de la surface: %s\n", SDL_GetError());
        return;
    }
    set_pixel(bmp,x,y,mappedColor);
    SDL_UnlockSurface(bmp);
}


Color SDLDrawer::getPixel(SDL_Surface *surface, int x, int y)
{
    if (SDL_LockSurface(surface) < 0) {
        fprintf(stderr, "Erreur lors du verrouillage de la surface: %s\n", SDL_GetError());
        return Color{0,0,0,255};
    }
    auto tmpColor = get_pixel(surface,x,y);
    SDL_UnlockSurface(surface);
    Color extractedColor;
    SDL_GetRGBA(tmpColor, surface->format,
                &extractedColor.r,
                &extractedColor.g,
                &extractedColor.b,
                &extractedColor.a);
    return extractedColor;
}


/**
	Function that will go through all pixels and will replace a certain color with another.
    Ie, this can be used to create the fading animation for Windtraps.
**/
// void SDLDrawer::bitmap_replace_color(SDL_Surface *bmp, Color colorToReplace, Color newColor)
// {
//     Uint32 mappedColorToReplace = SDL_MapRGBA(bmp->format,
//                                   colorToReplace.r, colorToReplace.g, colorToReplace.b, colorToReplace.a);
//     Uint32 mappedNewColor = SDL_MapRGBA(bmp->format,
//                                         newColor.r, newColor.g, newColor.b, newColor.a);
//     if (SDL_LockSurface(bmp) < 0) {
//         fprintf(stderr, "Erreur lors du verrouillage de la surface: %s\n", SDL_GetError());
//         return;
//     }
//     for (int x = 0; x < bmp->w; x++) {
//         for (int y = 0; y < bmp->h; y++) {
//             if (get_pixel(bmp, x, y) == mappedColorToReplace) {
//                 set_pixel(bmp, x, y, mappedNewColor);
//             }
//         }
//     }
//     SDL_UnlockSurface(bmp);
// }


void SDLDrawer::renderChangeColor(Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

// Creates a render texture
Texture* SDLDrawer::createRenderTargetTexture(int width, int height) {
    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                              SDL_PIXELFORMAT_RGBA8888,
                                              SDL_TEXTUREACCESS_TARGET,
                                              width,
                                              height);
    if (!texture) {
        throw std::runtime_error("Error creating render texture: " + std::string(SDL_GetError()));
    }
    return new Texture(texture, width, height, true);
}

// Sets a texture as a render target.
// Takes a pointer to the texture. If nullptr, returns to the main window.
void SDLDrawer::beginDrawingToTexture(Texture* targetTexture)
{
    assert(targetTexture && targetTexture->isRenderTarget == true);
    SDL_Texture* currentTarget = SDL_GetRenderTarget(renderer);
    renderTargetStack.push(currentTarget);

    if (SDL_SetRenderTarget(renderer, targetTexture->tex) < 0) {
        renderTargetStack.pop();
        throw std::runtime_error("Erreur lors du changement de cible de rendu: " + std::string(SDL_GetError()));
    }
}

// Termine le dessin sur une texture et revient à la cible par défaut (la fenêtre)
void SDLDrawer::endDrawingToTexture()
{
    if (renderTargetStack.empty()) {
        throw std::runtime_error("endDrawingToTexture without beginDrawingToTexture !");
    }
    SDL_Texture* previousTarget = renderTargetStack.top();
    renderTargetStack.pop();
    if (SDL_SetRenderTarget(renderer, previousTarget) < 0) {
        throw std::runtime_error("Erreur lors de la restauration de la cible de rendu par défaut: " + std::string(SDL_GetError()));
    }
}