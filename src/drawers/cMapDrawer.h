#pragma once

#include "map/cMap.h"
#include "map/cMapCamera.h"

struct BITMAP;
class cPlayer;

class cMapDrawer {
public:
    cMapDrawer(cMap *map, cPlayer *player, cMapCamera *camera);
    ~cMapDrawer();

    void setPlayer(cPlayer *thePlayer);

    void drawTerrain();
    void drawShroud();

    void setDrawWithoutShroudTiles(bool value) {
        m_drawWithoutShroudTiles = value;
    }
    void setDrawGrid(bool value) {
        m_drawGrid = value;
    }

protected:


private:
    cMap *m_map;
    cPlayer *m_player;
    cMapCamera *m_camera;

    // bitmap for drawing tiles, and possibly stretching (depending on zoom level)
    BITMAP *m_BmpTemp;

    bool m_drawWithoutShroudTiles;
    bool m_drawGrid;

    int determineWhichShroudTileToDraw(int cll, int playerId) const;

    void drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY);
};
