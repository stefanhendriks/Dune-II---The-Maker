#include <assert.h>

#include "../include/allegroh.h"

#include "AllegroDrawer.h"

AllegroDrawer::AllegroDrawer() {
}

AllegroDrawer::AllegroDrawer(ScreenResolution * theScreenResolution) {
	screenResolution = theScreenResolution;
}

AllegroDrawer::~AllegroDrawer() {
	screenResolution = NULL;
}

void AllegroDrawer::drawCenteredSprite(BITMAP *dest, BITMAP *src) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src);
	int yPos = getCenteredXPosForBitmap(src);
	draw_sprite(dest, src, xPos, yPos);
}

void AllegroDrawer::drawSpriteCenteredRelativelyVertical(BITMAP *dest, BITMAP* src, float percentage) {
	int xPos = getCenteredXPosForBitmap(src);

	// we want to know the 'center' first. This is done in the percentage
	int wantedYPos = ((float) screenResolution->getHeight() * percentage);

	// we need to know the height of the src
	int height = src->h;
	int halfOfHeight = height / 2;
	int yPos = wantedYPos - halfOfHeight;
	draw_sprite(dest, src, xPos, yPos);
}

void AllegroDrawer::drawCenteredSpriteHorizontal(BITMAP *dest, BITMAP *src, int y) {
	assert(src);
	assert(dest);
	int xPos = getCenteredXPosForBitmap(src);
	draw_sprite(dest, src, xPos, y);
}

void AllegroDrawer::drawCenteredSpriteVertical(BITMAP *dest, BITMAP *src, int x) {
	assert(src);
	assert(dest);
	int yPos = getCenteredXPosForBitmap(src);
	draw_sprite(dest, src, x, yPos);
}

int AllegroDrawer::getCenteredXPosForBitmap(BITMAP *bmp) {
	assert(bmp);
	int width = bmp->w;
	int halfOfWidth = width / 2;
	return (screenResolution->getWidth() / 2) - halfOfWidth;
}

int AllegroDrawer::getCenteredYPosForBitmap(BITMAP *bmp) {
	assert(bmp);
	int height = bmp->h;
	int halfOfHeight = height / 2;
	return (screenResolution->getHeight() / 2) - halfOfHeight;
}
