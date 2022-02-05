/*
 * cMapCamera.h
 *
 *  A map camera is 'looking' at the map. Using this camera (or viewport) we can determine
 *  what to draw, how big, etc.
 *
 *  Using a camera technique we can easily adjust the drawing logic whenever the resolution
 *  of the game changes.
 *
 */

#pragma once

#include "cMap.h"

#include "observers/cInputObserver.h"

#include <cmath>

class cMapCamera : cInputObserver {

public:
    cMapCamera(cMap * theMap, float moveSpeedDrag, float moveSpeedKeys, bool cameraEdgeMove);

    ~cMapCamera();

    void onNotifyMouseEvent(const s_MouseEvent &event) override;
    void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

    void thinkFast();

    void resetZoom() {
        m_zoomLevel = 1.0f;
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
        int absoluteXPosition = m_pMap->getAbsoluteXPositionFromCell(cell);
        return getWindowXPositionWithOffset(absoluteXPosition, offset);
    }

    int getWindowYPositionFromCellWithOffset(int cell, int offset) {
        int absoluteYPosition = m_pMap->getAbsoluteYPositionFromCell(cell);
        return getWindowYPositionWithOffset(absoluteYPosition, offset);
    }

    /**
     * Translates an absolute X position into a position on the window. The offset is added after translating
     * between absolute x coordinate to window coordinate. Zoom level is applied at last.
     * @param absoluteXPosition
     * @param offset
     * @return
     */
    int getWindowXPositionWithOffset(int absoluteXPosition, int offset) {
        return std::round(factorZoomLevel((absoluteXPosition - m_viewportStartX) + offset));
    }

    /**
     * Translates an absolute Y position into a position on the window. The offset is added after translating
     * between absolute x coordinate to window coordinate. Zoom level is applied at last.
     * @param absoluteYPosition
     * @param offset
     * @return
     */
    int getWindowYPositionWithOffset(int absoluteYPosition, int offset) {
        return std::round(factorZoomLevel((absoluteYPosition - m_viewportStartY) + offset) + m_heightOfTopBar);
    }

    // These methods need to use zoomfactor to properly calculate the position on the map
    int getAbsMapMouseX(int mouseX) {
        int iMouseX = divideByZoomLevel(mouseX);
        return iMouseX + getViewportStartX();
    }

    int getAbsMapMouseY(int mouseY) {
        int iMouseY = divideByZoomLevel(mouseY - m_heightOfTopBar);
        return iMouseY + getViewportStartY();
    }

    int getViewportStartX() { return m_viewportStartX; }

    int getViewportStartY() { return m_viewportStartY; }

    int getViewportEndX() { return m_viewportStartX + m_viewportWidth; }

    int getViewportEndY() { return m_viewportStartY + m_viewportHeight; }

    int getViewportWidth() { return m_viewportWidth; }

    int getViewportHeight() { return m_viewportHeight; }

    int getViewportCenterX() { return m_viewportStartX + (m_viewportWidth / 2); }

    int getViewportCenterY() { return m_viewportStartY + (m_viewportHeight / 2); }

    void centerAndJumpViewPortToCell(int cell);

    float getZoomedTileWidth() const { return m_tileWidth; }

    float getZoomedTileHeight() const { return m_tileHeight; }

    float factorZoomLevel(int value) {
        return value * m_zoomLevel;
    }

    float factorZoomLevel(float value) {
        return value * m_zoomLevel;
    }

    float divideByZoomLevel(int value) {
        return value / m_zoomLevel;
    }

    float divideByZoomLevel(float value) {
        return value / m_zoomLevel;
    }

    int getWindowWidth() {
        return m_windowWidth;
    }

    int getWindowHeight() {
        return m_windowHeight;
    }

    float getZoomLevel() {
        return m_zoomLevel;
    }

    int getCellFromAbsolutePosition(int x, int y);

    void setViewportPosition(int x, int y);

private:
    // rendering coordinates, they are manipulated by zooming
    float m_viewportStartX;
    float m_viewportStartY;
    int m_viewportWidth;
    int m_viewportHeight;

    // used for moving
    float m_moveX;
    float m_moveY;
    float m_moveSpeedDrag;
    float m_moveSpeedBorderOrKeys;
    bool m_cameraEdgeMove;

    bool m_keyPressedUp;
    bool m_keyPressedDown;
    bool m_keyPressedLeft;
    bool m_keyPressedRight;

    int m_heightOfTopBar;
    int m_windowWidth;
    int m_windowHeight;

    // Zoom level, 1 == normal, > 1 is zooming in. < 1 is zooming out.
    float m_zoomLevel;

    // the calculated width and height, taking zoomlevel into account
    float m_tileHeight;
    float m_tileWidth;

    // the map this camera is viewing
    cMap * m_pMap;

    void adjustViewport(float screenX, float screenY);

    // Calculates viewport and tile width/height
    void calibrate();

    void keepViewportWithinReasonableBounds();

    void zoomIn();
    void zoomOut();

    void onMouseMovedTo(const s_MouseEvent &event);

    void onMouseRightButtonClicked(const s_MouseEvent &event);
    void onMouseRightButtonPressed(const s_MouseEvent &event);

    void setMoveX(float x, float moveSpeed);
    void setMoveY(float y, float moveSpeed);

    void onKeyHold(const cKeyboardEvent &event);
    void onKeyPressed(const cKeyboardEvent &event);
};
