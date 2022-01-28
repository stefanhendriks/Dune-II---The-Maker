/*
 * cMapCamera.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

#include <allegro/mouse.h>

cMapCamera::cMapCamera(cMap * theMap) : pMap(theMap) {
    viewportStartX = viewportStartY = 32;
	TIMER_move=0;
	zoomLevel = 1.0f;

    int widthOfSidebar = cSideBar::SidebarWidth;
    heightOfTopBar = cSideBar::TopBarHeight;

    windowWidth= game.m_screenX - widthOfSidebar;
    windowHeight= game.m_screenY - heightOfTopBar;

    viewportWidth=windowWidth;
    viewportHeight=windowHeight;

    calibrate();
}

cMapCamera::~cMapCamera() {
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

void cMapCamera::keepViewportWithinReasonableBounds() {
    int halfViewportWidth = viewportWidth / 2;
    int halfViewportHeight = viewportHeight / 2;

    if (viewportStartX < -halfViewportWidth) {
        viewportStartX = -halfViewportWidth;
    }

    if (viewportStartY < -halfViewportHeight) {
        viewportStartY = -halfViewportHeight;
    }

    int maxWidth = (map.getWidth() * TILESIZE_WIDTH_PIXELS) + halfViewportWidth;
    if (getViewportEndX() > maxWidth) {
        viewportStartX = maxWidth-viewportWidth;
    }

    int maxHeight = (map.getHeight() * TILESIZE_HEIGHT_PIXELS) + halfViewportHeight;
    if ((getViewportEndY()) > maxHeight) {
        viewportStartY = maxHeight-viewportHeight;
    }
}

void cMapCamera::centerAndJumpViewPortToCell(int cell) {
	// fix any boundaries
	if (cell < 0) cell = 0;
	if (cell >= map.getMaxCells()) cell = (map.getMaxCells()-1);

	int mapCellX = pMap->getAbsoluteXPositionFromCell(cell);
	int mapCellY = pMap->getAbsoluteYPositionFromCell(cell);

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

// TODO: replace with onNotifyGameEvent (mouse observer)
// replace this with an event based system, where the
// event "mouse moved" is captured, hence we can
// react on those events and then move the camera around
void cMapCamera::thinkInteraction() {
    // Mouse is 'dragging' (border select) so do not do anything
    cMouse *pMouse = game.getMouse();
    if (pMouse->isBoxSelecting()) {
        return;
    }

    // mouse is 'moving by pressing right mouse button', this supersedes behavior with borders
    if (pMouse->isMapScrolling()) {

        // difference in pixels (- means up/left, + means down/right)
        int diffX = pMouse->getMouseDragDeltaX();
        int diffY = pMouse->getMouseDragDeltaY();

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
			pMouse->setTile(MOUSE_LEFT);
		}
	}


	if (mouse_y <= 1 || key[KEY_UP]) {
		if (viewportStartY > -halfViewportHeight) {
            setViewportPosition(viewportStartX, viewportStartY -= 1);
            pMouse->setTile(MOUSE_UP);
		}
	}

	if (mouse_x >= (game.m_screenX - 2) || key[KEY_RIGHT]) {
		if (getViewportEndX() < ((map.getWidth()*TILESIZE_WIDTH_PIXELS)+halfViewportWidth)) {
            setViewportPosition(viewportStartX += 1, viewportStartY);
            pMouse->setTile(MOUSE_RIGHT);
		}
	}

	if (mouse_y >= (game.m_screenY - 2) || key[KEY_DOWN]) {
		if ((getViewportEndY()) < ((map.getHeight()*TILESIZE_HEIGHT_PIXELS)+halfViewportHeight)) {
            setViewportPosition(viewportStartX, viewportStartY += 1);
            pMouse->setTile(MOUSE_DOWN);
		}
	}
}

int cMapCamera::getCellFromAbsolutePosition(int x, int y) {
    return map.getCellWithMapDimensions((x / 32), (y / 32));
}

void cMapCamera::onNotify(const s_MouseEvent &event) {
    // MOUSE WHEEL scrolling causes zooming in/out
    switch (event.eventType) {
        case eMouseEventType::MOUSE_SCROLLED_DOWN:
            mapCamera->zoomOut();
            return;
        case eMouseEventType::MOUSE_SCROLLED_UP:
            mapCamera->zoomIn();
            return;
        default:
            return;
    }

}
