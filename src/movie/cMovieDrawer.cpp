/*
 * cMovieDrawer.cpp
 *
 *  Created on: 20-feb-2011
 *      Author: Stefan
 */

#include "allegroh.h"

#include "cMoviePlayer.h"
#include "cMovieDrawer.h"

cMovieDrawer::cMovieDrawer(cMoviePlayer * playerToDraw, BITMAP * bitmap) {
	player = playerToDraw;
	destinationBitmap = bitmap;
}

cMovieDrawer::~cMovieDrawer() {
	player = NULL;
}

void cMovieDrawer::drawIfPlaying(int x, int y) {
	if (player) {
		BITMAP * bitmapToDraw = player->getFrameToDraw();
		if (bitmapToDraw) {
			draw_sprite(destinationBitmap, bitmapToDraw, x, y);
		}
	}
}
