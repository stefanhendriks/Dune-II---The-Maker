/*
 * cMapCamera.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMapCamera::cMapCamera() {
	x=y=1;
    absX = absY = 32;
	targetX=targetY=1;
	TIMER_move=0;
	zoomLevel = 1.0f;
	desiredZoomLevel = zoomLevel;

    int widthOfSidebar = 160;
    int heightOfOptions = 42;
    absViewportWidth=game.screen_x-widthOfSidebar;
    absViewportHeight=game.screen_y-heightOfOptions;

    calibrate();

	char msg[255];
	sprintf(msg, "Camera initialized. Viewport width is [%d], height [%d]. Position [%d,%d]", viewportWidth, viewportHeight, getX(), getY());
	logbook(msg);
	cellCalculator = new cCellCalculator(&map);
}

cMapCamera::~cMapCamera() {
	delete cellCalculator;
}

void cMapCamera::calibrate() {
    tileHeight = factorZoomLevel(TILESIZE_WIDTH_PIXELS);
    tileWidth = factorZoomLevel(TILESIZE_HEIGHT_PIXELS);
    viewportWidth=(absViewportWidth/tileWidth);
    viewportHeight=(absViewportHeight/tileHeight)+1;

    correctCameraIfOutsideBoundaries(getX(), getY());
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
}

void cMapCamera::correctCameraIfOutsideBoundaries(int newViewPortX, int newViewPortY) {
    int diffX = getEndX() - (game.map_width - 1);
    int diffY = getEndY() - (game.map_height - 1);

    // when > 0 then it has overlapped, and should be substracted to the original X
    if (diffX > 0) {
        newViewPortX -= diffX;
    }

    if (diffY > 0) {
        newViewPortY -= diffY;
    }

    if (newViewPortX < 1) {
        newViewPortX = 1;
    }

    if (newViewPortY < 1) {
        newViewPortY = 1;
    }

    // now the final 'jump' to the correct positions
    jumpTo(newViewPortX, newViewPortY);
}

void cMapCamera::think() {
    // NOT USED!?
	if (targetX != x || targetY != y) {
		TIMER_move++;
		if (TIMER_move > 5) {
			TIMER_move = 0;
			// move a bit closer
		}
	}
}

void cMapCamera::jumpTo(int theX, int theY) {
	x = theX;
	absX = x * tileWidth;
	targetX = theX;
	y = theY;
	absY = y * tileHeight;
	targetY = theY;
}

void cMapCamera::moveTo(int theX, int theY) {
	targetX = theX;
	targetY = theY;
}


void cMapCamera::thinkInteraction() {
    // Mouse is 'dragging' (border select) so do not do anything
    if (mouse_co_x1 > -1 && mouse_co_y1 > -1) {
        return;
    }

	// thinking for map (scrolling that is)
	if (mouse_x <= 1 || key[KEY_LEFT]) {
		if (x > 1) {
			x--;
			absX -= 32;
			mouse_tile = MOUSE_LEFT;
		}
	}


	if (mouse_y <= 1 || key[KEY_UP]) {
		if (y > 1) {
			y--;
			absY -= 32;
			mouse_tile = MOUSE_UP;
		}
	}


	if (mouse_x >= (game.screen_x-2) || key[KEY_RIGHT]) {
		if ((getEndX()) < (game.map_width-1)) {
			x++;
			absX += 32;
			mouse_tile = MOUSE_RIGHT;
		}
	}

	if (mouse_y >= (game.screen_y-2) || key[KEY_DOWN]) {
		if ((getEndY()) < (game.map_height-1)) {
			y++;
            absY += 32;
			mouse_tile = MOUSE_DOWN;
		}
	}

    if (desiredZoomLevel < zoomLevel) {
        zoomLevel -= 0.025;
        if (zoomLevel < 0.5) zoomLevel = 0.5;
        calibrate();
    }

    if (desiredZoomLevel > zoomLevel) {
        zoomLevel += 0.025;
        if (zoomLevel > 2) zoomLevel = 2;
        calibrate();
    }
}
