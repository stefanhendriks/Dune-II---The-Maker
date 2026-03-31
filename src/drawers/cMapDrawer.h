#pragma once

// #include "map/cMap.h"
// #include "map/cMapCamera.h"

struct SDL_Surface;
class cPlayer;
class GameContext;
class Graphics;
class SDLDrawer;
class cMap;
class cMapCamera;

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
    cMap *m_map;
    cPlayer *m_player;
    cMapCamera *m_camera;
    GameContext *m_ctx;
    SDLDrawer* m_renderDrawer;
    Graphics *m_gfxdata;

    bool m_drawWithoutShroudTiles;
    bool m_drawGrid;

    int determineWhichShroudTileToDraw(int cll, int playerId) const;

    void drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY);
};
