#include "gamestates/cEditorState/cEditorCam.h"

#include <algorithm>

cEditorCam::cEditorCam(const cRectangle &mapArea)
    : m_mapArea(mapArea)
{
}

void cEditorCam::reset()
{
    m_tileLenSize = 16;
    m_cameraX = 0;
    m_cameraY = 0;
    m_startX = 0;
    m_startY = 0;
    m_endX = 0;
    m_endY = 0;
    m_tilesAcross = 0;
    m_tilesDown = 0;
}

void cEditorCam::zoomAtMapPosition(int screenX, int screenY, ZoomDirection direction, const Matrix<int> &mapData)
{
    int previousTileSize = m_tileLenSize;
    if (direction == ZoomDirection::zoomOut) {
        m_tileLenSize -= DELTA_TILE_SIZE;
        m_tileLenSize = std::max(m_tileLenSize, MIN_TILE_SIZE);
    } else if (direction == ZoomDirection::zoomIn) {
        m_tileLenSize += DELTA_TILE_SIZE;
        m_tileLenSize = std::min(m_tileLenSize, MAX_TILE_SIZE);
    }

    if (m_tileLenSize == previousTileSize) {
        return;
    }

    const int worldTileX = (m_cameraX + screenX) / previousTileSize;
    const int worldTileY = (m_cameraY + screenY) / previousTileSize;
    m_cameraX = worldTileX * m_tileLenSize - screenX;
    m_cameraY = worldTileY * m_tileLenSize - screenY;

    clampCameraXToMapBounds(mapData);
    clampCameraYToMapBounds(mapData);
}

void cEditorCam::scrollByTiles(int deltaTilesX, int deltaTilesY, const Matrix<int> &mapData)
{
    m_cameraX += deltaTilesX * m_tileLenSize;
    m_cameraY += deltaTilesY * m_tileLenSize;
    clampCameraXToMapBounds(mapData);
    clampCameraYToMapBounds(mapData);
}

void cEditorCam::updateVisibleTiles(const Matrix<int> &mapData)
{
    m_startX = m_cameraX / m_tileLenSize;
    m_startY = m_cameraY / m_tileLenSize;

    m_tilesAcross = (m_mapArea.getWidth() / m_tileLenSize) + 1;
    m_tilesDown = (m_mapArea.getHeight() / m_tileLenSize) + 1;

    m_endX = static_cast<size_t>(m_startX + m_tilesAcross);
    m_endY = static_cast<size_t>(m_startY + m_tilesDown);

    const size_t cols = mapData.getCols();
    const size_t rows = mapData.getRows();

    if (m_endX > cols) {
        m_endX = cols;
        m_startX = static_cast<int>(m_endX) - m_tilesAcross;
        if (m_startX < 0) {
            m_startX = 0;
        }
    }
    if (m_endY > rows) {
        m_endY = rows;
        m_startY = static_cast<int>(m_endY) - m_tilesDown;
        if (m_startY < 0) {
            m_startY = 0;
        }
    }
}

int cEditorCam::screenToTileX(int screenX) const
{
    return (m_cameraX + screenX) / m_tileLenSize;
}

int cEditorCam::screenToTileY(int screenY) const
{
    return (m_cameraY + screenY) / m_tileLenSize;
}

int cEditorCam::getCameraX() const
{
    return m_cameraX;
}

int cEditorCam::getCameraY() const
{
    return m_cameraY;
}

int cEditorCam::getTileSize() const
{
    return m_tileLenSize;
}

int cEditorCam::getStartX() const
{
    return m_startX;
}

int cEditorCam::getStartY() const
{
    return m_startY;
}

size_t cEditorCam::getEndX() const
{
    return m_endX;
}

size_t cEditorCam::getEndY() const
{
    return m_endY;
}

void cEditorCam::clampCameraXToMapBounds(const Matrix<int> &mapData)
{
    if (m_cameraX < 0) {
        m_cameraX = 0;
        return;
    }

    int maxCameraX = static_cast<int>(mapData.getCols()) * m_tileLenSize - m_mapArea.getWidth();
    if (maxCameraX < 0) {
        maxCameraX = 0;
    }
    if (m_cameraX > maxCameraX) {
        m_cameraX = maxCameraX;
    }
}

void cEditorCam::clampCameraYToMapBounds(const Matrix<int> &mapData)
{
    if (m_cameraY < 0) {
        m_cameraY = 0;
        return;
    }

    int maxCameraY = static_cast<int>(mapData.getRows()) * m_tileLenSize - m_mapArea.getHeight();
    if (maxCameraY < 0) {
        maxCameraY = 0;
    }
    if (m_cameraY > maxCameraY) {
        m_cameraY = maxCameraY;
    }
}
