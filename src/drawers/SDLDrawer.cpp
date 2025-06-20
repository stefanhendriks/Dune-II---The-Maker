#include "SDLDrawer.hpp"

#include "d2tmc.h"

#include <algorithm>
#include <memory>


SDLDrawer::SDLDrawer(cAllegroDataRepository * dataRepository) : m_dataRepository(dataRepository) {
    colorBlack= SDL_Color{0,0,0,255};
    gui_colorWindow = SDL_Color{176,176,196,255};
    gui_colorBorderDark = SDL_Color{84,84,120,255};
    gui_colorBorderLight = SDL_Color{252,252,252,255};
}

SDLDrawer::~SDLDrawer() {
    // do not delete data repository, we do not own it!
    m_dataRepository = nullptr;

    for (auto& p : bitmapCache) {
      SDL_FreeSurface(p.second);
    }
    bitmapCache.clear();
}

void SDLDrawer::stretchSprite(SDL_Surface *src, SDL_Surface *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    if (src == nullptr) return;
    if (dest == nullptr) return;
    // only same color depth is supported
    if (bitmap_color_depth(src) != bitmap_color_depth(dest)) return;

    // no use drawing outside of bounds
    if (pos_x + desiredWidth < 0) return;
    if (pos_y + desiredHeight < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    // no use drawing a desired image with 0 width or height
    if (desiredHeight <= 0) return;
    if (desiredWidth <= 0) return;

    stretch_sprite(dest, src, pos_x, pos_y, desiredWidth, desiredHeight);
}

void SDLDrawer::stretchBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    if (src == nullptr) return;
    if (dest == nullptr) return;
    // only same color depth is supported
    if (bitmap_color_depth(src) != bitmap_color_depth(dest)) return;

    if (src_x + width > src->w) width = src->w-src_x;
    if (src_y + height > src->h) height = src->h-src_y;

    // could happen due to 'fix' above, you can't have a original width or height of 0
    if (width < 0) return;
    if (height < 0) return;

    // no use drawing outside of bounds
    if (pos_x + desiredWidth < 0) return;
    if (pos_y + desiredHeight < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    // no use drawing a desired image with 0 width or height
    if (desiredHeight <= 0) return;
    if (desiredWidth <= 0) return;

    stretch_blit(src, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void SDLDrawer::stretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    stretchBlit((SDL_Surface *)gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void SDLDrawer::maskedBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height) {
    if (src == nullptr) return;
    if (dest == nullptr) return;

    if (src_x + width > src->w) width = src->w-src_x;
    if (src_y + height > src->h) height = src->h-src_y;

    // could happen due to 'fix' above, you can't have a original width or height of 0
    if (width <= 0) return;
    if (height <= 0) return;

    // no use drawing outside of bounds
    if (pos_x + width < 0) return;
    if (pos_y + height < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    masked_blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void SDLDrawer::maskedBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height) {
    maskedBlit((SDL_Surface *)gfxdata[index].dat, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void SDLDrawer::maskedStretchBlitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    maskedStretchBlit((SDL_Surface *)gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void SDLDrawer::maskedStretchBlit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    if (src == nullptr) return;
    if (dest == nullptr) return;
    // only same color depth is supported
    if (bitmap_color_depth(src) != bitmap_color_depth(dest)) return;

    if (src_x + width > src->w) width = src->w-src_x;
    if (src_y + height > src->h) height = src->h-src_y;

    // could happen due to 'fix' above, you can't have a original width or height of 0
    if (width <= 0) return;
    if (height <= 0) return;

    // no use drawing outside of bounds
    if (pos_x + desiredWidth < 0) return;
    if (pos_y + desiredHeight < 0) return;
    if (pos_x > dest->w) return;
    if (pos_y > dest->h) return;

    // no use drawing a desired image with 0 width or height
    if (desiredHeight <= 0) return;
    if (desiredWidth <= 0) return;

    masked_stretch_blit(src, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void SDLDrawer::drawCenteredSprite(SDL_Surface *dest, SDL_Surface *src) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src, game.m_screenW);
	int yPos = getCenteredYPosForBitmap(src);
	this->drawSprite(dest, src, xPos, yPos);
}

void SDLDrawer::drawSpriteCenteredRelativelyVertical(SDL_Surface *dest, SDL_Surface* src, float percentage) {
	int xPos = getCenteredXPosForBitmap(src, game.m_screenW);

	// we want to know the 'center' first. This is done in the percentage
	int wantedYPos = ((float)game.m_screenH * percentage);

	// we need to know the height of the src
	int height = src->h;
	int halfOfHeight = height / 2;
	int yPos = wantedYPos - halfOfHeight;
	this->drawSprite(dest, src, xPos, yPos);
}


void SDLDrawer::drawCenteredSpriteHorizontal(SDL_Surface *dest, SDL_Surface *src, int y, int totalWidth, int xOffset) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src, totalWidth) + xOffset;
	this->drawSprite(dest, src, xPos, y);
}

void SDLDrawer::drawCenteredSpriteVertical(SDL_Surface *dest, SDL_Surface *src, int x) {
	assert(src);
	assert(dest);
	int yPos = getCenteredXPosForBitmap(src, game.m_screenW);
	this->drawSprite(dest, src, x, yPos);
}

int SDLDrawer::getCenteredXPosForBitmap(SDL_Surface *bmp, int totalWidth) {
	assert(bmp);
	int width = bmp->w;
	int halfOfWidth = width / 2;
	return (totalWidth / 2) - halfOfWidth;
}

int SDLDrawer::getCenteredYPosForBitmap(SDL_Surface *bmp) {
	assert(bmp);
	int height = bmp->h;
	int halfOfHeight = height / 2;
	return (game.m_screenH / 2) - halfOfHeight;
}

// void SDLDrawer::blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) {
//     if (src == nullptr) return;
//     blit(src, dest, src_x, src_y, width, height, pos_x, pos_y);
// }

void SDLDrawer::blit(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) const {
    // use :: so we use global scope Allegro blitSprite
    if (src == nullptr || dest == nullptr) return;
    ::blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void SDLDrawer::blitSprite(SDL_Surface *src, SDL_Surface *dest, const cRectangle *rectangle) const {
    if (rectangle == nullptr) return;
    if (src == nullptr) return;
    if (dest == nullptr) return;
    blit(src, dest, 0, 0, rectangle->getWidth(), rectangle->getHeight(), rectangle->getX(), rectangle->getY());
}

void SDLDrawer::drawRect(SDL_Surface *dest, const cRectangle &pRectangle, SDL_Color color) {
    rect(dest, pRectangle.getX(), pRectangle.getY(), pRectangle.getEndX(), pRectangle.getEndY(), color);
}

void SDLDrawer::drawRect(SDL_Surface *dest, int x, int y, int width, int height, SDL_Color color) {
    if (dest == nullptr) return;
    // the -1 is because the width /height is "including" the current pixel
    // ie, a width of 1 pixel means it draws just 1 pixel, since the x2 is a dest it should result into x1
    rect(dest, x, y, x + (width-1), y + (height-1), color);
}

void SDLDrawer::drawRectFilled(SDL_Surface *dest, const cRectangle &pRectangle, SDL_Color color) {
    rectfill(dest, pRectangle.getX(), pRectangle.getY(), pRectangle.getEndX(), pRectangle.getEndY(), color);
}

void SDLDrawer::drawRectFilled(SDL_Surface *dest, int x, int y, int width, int height, SDL_Color color) {
    const SDL_Rect tmp = {0, 0, width, height};
    Uint32 mappedColor = SDL_MapRGBA(dest->format, color.r, color.g, color.b, color.a);
    SDL_FillRect(dest, &tmp, mappedColor);
}

void SDLDrawer::drawRectTransparentFilled(SDL_Surface *dest, const cRectangle& rect, SDL_Color color, int alpha) {
    assert(alpha >= 0);
    assert(alpha <= 255);

    auto bitmap = bitmapCache[sSize{.width = rect.getWidth(), .height = rect.getHeight()}];
    if (bitmap == nullptr) {
        bitmap = SDL_CreateRGBSurface(0, rect.getWidth(), rect.getHeight(),32,0,0,0,alpha );
        bitmapCache[sSize{.width = rect.getWidth(), .height = rect.getHeight()}] = bitmap;
    }
    drawRectFilled(bitmap, 0, 0, rect.getWidth(), rect.getHeight(), color);

    // set_trans_blender(0, 0, 0, alpha);
    // draw_trans_sprite(dest, bitmap, rect.getX(),rect.getY());
}

// cRectangle *SDLDrawer::fromBitmap(int x, int y, SDL_Surface *src) {
//     return new cRectangle(x, y, src->w, src->h);
// }

void SDLDrawer::setTransBlender(int red, int green, int blue, int alpha) {
    set_trans_blender(std::clamp(red, 0, 255),
                      std::clamp(green, 0, 255),
                      std::clamp(blue, 0, 255),
                      std::clamp(alpha, 0, 255));
}

void SDLDrawer::drawSprite(SDL_Surface *dest, SDL_Surface *src, int x, int y) {
    // draw_sprite(dest, src, x, y);
    SDL_Rect tmp = {x,y,src->w, src->h};
    SDL_BlitSurface(src, nullptr, dest, &tmp);
}


/**
	Function that will go through all pixels and will replace a certain color with another.
    Ie, this can be used to create the fading animation for Windtraps.
**/
void SDLDrawer::bitmap_replace_color(SDL_Surface *bmp, int colorToReplace, int newColor) {
    for (int x = 0; x < bmp->w; x++) {
        for (int y = 0; y < bmp->h; y++) {
            if (getpixel(bmp, x, y) == colorToReplace) {
                putpixel(bmp, x, y, newColor);
            }
        }
    }
}

void SDLDrawer::drawSprite(SDL_Surface *dest, int index, int x, int y) {
    SDL_Surface * sBitmap = m_dataRepository->getBitmapAt(index);
    if (!sBitmap) return; // failed, bail!
    drawSprite(dest, sBitmap, x, y);
}

void SDLDrawer::resetClippingFor(SDL_Surface *bmp) {
    if (!bmp) return;
    setClippingFor(bmp, 0, 0, bmp->w, bmp->h);
}

void SDLDrawer::setClippingFor(SDL_Surface *bmp, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY) {
    if (!bmp) return;
    set_clip_rect(bmp, topLeftX, topLeftY, bottomRightX, bottomRightY);
}

void SDLDrawer::blitFromGfxData(int index, SDL_Surface *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) {
    SDL_Surface *src = (SDL_Surface *) gfxdata[index].dat;
    this->blit(src, dest, src_x, src_y, width, height, pos_x, pos_y);
}

SDL_Color SDLDrawer::getColorByNormValue(int r, int g, int b, float norm) {
    return SDL_Color{(Uint8)(r * norm), (Uint8)(g * norm), (Uint8)(b * norm), 255};
}

void SDLDrawer::gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle) {
    gui_DrawRect(dest, rectangle, gui_colorWindow, gui_colorBorderLight, gui_colorBorderDark);
}

void SDLDrawer::gui_DrawRect(SDL_Surface *dest, const cRectangle &rectangle, SDL_Color gui_colorWindow, SDL_Color gui_colorBorderLight, SDL_Color gui_colorBorderDark) {
    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();

    drawRectFilled(dest, rectangle, gui_colorWindow);
    drawRect(dest, rectangle, gui_colorBorderLight);

    // lines to darken the right sides
    line(bmp_screen, x1+width, y1, x1+width , y1+height, gui_colorBorderDark);
    line(bmp_screen, x1, y1+height, x1+width , y1+height, gui_colorBorderDark);
}

void SDLDrawer::gui_DrawRectBorder(SDL_Surface *dest, const cRectangle &rectangle, int gui_colorBorderLight, SDL_Color gui_colorBorderDark) {

    int x1= rectangle.getX();
    int y1 = rectangle.getY();
    int width = rectangle.getWidth();
    int height = rectangle.getHeight();

    drawRect(dest, rectangle, gui_colorBorderLight);
    line(bmp_screen, x1+width, y1, x1+width , y1+height, gui_colorBorderDark);
    line(bmp_screen, x1, y1+height, x1+width , y1+height, gui_colorBorderDark);
}

void SDLDrawer::drawTransSprite(SDL_Surface *sprite, SDL_Surface *dest, int x, int y) {
    draw_trans_sprite(dest, sprite, x, y);
}

void SDLDrawer::drawLine(SDL_Surface *bmp, int x1, int y1, int x2, int y2, SDL_Color color) {
    line(bmp, x1, y1, x2, y2, color);
}

void SDLDrawer::drawDot(SDL_Surface *bmp, int x, int y, SDL_Color color, int size) {
    if (size < 1) return;

    if (size == 1) {
        putpixel(bmp, x, y, color);
        return;
    }

    int endX = x + size;
    int endY = y + size;
    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            putpixel(bmp, sx, sy, color);
        }
    }
}
