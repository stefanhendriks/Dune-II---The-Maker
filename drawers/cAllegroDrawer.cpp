/*
 * cAllegroDrawer.cpp
 *
 *  Created on: 5 dec. 2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cAllegroDrawer::cAllegroDrawer() {
}

cAllegroDrawer::~cAllegroDrawer() {
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
	int xPos = getCenteredXPosForBitmap(src);
	int yPos = getCenteredXPosForBitmap(src);
	draw_sprite(dest, src, xPos, yPos);
}

void cAllegroDrawer::drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage) {
	int xPos = getCenteredXPosForBitmap(src);

	// we want to know the 'center' first. This is done in the percentage
	int wantedYPos = ((float)game.screen_y * percentage);

	// we need to know the height of the src
	int height = src->h;
	int halfOfHeight = height / 2;
	int yPos = wantedYPos - halfOfHeight;
	draw_sprite(dest, src, xPos, yPos);
}


void cAllegroDrawer::drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src);
	draw_sprite(dest, src, xPos, y);
}

void cAllegroDrawer::drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x) {
	assert(src);
	assert(dest);
	int yPos = getCenteredXPosForBitmap(src);
	draw_sprite(dest, src, x, yPos);
}

int cAllegroDrawer::getCenteredXPosForBitmap(BITMAP *bmp) {
	assert(bmp);
	int width = bmp->w;
	int halfOfWidth = width / 2;
	return (game.screen_x / 2) - halfOfWidth;
}

int cAllegroDrawer::getCenteredYPosForBitmap(BITMAP *bmp) {
	assert(bmp);
	int height = bmp->h;
	int halfOfHeight = height / 2;
	return (game.screen_y / 2) - halfOfHeight;
}
