/*
 * cMapCamera.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMapCamera::cMapCamera() {
    viewportStartX = viewportStartY = 32;
	TIMER_move=0;
	zoomLevel = 1.0f;
	desiredZoomLevel = zoomLevel;

    int widthOfSidebar = 160;
    int heightOfOptions = 42;

    windowWidth=game.screen_x-widthOfSidebar;
    windowHeight=game.screen_y-heightOfOptions;

    viewportWidth=windowWidth;
    viewportHeight=windowHeight;

    calibrate();

	char msg[255];
	sprintf(msg, "Camera initialized. Viewport width is [%d], height [%d]. Position [%d,%d]", viewportWidth, viewportHeight, viewportStartX, viewportStartY);
	logbook(msg);
	cellCalculator = new cCellCalculator(&map);
}

cMapCamera::~cMapCamera() {
	delete cellCalculator;
}

void cMapCamera::calibrate() {
    tileHeight = factorZoomLevel(TILESIZE_WIDTH_PIXELS);
    tileWidth = factorZoomLevel(TILESIZE_HEIGHT_PIXELS);
    halfTile = factorZoomLevel(TILESIZE_WIDTH_PIXELS); // assumes squared tiles

    viewportWidth = divideByZoomLevel(windowWidth);
    viewportHeight = divideByZoomLevel(windowHeight);
}

void cMapCamera::centerAndJumpViewPortToCell(int cell) {
	// fix any boundaries
	if (cell < 0) cell = 0;
	if (cell >= MAX_CELLS) cell = (MAX_CELLS-1);

	int cellX = cellCalculator->getX(cell);
	int cellY = cellCalculator->getY(cell);

	// determine the half of our screen
	int width = mapCamera->getViewportWidth();
	int height = mapCamera->getViewportHeight();

	// Half ...
	int iHalfX = width/2;
	int iHalfY = height/2;

	// determine the new X and Y position
	int newViewPortX = cellX - iHalfX;
	int newViewPortY = cellY - iHalfY;

	// now make sure the bottom right does not reach outside the map borders.
	// first jump to the new coordinates
	jumpTo(newViewPortX, newViewPortY);

    correctCameraIfOutsideBoundaries(newViewPortX, newViewPortY);

    calibrate();
}

void cMapCamera::correctCameraIfOutsideBoundaries(int newViewPortX, int newViewPortY) {
    int diffX = getViewportEndX() - (game.map_width - 32);
    int diffY = getViewportEndY() - (game.map_height - 32);

    // when > 0 then it has overlapped, and should be substracted to the original X
    if (diffX > 0) {
        newViewPortX -= diffX;
    }

    if (diffY > 0) {
        newViewPortY -= diffY;
    }

    if (newViewPortX < 32) {
        newViewPortX = 32;
    }

    if (newViewPortY < 32) {
        newViewPortY = 32;
    }

    // now the final 'jump' to the correct positions
    jumpTo(newViewPortX, newViewPortY);
}

void cMapCamera::think() {
//    // NOT USED!?
//	if (targetX != x || targetY != y) {
//		TIMER_move++;
//		if (TIMER_move > 5) {
//			TIMER_move = 0;
//			// move a bit closer
//		}
//	}
}

void cMapCamera::jumpTo(int theX, int theY) {
	viewportStartX = theX * tileWidth;
	viewportStartY = theY * tileHeight;
    calibrate();
}

void cMapCamera::thinkInteraction() {
    // Mouse is 'dragging' (border select) so do not do anything
    if (mouse_co_x1 > -1 && mouse_co_y1 > -1) {
        return;
    }

	// thinking for map (scrolling that is)
	if (mouse_x <= 1 || key[KEY_LEFT]) {
		if (viewportStartX > TILESIZE_WIDTH_PIXELS) {
			viewportStartX -= TILESIZE_HEIGHT_PIXELS;
			mouse_tile = MOUSE_LEFT;
		}
	}


	if (mouse_y <= 1 || key[KEY_UP]) {
		if (viewportStartY > TILESIZE_HEIGHT_PIXELS) {
			viewportStartY -= TILESIZE_HEIGHT_PIXELS;
			mouse_tile = MOUSE_UP;
		}
	}

	if (mouse_x >= (game.screen_x-2) || key[KEY_RIGHT]) {
		if (getViewportEndX() < ((game.map_width*TILESIZE_WIDTH_PIXELS)-TILESIZE_WIDTH_PIXELS)) {
			viewportStartX += TILESIZE_WIDTH_PIXELS;
			mouse_tile = MOUSE_RIGHT;
		}
	}

	if (mouse_y >= (game.screen_y-2) || key[KEY_DOWN]) {
		if ((getViewportEndY()) < ((game.map_height*TILESIZE_HEIGHT_PIXELS)-TILESIZE_HEIGHT_PIXELS)) {
			viewportStartY += TILESIZE_HEIGHT_PIXELS;
			mouse_tile = MOUSE_DOWN;
		}
	}

//    if (desiredZoomLevel < zoomLevel) {
//        zoomLevel -= 0.025;
//        if (zoomLevel < 0.5) zoomLevel = 0.5;
//        calibrate();
//    }

//    if (desiredZoomLevel > zoomLevel) {
//        zoomLevel += 0.025;
//        if (zoomLevel > 2) zoomLevel = 2;
//        calibrate();
//    }
}

int cMapCamera::getCellFromViewportPosition(int x, int y) {
    return iCellMakeWhichCanReturnMinusOne((x / 32), (y / 32));
}
