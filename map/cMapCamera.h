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

class cMapCamera {

public:
    cMapCamera(cMap * theMap);

    ~cMapCamera();

    void think();

    void zoomIn();

    void zoomOut();

    void resetZoom() {
        zoomLevel = 1.0f;
        calibrate();
        keepViewportWithinReasonableBounds();
    }

    void jumpTo(int theX, int theY);

    void moveTo(int theX, int theY);

    /**
     * Returns the pixel coordinate on the window to draw onto. Use this function to convert absolute
     * coordinates into window coordinates so you can draw something.
     * @param absoluteXPosition
     * @return
     */
    int getWindowXPosition(int absoluteXPosition) {
        return getWindowXPositionWithOffset(absoluteXPosition, 0);
    }

    int getWindowYPosition(int absoluteYPosition) {
        return getWindowYPositionWithOffset(absoluteYPosition, 0);
    }

    int getWindowXPositionFromCell(int cell) {
        return getWindowXPositionFromCellWithOffset(cell, 0);
    }

    int getWindowYPositionFromCell(int cell) {
        return getWindowYPositionFromCellWithOffset(cell, 0);
    }

    int getWindowXPositionFromCellWithOffset(int cell, int offset) {
        int absoluteXPosition = pMap->getAbsoluteXPositionFromCell(cell);
        return getWindowXPositionWithOffset(absoluteXPosition, offset);
    }

    int getWindowYPositionFromCellWithOffset(int cell, int offset) {
        int absoluteYPosition = pMap->getAbsoluteYPositionFromCell(cell);
        return getWindowYPositionWithOffset(absoluteYPosition, offset);
    }

    /**
     * Translates an absolute X position into a position on the window. The offset is substracted after translating
     * between absolute x coordinate to window coordinate. Zoom level is applied at last.
     * @param absoluteXPosition
     * @param offset
     * @return
     */
    int getWindowXPositionWithOffset(int absoluteXPosition, int offset) {
        return factorZoomLevel((absoluteXPosition - viewportStartX)+offset);
    }

    /**
     * Translates an absolute Y position into a position on the window. The offset is substracted after translating
     * between absolute x coordinate to window coordinate. Zoom level is applied at last.
     * @param absoluteYPosition
     * @param offset
     * @return
     */
    int getWindowYPositionWithOffset(int absoluteYPosition, int offset) {
        return factorZoomLevel((absoluteYPosition - viewportStartY)+offset) + heightOfTopBar;
    }

    // These methods need to use zoomfactor to properly calculate the position on the map
    int getAbsMapMouseX(int mouseX) {
        int iMouseX = divideByZoomLevel(mouseX);
        return iMouseX + getViewportStartX();
    }

    int getAbsMapMouseY(int mouseY) {
        int iMouseY = divideByZoomLevel(mouseY - heightOfTopBar);
        return iMouseY + getViewportStartY();
    }

    int getViewportStartX() { return viewportStartX; }

    int getViewportStartY() { return viewportStartY; }

    int getViewportEndX() { return viewportStartX + viewportWidth; }

    int getViewportEndY() { return viewportStartY + viewportHeight; }

    int getViewportWidth() { return viewportWidth; }

    int getViewportHeight() { return viewportHeight; }

    int getViewportCenterX() { return viewportStartX + (viewportWidth / 2); }

    int getViewportCenterY() { return viewportStartY + (viewportHeight / 2); }


    void thinkInteraction();

    void centerAndJumpViewPortToCell(int cell);

    void keepViewportWithinReasonableBounds();

    int getZoomedHalfTileSize() const { return halfTile; }

    float getZoomedTileWidth() const { return tileWidth; }

    float getZoomedTileHeight() const { return tileHeight; }

    float factorZoomLevel(int value) {
        return value * zoomLevel;
    }

    float factorZoomLevel(float value) {
        return value * zoomLevel;
    }

    float divideByZoomLevel(int value) {
        return value / zoomLevel;
    }

    float divideByZoomLevel(float value) {
        return value / zoomLevel;
    }

    // Calculates viewport and tile width/height
    void calibrate();

    float getZoomLevel() {
        return zoomLevel;
    }

    int getCellFromAbsolutePosition(int x, int y);

    void setViewportPosition(int x, int y);

protected:

private:
    // rendering coordinates, they are manipulated by zooming
    int viewportStartX;
    int viewportStartY;
    int viewportWidth;
    int viewportHeight;

    int heightOfTopBar;
    int windowWidth;
    int windowHeight;

    // timer used, when to move camera
    int TIMER_move;

    // Zoom level, 1 == normal, > 1 is zooming in. < 1 is zooming out.
    float zoomLevel;

    // the calculated width and height, taking zoomlevel into account
    float tileHeight;
    float tileWidth;
    int halfTile;

    void adjustViewport(float screenX, float screenY);

    // the map this camera is viewing
    cMap * pMap;
};

#endif /* CMAPCAMERA_H_ */
