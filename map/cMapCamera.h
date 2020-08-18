/*
 * cMapCamera.h
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 *
 *  A map camera is 'looking' at the map. Using this camera (or viewport) we can determine
 *  what to draw, how big, etc.
 *
 *  Using a camera technique we can easily adjust the drawing logic whenever the resolution
 *  of the game changes.
 *
 *  The player can move the camera around, the thinkInteraction() method handles the keyboard
 *  and mouse logic (for now: TODO: abstract this behavior in own classes?)
 *
 */

#ifndef CMAPCAMERA_H_
#define CMAPCAMERA_H_

#define TILESIZE_WIDTH_PIXELS 32
#define TILESIZE_HEIGHT_PIXELS 32

class cMapCamera {
	public:
		cMapCamera();
		~cMapCamera();

		void think();

		void zoomIn() { if (desiredZoomLevel < 2.0) desiredZoomLevel += 0.1; }
		void zoomOut() { if (desiredZoomLevel > 0.25) desiredZoomLevel -= 0.1; }
		void resetZoom() { desiredZoomLevel = 1.0f; }

		void jumpTo(int	theX, int theY);
		void moveTo(int theX, int theY);

		int getX() { return x; }
		int getY() { return y; }
		int getAbsX() { return absX; }
		int getAbsY() { return absY; }

		int getTargetX() { return targetX; }
		int getTargetY() { return targetY; }

		int getEndX() { return x + viewportWidth; }
		int getEndY() { return y + viewportHeight; }

		int getAbsEndX() { return absX + absViewportWidth; }
		int getAbsEndY() { return absY + absViewportHeight; }

		int getViewportWidth() { return viewportWidth; }
		int getViewportHeight() { return viewportHeight; }

		int getAbsViewportWidth() { return absViewportWidth; }
		int getAbsViewportHeight() { return absViewportHeight; }

		void thinkInteraction();

		void centerAndJumpViewPortToCell(int cell);

		int getHalfTileSize() const { return halfTile; }
		int getTileWidth() const { return tileWidth; }
		int getTileHeight() const { return tileHeight; }

		int factorZoomLevel(int value) {
		    return value * zoomLevel;
		}

		// Calculates viewport and tile width/height
		void calibrate();

	protected:


	private:
		// the width and height of the viewport
		// calculated at the constructor. The viewport width and height is
		// in cells (not in pixels).
		int viewportWidth;
		int viewportHeight;

		// viewport dimensions in pixels
		int absViewportWidth;
		int absViewportHeight;

		// the X,Y position (top left corner of the viewport)
		// the targetX and targetY are leading.
		// absX and absY are the absolute pixel coordinates of the map camera
		// TODO: get rid of the cell based positions and use pixel based positions instead
		int x, targetX, absX;
		int y, targetY, absY;

		// timer used, when to move camera
		int TIMER_move;

		// Zoom level, 1 == normal, > 1 is zooming in. < 1 is zooming out. Increases/decreases over time according
		// to desiredZoomLevel
		float zoomLevel;
		float desiredZoomLevel;

		cCellCalculator * cellCalculator;

		// the calculated width and height, taking zoomlevel into account
		int tileHeight;
		int tileWidth;
		int halfTile;

        void correctCameraIfOutsideBoundaries(int newViewPortX, int newViewPortY);
};

#endif /* CMAPCAMERA_H_ */
