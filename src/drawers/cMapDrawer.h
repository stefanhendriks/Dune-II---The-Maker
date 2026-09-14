/**
 * @file cMapDrawer.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

// #include "gameobjects/map/cMap.h"
// #include "gameobjects/map/cMapCamera.h"

#include <memory>

struct SDL_Surface;
class Texture;
class cPlayer;
class GameContext;
class Graphics;
class SDLDrawer;
class cGameSettings;
class cMap;
class cMapCamera;
class MapGeometry;
struct sGameServices;

class cMapDrawer {
public:
    cMapDrawer(GameContext *ctx, cMap *map, cPlayer *player, cMapCamera *camera);
    ~cMapDrawer();

    void setPlayer(cPlayer *thePlayer);

    void serviceInit(sGameServices*) {}

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
    SDLDrawer* m_sdlDrawer = nullptr;
    Graphics *m_gfxdata = nullptr;
    cGameSettings* m_settings = nullptr;

    bool m_drawWithoutShroudTiles;
    bool m_drawGrid;

    // the shroud sprite, but grey instead of black, used to veil the fog of war
    std::unique_ptr<Texture> m_fogTexture;

    /**
     * Which border tile of the shroud sprite fits this cell? When fogOfWar is true the borders are
     * determined by the cells that are not observed right now, instead of the undiscovered cells.
     */
    int determineWhichShroudTileToDraw(int cll, int playerId, bool fogOfWar) const;

    std::unique_ptr<Texture> createFogTexture() const;

    void drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY);

    /**
     * Draws the grey veil over a discovered-but-not-currently-observed cell, when fog of war is
     * enabled. isSeen() alone isn't enough to decide this: it stops updating the instant fog of war
     * is disabled, so this checks the live setting itself rather than trusting isSeen()'s state.
     */
    void drawFogVeil(int iCell, int iPl, int iDrawX, int iDrawY, int iTileWidth, int iTileHeight);
};
