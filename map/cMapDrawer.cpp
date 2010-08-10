/*
 * cMapDrawer.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cMapDrawer::cMapDrawer(cMap * theMap, cPlayer * thePlayer) {
	assert(theMap);
	assert(thePlayer);
	map = theMap;
	player = thePlayer;

	viewportWidth=((game.screen_x-160)/TILESIZE_WIDTH_PIXELS);
	viewportHeight=((game.screen_y-42)/TILESIZE_HEIGHT_PIXELS)+1;
}

cMapDrawer::~cMapDrawer() {
	map = NULL;
}

// draw function, called by the drawManager
void cMapDrawer::draw() {

}
