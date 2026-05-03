#pragma once

// #include "gameobjects/map/cMap.h"
// #include "gameobjects/map/cMapCamera.h"

struct SDL_Surface;
class cPlayer;
class GameContext;
class Graphics;
class SDLDrawer;
class cMap;
class cMapCamera;
class MapGeometry;

class cMapDrawer {
public:
    cMapDrawer(GameContext *ctx, cMap *map, cPlayer *player, cMapCamera *camera);
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
    cMap *m_map = nullptr;
    MapGeometry *m_mapGeometry = nullptr;
    cPlayer *m_player = nullptr;
    cMapCamera *m_camera = nullptr;
    GameContext *m_ctx = nullptr;
    SDLDrawer* m_renderDrawer = nullptr;
    Graphics *m_gfxdata = nullptr;

    bool m_drawWithoutShroudTiles;
    bool m_drawGrid;

    int determineWhichShroudTileToDraw(int cll, int playerId) const;

    void drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY);
};
