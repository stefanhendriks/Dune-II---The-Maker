/*
 * cMapCamera.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cMapCamera::cMapCamera() {
	int widthOfSidebar = 160;
	int heightOfOptions = 42;
	viewportWidth=((game.screen_x-widthOfSidebar)/TILESIZE_WIDTH_PIXELS);
	viewportHeight=((game.screen_y-heightOfOptions)/TILESIZE_HEIGHT_PIXELS)+1;
	x=y=1;
	targetX=targetY=1;
	TIMER_move=0;

	char msg[255];
	sprintf(msg, "Camera initialized. Viewport width is [%d], height [%d]. Position [%d,%d]", viewportWidth, viewportHeight, getX(), getY());
	logbook(msg);
}

cMapCamera::~cMapCamera() {
}

void cMapCamera::centerAndJumpViewPortToCell(int cell) {
	cCellCalculator cellCalculator;

	int cellX = cellCalculator.getX(cell);
	int cellY = cellCalculator.getY(cell);

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

	int diffX = getEndX() - (game.map_width - 2);
	int diffY = getEndY() - (game.map_height - 2);

	// when > 0 then it has overlapped, and should be substracted to the original X
	if (diffX > 0) {
		newViewPortX -= diffX;
	}

	if (diffY > 0) {
		newViewPortY -= diffY;
	}

	// now the final 'jump' to the correct positions
	jumpTo(newViewPortX, newViewPortY);
}

void cMapCamera::think() {
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
	targetX = theX;
	y = theY;
	targetY = theY;
}

void cMapCamera::moveTo(int theX, int theY) {
	targetX = theX;
	targetY = theY;
}


void cMapCamera::thinkInteraction() {
   if (mouse_co_x1 > -1 && mouse_co_y1 > -1) {
		return;
   }

	// thinking for map (scrolling that is)
	if (mouse_x <= 1 || key[KEY_LEFT]) {
		if (x > 1) {
			x--;
			mouse_tile = MOUSE_LEFT;
		}
	}


	if (mouse_y <= 1 || key[KEY_UP]) {
		if (y > 1) {
			y--;
			mouse_tile = MOUSE_UP;

		}
	}


	if (mouse_x >= (game.screen_x-2) || key[KEY_RIGHT]) {
		if ((getEndX()) < (game.map_width-1)) {
			x++;
			mouse_tile = MOUSE_RIGHT;
		}
	}

	if (mouse_y >= (game.screen_y-2) || key[KEY_DOWN]) {
		if ((getEndY()) < (game.map_height-1)) {
			y++;
			mouse_tile = MOUSE_DOWN;
		}
	}
}
