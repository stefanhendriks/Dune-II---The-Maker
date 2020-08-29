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

		void zoomIn() {
		    if (zoomLevel < 2.0) {
                zoomLevel += 0.1;
                calibrate();
                keepCameraWithinReasonableBounds();
            }
		}
		void zoomOut() {
		    if (zoomLevel > 0.25) {
		        zoomLevel -= 0.1;
                calibrate();
                keepCameraWithinReasonableBounds();
            }
		}
		void resetZoom() {
		    zoomLevel = 1.0f;
		    calibrate();
            keepCameraWithinReasonableBounds();
		}

		void jumpTo(int	theX, int theY);
		void moveTo(int theX, int theY);

		// These methods need to use zoomfactor to properly calculate the position on the map
		int getAbsMapMouseX(int mouseX) {
            int iMouseX = divideByZoomLevel(mouseX);
            return iMouseX + getViewportStartX();
		}

		int getAbsMapMouseY(int mouseY) {
		    int heightTopBar = 42; // TODO: this should be Y position of window
            int iMouseY = divideByZoomLevel(mouseY - heightTopBar);
            return iMouseY + getViewportStartY();
        }

		int getViewportStartX() { return viewportStartX; }
		int getViewportStartY() { return viewportStartY; }

		int getViewportEndX() { return viewportStartX + viewportWidth; }
		int getViewportEndY() { return viewportStartY + viewportHeight; }

		int getViewportWidth() { return viewportWidth; }
		int getViewportHeight() { return viewportHeight; }

		void thinkInteraction();

		void centerAndJumpViewPortToCell(int cell);
		void keepCameraWithinReasonableBounds();

		int getZoomedHalfTileSize() const { return halfTile; }
		float getZoomedTileWidth() const { return tileWidth; }
		float getZoomedTileHeight() const { return tileHeight; }

		float factorZoomLevel(int value) {
		    return value * zoomLevel;
		}

		float divideByZoomLevel(int value) {
		    return value / zoomLevel;
		}

		// Calculates viewport and tile width/height
		void calibrate();

		float getZoomLevel() {
		    return zoomLevel;
		}

    int getCellFromViewportPosition(int x, int y);

		void setViewportPosition(int x, int y);

protected:

	private:
		// rendering coordinates, they are manipulated by zooming
		int viewportStartX;
		int viewportStartY;
        int viewportWidth;
        int viewportHeight;

		int windowWidth;
		int windowHeight;

		// timer used, when to move camera
		int TIMER_move;

		// Zoom level, 1 == normal, > 1 is zooming in. < 1 is zooming out.
		float zoomLevel;

		cCellCalculator * cellCalculator;

		// the calculated width and height, taking zoomlevel into account
		float tileHeight;
		float tileWidth;
		int halfTile;

        int getMapXPositionFromCell(int cell);

        int getMapYPositionFromCell(int cell);
};

#endif /* CMAPCAMERA_H_ */
