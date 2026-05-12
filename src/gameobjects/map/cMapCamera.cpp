#include "cMapCamera.h"

#include "controls/eKeyAction.h"
#include "data/gfxdata.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "sidebar/cSideBar.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "game/cGameSettings.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "gameobjects/map/cMap.h"
#include <algorithm>
#include <cassert>

namespace {
constexpr auto kMapBoundaryScrollSpeed = 5.0f;
}

cMapCamera::cMapCamera(cMap *theMap, float moveSpeedDrag, float moveSpeedBorderOrKeys, bool cameraEdgeMove) :
    m_moveSpeedDrag(moveSpeedDrag),
    m_moveSpeedBorderOrKeys(moveSpeedBorderOrKeys),
    m_cameraEdgeMove(cameraEdgeMove),
    m_pMap(theMap),
    m_mapGeometry(theMap ? &theMap->getGeometry() : nullptr)
{
    assert(theMap != nullptr);
    assert(m_mapGeometry != nullptr);
    m_viewportStartX = m_viewportStartY = 32;
    m_zoomLevel = 1.0f;

    int widthOfSidebar = cSideBar::SidebarWidth;
    m_heightOfTopBar = cSideBar::TopBarHeight;

    m_windowWidth= game.m_gameSettings->getScreenW() - widthOfSidebar;
    m_windowHeight= game.m_gameSettings->getScreenH() - m_heightOfTopBar;

    m_viewportWidth=m_windowWidth;
    m_viewportHeight=m_windowHeight;

    m_moveX = 0.0f;
    m_moveY = 0.0f;

    m_keyPressedDown = false;
    m_keyPressedUp = false;
    m_keyPressedLeft = false;
    m_keyPressedRight = false;

    calibrate();
}

cMapCamera::~cMapCamera()
{
}

void cMapCamera::trackToAbsPosition(float absX, float absY)
{
    m_viewportStartX = absX - (m_viewportWidth / 2.0f);
    m_viewportStartY = absY - (m_viewportHeight / 2.0f);
    keepViewportWithinReasonableBounds();
}

void cMapCamera::zoomOutToFit(const cRectangle& bounds)
{
    float zoomToFit = std::min(
        static_cast<float>(m_windowWidth) / bounds.getWidth(),
        static_cast<float>(m_windowHeight) / bounds.getHeight()
    );
    if (zoomToFit < m_zoomLevel) {
        setZoomLevel(zoomToFit);
    }
}

void cMapCamera::setZoomLevel(float zoom)
{
    if (zoom < 0.25f) zoom = 0.25f;
    if (zoom > 4.0f) zoom = 4.0f;
    m_zoomLevel = zoom;
    calibrate();
    keepViewportWithinReasonableBounds();
}

void cMapCamera::zoomIn()
{
    if (m_zoomLevel < 4.0) {
        m_zoomLevel += 0.1;
        auto m_mouse = game.getMouse();
        adjustViewport(m_mouse->getX(), m_mouse->getY());
    }
}

void cMapCamera::adjustViewport(float screenX, float screenY)
{
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

void cMapCamera::zoomOut()
{
    if (m_zoomLevel > 0.25) {
        m_zoomLevel -= 0.1;
        auto m_mouse = game.getMouse();
        adjustViewport(m_mouse->getX(), m_mouse->getY());
    }
}

void cMapCamera::calibrate()
{
    m_tileHeight = factorZoomLevel(TILESIZE_WIDTH_PIXELS);
    m_tileWidth = factorZoomLevel(TILESIZE_HEIGHT_PIXELS);

    m_viewportWidth = divideByZoomLevel(m_windowWidth);
    m_viewportHeight = divideByZoomLevel(m_windowHeight);
}

void cMapCamera::keepViewportWithinReasonableBounds()
{
    int halfViewportWidth = m_viewportWidth / 2;
    int halfViewportHeight = m_viewportHeight / 2;

    if (m_viewportStartX < -halfViewportWidth) {
        m_viewportStartX = -halfViewportWidth;
    }

    if (m_viewportStartY < -halfViewportHeight) {
        m_viewportStartY = -halfViewportHeight;
    }

    int maxWidth = (game.m_gameObjectsContext->getMap().getWidth() * TILESIZE_WIDTH_PIXELS) + halfViewportWidth;
    if (getViewportEndX() > maxWidth) {
        m_viewportStartX = maxWidth - m_viewportWidth;
    }

    int maxHeight = (game.m_gameObjectsContext->getMap().getHeight() * TILESIZE_HEIGHT_PIXELS) + halfViewportHeight;
    if ((getViewportEndY()) > maxHeight) {
        m_viewportStartY = maxHeight - m_viewportHeight;
    }
}

void cMapCamera::centerAndJumpViewPortToCell(int cell)
{
    // fix any boundaries
    if (cell < 0) cell = 0;
    if (cell >= game.m_gameObjectsContext->getMap().getMaxCells()) cell = (game.m_gameObjectsContext->getMap().getMaxCells()-1);

    int mapCellX = m_mapGeometry->getAbsoluteXPositionFromCell(cell);
    int mapCellY = m_mapGeometry->getAbsoluteYPositionFromCell(cell);

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

void cMapCamera::thinkFast()
{
    // update viewport coordinates
    m_viewportStartX = m_viewportStartX + m_moveX;
    m_viewportStartY = m_viewportStartY + m_moveY;

    keepViewportWithinReasonableBounds();

}

void cMapCamera::jumpTo(int theX, int theY)
{
    m_viewportStartX = theX;
    m_viewportStartY = theY;
    calibrate();
}

void cMapCamera::setViewportPosition(int x, int y)
{
    jumpTo(x, y);
    keepViewportWithinReasonableBounds();
}

int cMapCamera::getCellFromAbsolutePosition(int x, int y)
{
    return m_mapGeometry->getCellWithMapDimensions((x / 32), (y / 32));
}

int cMapCamera::getCellFromAbsolutePositionClamped(int x, int y)
{
    int cx, cy;
    cPoint::split(cx, cy) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap((x / 32), (y / 32));
    return m_mapGeometry->makeCell(cx, cy);
}

void cMapCamera::onNotifyMouseEvent(const s_MouseEvent &event)
{
    // MOUSE WHEEL scrolling causes zooming in/out
    switch (event.eventType) {
        case eMouseEventType::MOUSE_MOVED_TO:
            onMouseMovedTo(event);
            break;
        case eMouseEventType::MOUSE_SCROLLED_DOWN:
            game.m_mapCamera->zoomOut();
            break;
        case eMouseEventType::MOUSE_SCROLLED_UP:
            game.m_mapCamera->zoomIn();
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

void cMapCamera::onMouseMovedTo(const s_MouseEvent &event)
{
    cMouse *pMouse = game.getMouse();

    // mouse is 'moving by pressing right mouse button', this supersedes behavior with edges
    if (!pMouse->isMapScrolling() && m_cameraEdgeMove) {

        int mouseX = event.coords.x;
        int mouseY = event.coords.y;

        if (mouseX <= 2) {
            setMoveX(-kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_LEFT);
        }
        else if (mouseX >= (game.m_gameSettings->getScreenW() - 2)) {
            setMoveX(kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_RIGHT);
        }
        else {
            if (!m_keyPressedLeft && !m_keyPressedRight) {
                setMoveX(0.0f, m_moveSpeedBorderOrKeys);
            }
        }

        if (mouseY <= 2) {
            setMoveY(-kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_UP);
        }
        else if (mouseY >= (game.m_gameSettings->getScreenH() - 2)) {
            setMoveY(kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_DOWN);
        }
        else {
            if (!m_keyPressedUp && !m_keyPressedDown) {
                setMoveY(0.0f, m_moveSpeedBorderOrKeys);
            }
        }
    }
}

void cMapCamera::onNotifyKeyboardEvent(const cKeyboardEvent &event)
{
    switch (event.getType()) {
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

void cMapCamera::onMouseRightButtonPressed(const s_MouseEvent &)
{
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
        diffX = divideByZoomLevel(std::clamp(diffX, -24, 24));
        diffY = divideByZoomLevel(std::clamp(diffY, -24, 24));

        float resultX = diffX * factorX;
        float resultY = diffY * factorY;

        setMoveX(resultX, m_moveSpeedDrag);
        setMoveY(resultY, m_moveSpeedDrag);

        // reset keyboard state, as this supersedes keyboard movement
        m_keyPressedDown = false;
        m_keyPressedUp = false;
        m_keyPressedLeft = false;
        m_keyPressedRight = false;

        return;
    }
}

void cMapCamera::setMoveX(float x, float moveSpeed)
{
    m_moveX = x * moveSpeed;
}

void cMapCamera::setMoveY(float y, float moveSpeed)
{
    m_moveY = y * moveSpeed;
}

void cMapCamera::onKeyHold(const cKeyboardEvent &event)
{
    cMouse *pMouse = game.getMouse();

    // mouse is 'moving by pressing right mouse button', this supersedes reacting to keypress
    if (!pMouse->isMapScrolling()) {
        if (event.isAction(eKeyAction::SCROLL_LEFT)) {
            setMoveX(-kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_LEFT);
            m_keyPressedLeft = true;
        }

        if (event.isAction(eKeyAction::SCROLL_UP)) {
            setMoveY(-kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_UP);
            m_keyPressedUp = true;
        }

        if (event.isAction(eKeyAction::SCROLL_RIGHT)) {
            setMoveX(kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_RIGHT);
            m_keyPressedRight = true;
        }

        if (event.isAction(eKeyAction::SCROLL_DOWN)) {
            setMoveY(kMapBoundaryScrollSpeed, m_moveSpeedBorderOrKeys);
            pMouse->setTile(MOUSE_DOWN);
            m_keyPressedDown = true;
        }
    }
}

void cMapCamera::onKeyPressed(const cKeyboardEvent &event)
{
    if (event.isAction(eKeyAction::SCROLL_LEFT)) {
        setMoveX(0.0f, m_moveSpeedBorderOrKeys);
        m_keyPressedLeft = false;
    }

    if (event.isAction(eKeyAction::SCROLL_UP)) {
        setMoveY(0.0f, m_moveSpeedBorderOrKeys);
        m_keyPressedUp = false;
    }

    if (event.isAction(eKeyAction::SCROLL_RIGHT)) {
        setMoveX(0.0f, m_moveSpeedBorderOrKeys);
        m_keyPressedRight = false;
    }

    if (event.isAction(eKeyAction::SCROLL_DOWN)) {
        setMoveY(0.0f, m_moveSpeedBorderOrKeys);
        m_keyPressedDown = false;
    }
}

void cMapCamera::onMouseRightButtonClicked(const s_MouseEvent &)
{
    m_moveX = 0.0f;
    m_moveY = 0.0f;
}

int cMapCamera::getWindowXPositionFromCellWithOffset(int cell, int offset) {
    int absoluteXPosition = m_mapGeometry->getAbsoluteXPositionFromCell(cell);
    return getWindowXPositionWithOffset(absoluteXPosition, offset);
}

int cMapCamera::getWindowYPositionFromCellWithOffset(int cell, int offset) {
    int absoluteYPosition = m_mapGeometry->getAbsoluteYPositionFromCell(cell);
    return getWindowYPositionWithOffset(absoluteYPosition, offset);
}