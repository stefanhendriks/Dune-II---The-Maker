#include "../include/d2tmh.h"
#include "cAllegroDrawer.h"


cAllegroDrawer::cAllegroDrawer(cAllegroDataRepository * dataRepository) : m_dataRepository(dataRepository) {
    colorBlack=makecol(0,0,0);
}

cAllegroDrawer::~cAllegroDrawer() {
    // do not delete data repository, we do not own it!
    m_dataRepository = nullptr;
}

void cAllegroDrawer::stretchSprite(BITMAP *src, BITMAP *dest, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
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

void cAllegroDrawer::stretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
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

void cAllegroDrawer::stretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    stretchBlit((BITMAP *)gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::maskedBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height) {
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

void cAllegroDrawer::maskedBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int pos_x, int pos_y, int width, int height) {
    maskedBlit((BITMAP *)gfxdata[index].dat, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void cAllegroDrawer::maskedStretchBlitFromGfxData(int index, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
    maskedStretchBlit((BITMAP *)gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y, desiredWidth, desiredHeight);
}

void cAllegroDrawer::maskedStretchBlit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y, int desiredWidth, int desiredHeight) {
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

void cAllegroDrawer::drawCenteredSprite(BITMAP *dest, BITMAP *src) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src, game.screen_x);
	int yPos = getCenteredYPosForBitmap(src);
	draw_sprite(dest, src, xPos, yPos);
}

void cAllegroDrawer::drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage) {
	int xPos = getCenteredXPosForBitmap(src, game.screen_x);

	// we want to know the 'center' first. This is done in the percentage
	int wantedYPos = ((float)game.screen_y * percentage);

	// we need to know the height of the src
	int height = src->h;
	int halfOfHeight = height / 2;
	int yPos = wantedYPos - halfOfHeight;
	draw_sprite(dest, src, xPos, yPos);
}


void cAllegroDrawer::drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y, int totalWidth, int xOffset) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src, totalWidth) + xOffset;
	draw_sprite(dest, src, xPos, y);
}

void cAllegroDrawer::drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x) {
	assert(src);
	assert(dest);
	int yPos = getCenteredXPosForBitmap(src, game.screen_x);
	draw_sprite(dest, src, x, yPos);
}

int cAllegroDrawer::getCenteredXPosForBitmap(BITMAP *bmp, int totalWidth) {
	assert(bmp);
	int width = bmp->w;
	int halfOfWidth = width / 2;
	return (totalWidth / 2) - halfOfWidth;
}

int cAllegroDrawer::getCenteredYPosForBitmap(BITMAP *bmp) {
	assert(bmp);
	int height = bmp->h;
	int halfOfHeight = height / 2;
	return (game.screen_y / 2) - halfOfHeight;
}

void cAllegroDrawer::blit(BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) {
    // use :: so we use global scope Allegro blitSprite
    ::blit(src, dest, src_x, src_y, pos_x, pos_y, width, height);
}

void cAllegroDrawer::blitSprite(BITMAP *src, BITMAP *dest, cRectangle *rectangle) {
    if (rectangle == nullptr) return;
    if (src == nullptr) return;
    if (dest == nullptr) return;
    blit(src, dest, 0, 0, rectangle->getWidth(), rectangle->getHeight(), rectangle->getX(), rectangle->getY());
}

void cAllegroDrawer::drawRectangle(BITMAP *dest, cRectangle *pRectangle, int color) {
    if (pRectangle == nullptr) return;
    rect(dest, pRectangle->getX(), pRectangle->getY(), pRectangle->getEndX(), pRectangle->getEndY(), color);
}

void cAllegroDrawer::drawRectangle(BITMAP *dest, int x, int y, int width, int height, int color) {
    if (dest == nullptr) return;
    // the -1 is because the width /height is "including" the current pixel
    // ie, a width of 1 pixel means it draws just 1 pixel, since the x2 is a dest it should result into x1
    rect(dest, x, y, x + (width-1), y + (height-1), color);
}

void cAllegroDrawer::drawRectangleFilled(BITMAP *dest, cRectangle *pRectangle, int color) {
    if (pRectangle == nullptr) return;
    rectfill(dest, pRectangle->getX(), pRectangle->getY(), pRectangle->getEndX(), pRectangle->getEndY(), color);
}

cRectangle *cAllegroDrawer::fromBitmap(int x, int y, BITMAP *src) {
    return new cRectangle(x, y, src->w, src->h);
}

void cAllegroDrawer::setTransBlender(int red, int green, int blue, int alpha) {
    set_trans_blender(BETWEEN(red, 0, 255),
                      BETWEEN(green, 0, 255),
                      BETWEEN(blue, 0, 255),
                      BETWEEN(alpha, 0, 255));
}

void cAllegroDrawer::drawSprite(BITMAP *dest, BITMAP *src, int x, int y) {
    draw_sprite(dest, src, x, y);
}


/**
	Function that will go through all pixels and will replace a certain color with another.
    Ie, this can be used to create the fading animation for Windtraps.
**/
void cAllegroDrawer::bitmap_replace_color(BITMAP *bmp, int colorToReplace, int newColor) {
    for (int x = 0; x < bmp->w; x++) {
        for (int y = 0; y < bmp->h; y++) {
            if (getpixel(bmp, x, y) == colorToReplace) {
                putpixel(bmp, x, y, newColor);
            }
        }
    }
}

void cAllegroDrawer::drawSprite(BITMAP *dest, int index, int x, int y) {
    sBitmap * sBitmap = m_dataRepository->getBitmapAt(index);
    if (!sBitmap) return; // failed, bail!
    drawSprite(dest, sBitmap->bitmap, x, y);
}

void cAllegroDrawer::resetClippingFor(BITMAP *bmp) {
    if (!bmp) return;
    setClippingFor(bmp, 0, 0, bmp->w, bmp->h);
}

void cAllegroDrawer::setClippingFor(BITMAP *bmp, int topLeftX, int topLeftY, int bottomRightX, int bottomRightY) {
    if (!bmp) return;
    set_clip_rect(bmp, topLeftX, topLeftY, bottomRightX, bottomRightY);
}

void cAllegroDrawer::blitFromGfxData(int index, BITMAP *src, BITMAP *dest, int src_x, int src_y, int width, int height, int pos_x, int pos_y) {
    blit((BITMAP *) gfxdata[index].dat, dest, src_x, src_y, width, height, pos_x, pos_y);
}
