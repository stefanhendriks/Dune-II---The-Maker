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

    int widthOfSidebar = cSideBar::SidebarWidth;
    heightOfTopBar = 42;

    windowWidth=game.screen_x-widthOfSidebar;
    windowHeight= game.screen_y - heightOfTopBar;

    viewportWidth=windowWidth;
    viewportHeight=windowHeight;

    calibrate();

	cellCalculator = new cCellCalculator(&map);
}

cMapCamera::~cMapCamera() {
	delete cellCalculator;
}

void cMapCamera::zoomIn() {
    if (zoomLevel < 4.0) {
        zoomLevel += 0.1;
        adjustViewport(mouse_x, mouse_y);
    }
}

void cMapCamera::adjustViewport(float screenX, float screenY) {
    int oldViewportWidth = viewportWidth;
    int oldViewportHeight = viewportHeight;

    calibrate();

    int diffViewportWidth = oldViewportWidth - viewportWidth;
    int diffViewportHeight = oldViewportHeight - viewportHeight;

    float nMouseX = screenX / windowWidth;
    float nMouseY = (screenY - heightOfTopBar) / windowHeight;

    viewportStartX += diffViewportWidth * nMouseX;
    viewportStartY += diffViewportHeight * nMouseY;

    keepViewportWithinReasonableBounds();
}

void cMapCamera::zoomOut()  {
    if (zoomLevel > 0.25) {
        zoomLevel -= 0.1;
        adjustViewport(mouse_x, mouse_y);
    }
}

void cMapCamera::calibrate() {
    tileHeight = factorZoomLevel(TILESIZE_WIDTH_PIXELS);
    tileWidth = factorZoomLevel(TILESIZE_HEIGHT_PIXELS);
    halfTile = factorZoomLevel(TILESIZE_WIDTH_PIXELS); // assumes squared tiles

    viewportWidth = divideByZoomLevel(windowWidth);
    viewportHeight = divideByZoomLevel(windowHeight);
}

int cMapCamera::getAbsoluteXPositionFromCell(int cell) {
    if (cell < 0) return -1;
    return cellCalculator->getX(cell) * 32;
}

int cMapCamera::getAbsoluteYPositionFromCell(int cell) {
    if (cell < 0) return -1;
    return cellCalculator->getY(cell) * 32;
}

void cMapCamera::keepViewportWithinReasonableBounds() {
    int halfViewportWidth = viewportWidth / 2;
    int halfViewportHeight = viewportHeight / 2;

    if (viewportStartX < -halfViewportWidth) {
        viewportStartX = -halfViewportWidth;
    }

    if (viewportStartY < -halfViewportHeight) {
        viewportStartY = -halfViewportHeight;
    }

    int maxWidth = (game.map_width * TILESIZE_WIDTH_PIXELS) + halfViewportWidth;
    if (getViewportEndX() > maxWidth) {
        viewportStartX = maxWidth-viewportWidth;
    }

    int maxHeight = (game.map_height * TILESIZE_HEIGHT_PIXELS) + halfViewportHeight;
    if ((getViewportEndY()) > maxHeight) {
        viewportStartY = maxHeight-viewportHeight;
    }
}

void cMapCamera::centerAndJumpViewPortToCell(int cell) {
	// fix any boundaries
	if (cell < 0) cell = 0;
	if (cell >= MAX_CELLS) cell = (MAX_CELLS-1);

	int mapCellX = getAbsoluteXPositionFromCell(cell);
	int mapCellY = getAbsoluteYPositionFromCell(cell);

	// determine the half of our screen
	int halfViewportWidth = viewportWidth / 2;
	int halfViewportHeight = viewportHeight / 2;

	// determine the new X and Y position, absolute map coordinates
	int newViewPortX = mapCellX - halfViewportWidth;
	int newViewPortY = mapCellY - halfViewportHeight;

	// for now, snap it to 32x32 grid
	newViewPortX = (newViewPortX / 32) * 32;
	newViewPortY = (newViewPortY / 32) * 32;

	jumpTo(newViewPortX, newViewPortY);

    calibrate();

    keepViewportWithinReasonableBounds();
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
	viewportStartX = theX;
	viewportStartY = theY;
    calibrate();
}

void cMapCamera::setViewportPosition(int x, int y) {
    jumpTo(x, y);
    keepViewportWithinReasonableBounds();
}

void cMapCamera::thinkInteraction() {
    // Mouse is 'dragging' (border select) so do not do anything
    if (mouse_co_x1 > -1 && mouse_co_y1 > -1) {
        return;
    }

    // mouse is 'moving by pressing right mouse button', this supersedes behavior with borders
    if (mouse_mv_x1 > -1 && mouse_mv_y1 > -1 &&
        mouse_mv_x2 > -1 && mouse_mv_y2 > -1) {

        // difference in pixels (- means up/left, + means down/right)
        int diffX = mouse_mv_x2 - mouse_mv_x1;
        int diffY = mouse_mv_y2 - mouse_mv_y1;

        // now calculate the speed in which we want to do this, the further
        // away (greater distance) the faster.
        float factorX = (float)abs(diffX) / (windowWidth/2);
        float factorY = (float)abs(diffY) / (windowHeight/2);

        // now we know factor, we only want to move so fast in pixels, so clamp/keep the diffX/Y between sane
        // values. If we don't do this, the scrolling is too fast. Also take zoomLevel into account so when we
        // zoom in, it won't go faster. And if we have zoomed out, it will be faster, but relatively the same speed as
        // zoom factor 1.0
        diffX = divideByZoomLevel(keepBetween(diffX, -24, 24));
        diffY = divideByZoomLevel(keepBetween(diffY, -24, 24));

        float resultX = diffX * factorX;
        float resultY = diffY * factorY;

        viewportStartX += resultX;
        viewportStartY += resultY;

        keepViewportWithinReasonableBounds();
        return;
    }

    int halfViewportWidth = viewportWidth / 2;
    int halfViewportHeight = viewportHeight / 2;

	// thinking for map (scrolling that is)
	if (mouse_x <= 1 || key[KEY_LEFT]) {
		if (viewportStartX > -halfViewportWidth) {
            setViewportPosition(viewportStartX -= 1, viewportStartY);
			mouse_tile = MOUSE_LEFT;
		}
	}


	if (mouse_y <= 1 || key[KEY_UP]) {
		if (viewportStartY > -halfViewportHeight) {
            setViewportPosition(viewportStartX, viewportStartY -= 1);
			mouse_tile = MOUSE_UP;
		}
	}

	if (mouse_x >= (game.screen_x-2) || key[KEY_RIGHT]) {
		if (getViewportEndX() < ((game.map_width*TILESIZE_WIDTH_PIXELS)+halfViewportWidth)) {
            setViewportPosition(viewportStartX += 1, viewportStartY);
			mouse_tile = MOUSE_RIGHT;
		}
	}

	if (mouse_y >= (game.screen_y-2) || key[KEY_DOWN]) {
		if ((getViewportEndY()) < ((game.map_height*TILESIZE_HEIGHT_PIXELS)+halfViewportHeight)) {
            setViewportPosition(viewportStartX, viewportStartY += 1);
			mouse_tile = MOUSE_DOWN;
		}
	}
}

int cMapCamera::getCellFromAbsolutePosition(int x, int y) {
    return iCellMakeWhichCanReturnMinusOne((x / 32), (y / 32));
}
