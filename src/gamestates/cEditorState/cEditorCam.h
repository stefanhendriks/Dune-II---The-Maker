#pragma once

#include "utils/cMatrix.h"
#include "utils/cRectangle.h"

#include <cstddef>

class cEditorCam {
public:
    enum class ZoomDirection : char {
        zoomIn,
        zoomOut
    };

    explicit cEditorCam(const cRectangle &mapArea);

    void reset();

    void zoomAtMapPosition(int screenX, int screenY, ZoomDirection direction, const Matrix<int> &mapData);
    void scrollByTiles(int deltaTilesX, int deltaTilesY, const Matrix<int> &mapData);
    void updateVisibleTiles(const Matrix<int> &mapData);

    int screenToTileX(int screenX) const;
    int screenToTileY(int screenY) const;

    int getCameraX() const;
    int getCameraY() const;
    int getTileSize() const;
    int getStartX() const;
    int getStartY() const;
    size_t getEndX() const;
    size_t getEndY() const;

private:
    void clampCameraXToMapBounds(const Matrix<int> &mapData);
    void clampCameraYToMapBounds(const Matrix<int> &mapData);

    cRectangle m_mapArea;

    int m_tileLenSize = 16;
    int m_cameraX = 0;
    int m_cameraY = 0;

    int m_startX = 0;
    int m_startY = 0;
    size_t m_endX = 0;
    size_t m_endY = 0;
    int m_tilesAcross = 0;
    int m_tilesDown = 0;

    static constexpr int MIN_TILE_SIZE = 4;
    static constexpr int MAX_TILE_SIZE = 64;
    static constexpr int DELTA_TILE_SIZE = 4;
};
