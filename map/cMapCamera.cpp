#include "../include/d2tmh.h"
#include "cMapCamera.h"


#include <allegro/mouse.h>

namespace {
    constexpr auto kMapBoundaryScrollSpeed = 5.0f;
}

cMapCamera::cMapCamera(cMap * theMap, float moveSpeed) : m_moveSpeed(moveSpeed), m_pMap(theMap) {
    m_viewportStartX = m_viewportStartY = 32;
    m_zoomLevel = 1.0f;

    int widthOfSidebar = cSideBar::SidebarWidth;
    m_heightOfTopBar = cSideBar::TopBarHeight;

    m_windowWidth= game.m_screenX - widthOfSidebar;
    m_windowHeight= game.m_screenY - m_heightOfTopBar;

    m_viewportWidth=m_windowWidth;
    m_viewportHeight=m_windowHeight;

    m_moveX = 0.0f;
    m_moveY = 0.0f;

    calibrate();
}

cMapCamera::~cMapCamera() {
}

void cMapCamera::zoomIn() {
    if (m_zoomLevel < 4.0) {
        m_zoomLevel += 0.1;
        adjustViewport(mouse_x, mouse_y);
    }
}

void cMapCamera::adjustViewport(float screenX, float screenY) {
    int oldViewportWidth = m_viewportWidth;
    int oldViewportHeight = m_viewportHeight;

    calibrate();

    int diffViewportWidth = oldViewportWidth - m_viewportWidth;
    int diffViewportHeight = oldViewportHeight - m_viewportHeight;

    float nMouseX = screenX / m_windowWidth;
    float nMouseY = (screenY - m_heightOfTopBar) / m_windowHeight;

    m_viewportStartX += diffViewportWidth * nMouseX;
    m_viewportStartY += diffViewportHeight * nMouseY;

    keepViewportWithinReasonableBounds();
}

void cMapCamera::zoomOut()  {
    if (m_zoomLevel > 0.25) {
        m_zoomLevel -= 0.1;
        adjustViewport(mouse_x, mouse_y);
    }
}

void cMapCamera::calibrate() {
    m_tileHeight = factorZoomLevel(TILESIZE_WIDTH_PIXELS);
    m_tileWidth = factorZoomLevel(TILESIZE_HEIGHT_PIXELS);

    m_viewportWidth = divideByZoomLevel(m_windowWidth);
    m_viewportHeight = divideByZoomLevel(m_windowHeight);
}

void cMapCamera::keepViewportWithinReasonableBounds() {
    int halfViewportWidth = m_viewportWidth / 2;
    int halfViewportHeight = m_viewportHeight / 2;

    if (m_viewportStartX < -halfViewportWidth) {
        m_viewportStartX = -halfViewportWidth;
    }

    if (m_viewportStartY < -halfViewportHeight) {
        m_viewportStartY = -halfViewportHeight;
    }

    int maxWidth = (map.getWidth() * TILESIZE_WIDTH_PIXELS) + halfViewportWidth;
    if (getViewportEndX() > maxWidth) {
        m_viewportStartX = maxWidth - m_viewportWidth;
    }

    int maxHeight = (map.getHeight() * TILESIZE_HEIGHT_PIXELS) + halfViewportHeight;
    if ((getViewportEndY()) > maxHeight) {
        m_viewportStartY = maxHeight - m_viewportHeight;
    }
}

void cMapCamera::centerAndJumpViewPortToCell(int cell) {
	// fix any boundaries
	if (cell < 0) cell = 0;
	if (cell >= map.getMaxCells()) cell = (map.getMaxCells()-1);

	int mapCellX = m_pMap->getAbsoluteXPositionFromCell(cell);
	int mapCellY = m_pMap->getAbsoluteYPositionFromCell(cell);

	// determine the half of our screen
	int halfViewportWidth = m_viewportWidth / 2;
	int halfViewportHeight = m_viewportHeight / 2;

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

void cMapCamera::thinkFast() {
    // update viewport coordinates
    m_viewportStartX = m_viewportStartX + m_moveX;
    m_viewportStartY = m_viewportStartY + m_moveY;

    keepViewportWithinReasonableBounds();

}

void cMapCamera::jumpTo(int theX, int theY) {
    m_viewportStartX = theX;
    m_viewportStartY = theY;
    calibrate();
}

void cMapCamera::setViewportPosition(int x, int y) {
    jumpTo(x, y);
    keepViewportWithinReasonableBounds();
}

int cMapCamera::getCellFromAbsolutePosition(int x, int y) {
    return map.getCellWithMapDimensions((x / 32), (y / 32));
}

void cMapCamera::onNotifyMouseEvent(const s_MouseEvent &event) {
    // MOUSE WHEEL scrolling causes zooming in/out
    switch (event.eventType) {
        case eMouseEventType::MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        case eMouseEventType::MOUSE_SCROLLED_DOWN:
            mapCamera->zoomOut();
            break;
        case eMouseEventType::MOUSE_SCROLLED_UP:
            mapCamera->zoomIn();
            break;
        case eMouseEventType::MOUSE_RIGHT_BUTTON_PRESSED:
            onMouseRightButtonPressed(event);
            break;
        case eMouseEventType::MOUSE_RIGHT_BUTTON_CLICKED:
            onMouseRightButtonClicked(event);
            break;
        default:
            break;
    }
}

void cMapCamera::onMouseMovedTo(const s_MouseEvent &event) {
    cMouse *pMouse = game.getMouse();

    int mouseX = event.coords.x;
    int mouseY = event.coords.y;

    if (mouseX <= 1) {
        setMoveX(-kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_LEFT);
    }

    if (mouseY <= 1) {
        setMoveY(-kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_UP);
    }

    if (mouseX >= (game.m_screenX - 2)) {
        setMoveX(kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_RIGHT);
    }

    if (mouseY >= (game.m_screenY - 2)) {
        setMoveY(kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_DOWN);
    }
}

void cMapCamera::onNotifyKeyboardEvent(const cKeyboardEvent &event) {
    switch (event.eventType) {
        case eKeyEventType::HOLD:
            onKeyHold(event);
            break;
        case eKeyEventType::PRESSED:
            onKeyPressed(event);
            break;
        default:
            break;
    }
}

void cMapCamera::onMouseRightButtonPressed(const s_MouseEvent &) {
    m_moveX = 0.0f;
    m_moveY = 0.0f;

    cMouse *pMouse = game.getMouse();

    // mouse is 'moving by pressing right mouse button', this supersedes behavior with borders
    if (pMouse->isMapScrolling()) {
        // difference in pixels (- means up/left, + means down/right)
        int diffX = pMouse->getMouseDragDeltaX();
        int diffY = pMouse->getMouseDragDeltaY();

        // now calculate the speed in which we want to do this, the further
        // away (greater distance) the faster.
        float factorX = (float)abs(diffX) / (m_windowWidth / 2);
        float factorY = (float)abs(diffY) / (m_windowHeight / 2);

        // now we know factor, we only want to move so fast in pixels, so clamp/keep the diffX/Y between sane
        // values. If we don't do this, the scrolling is too fast. Also take m_zoomLevel into account so when we
        // zoom in, it won't go faster. And if we have zoomed out, it will be faster, but relatively the same speed as
        // zoom factor 1.0
        diffX = divideByZoomLevel(keepBetween(diffX, -24, 24));
        diffY = divideByZoomLevel(keepBetween(diffY, -24, 24));

        float resultX = diffX * factorX;
        float resultY = diffY * factorY;

        setMoveX(resultX);
        setMoveY(resultY);
        return;
    }
}

void cMapCamera::setMoveX(float x) {
    m_moveX = x * m_moveSpeed;
}

void cMapCamera::setMoveY(float y) {
    m_moveY = y * m_moveSpeed;
}

void cMapCamera::onKeyHold(const cKeyboardEvent &event) {
    cMouse *pMouse = game.getMouse();

    if (event.hasKey(KEY_LEFT)) {
        setMoveX(-kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_LEFT);
    }

    if (event.hasKey(KEY_UP)) {
        setMoveY(-kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_UP);
    }

    if (event.hasKey(KEY_RIGHT)) {
        setMoveX(kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_RIGHT);
    }

    if (event.hasKey(KEY_DOWN)) {
        setMoveY(kMapBoundaryScrollSpeed);
        pMouse->setTile(MOUSE_DOWN);
    }
}

void cMapCamera::onKeyPressed(const cKeyboardEvent &event) {
    if (event.hasKey(KEY_LEFT)) {
        setMoveX(0.0f);
    }

    if (event.hasKey(KEY_UP)) {
        setMoveY(0.0f);
    }

    if (event.hasKey(KEY_RIGHT)) {
        setMoveX(0.0f);
    }

    if (event.hasKey(KEY_DOWN)) {
        setMoveY(0.0f);
    }
}

void cMapCamera::onMouseRightButtonClicked(const s_MouseEvent &) {
    m_moveX = 0.0f;
    m_moveY = 0.0f;
}
