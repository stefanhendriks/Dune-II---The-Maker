#include "cMapDrawer.h"

#include "controls/cGameControlsContext.h"
#include "data/gfxdata.h"
#include "d2tmc.h"
#include "drawers/SDLDrawer.hpp"
#include "drawers/cTextDrawer.h"
#include "player/cPlayer.h"
#include "utils/Graphics.hpp"

#include <SDL2/SDL.h>

#include <cmath>

cMapDrawer::cMapDrawer(cMap *map, cPlayer *player, cMapCamera *camera) :
    m_map(map),
    m_player(player),
    m_camera(camera),
    m_drawWithoutShroudTiles(false),
    m_drawGrid(false)
{
    assert(map);
    assert(camera);
}

cMapDrawer::~cMapDrawer()
{
    m_map = nullptr;
    m_camera = nullptr;
    m_player = nullptr;
}

void cMapDrawer::drawShroud()
{
    float tileWidth = mapCamera->getZoomedTileWidth();
    float tileHeight = mapCamera->getZoomedTileHeight();

    int iTileHeight = (tileHeight + 1);
    int iTileWidth = (tileWidth + 1);
    int iPl = m_player->getId();

    for (int viewportX = m_camera->getViewportStartX(); viewportX < m_camera->getViewportEndX() + 32; viewportX += 32) {

        // new row
        for (int viewportY = m_camera->getViewportStartY(); viewportY < m_camera->getViewportEndY() + 32; viewportY += 32) {
            int iCell = mapCamera->getCellFromAbsolutePosition(viewportX, viewportY);

            if (iCell < 0) continue;

            int absoluteXCoordinateOnMap = m_map->getAbsoluteXPositionFromCell(iCell);
            float fDrawX = mapCamera->getWindowXPosition(absoluteXCoordinateOnMap);

            int absoluteYCoordinateOnMap = m_map->getAbsoluteYPositionFromCell(iCell);
            float fDrawY = mapCamera->getWindowYPosition(absoluteYCoordinateOnMap);
            int iDrawX = round(fDrawX);
            int iDrawY = round(fDrawY);
            if (m_drawWithoutShroudTiles) {
                if (m_map->isVisible(iCell, iPl)) {
                    // do nothing
                }
                else {
                    renderDrawer->renderRectFillColor(fDrawX, fDrawY, tileWidth, tileHeight,0, 0, 0,128);
                }
            }
            else {
                if (m_map->isVisible(iCell, iPl)) {
                    int tile = determineWhichShroudTileToDraw(iCell, iPl);

                    if (tile > -1) {
                        const cRectangle src_pos = {tile * 32, 0, 32, 32};
                        cRectangle dest_pos = {iDrawX, iDrawY, iTileWidth, iTileHeight};
                        renderDrawer->renderStrechSprite(gfxdata->getTexture(SHROUD), src_pos, dest_pos);
                    }
                }
                else {
                    // NOT VISIBLE, DO NOT DRAW A THING THEN!
                    // Except when there is a building here, that should not be visible ;)
                    // tile 0 of shroud is entirely black... (effectively the same as drawing a rect here)
                    const cRectangle src_pos = {0, 0, 32, 32};
                    cRectangle dest_pos = {iDrawX, iDrawY, iTileWidth, iTileHeight};
                    renderDrawer->renderStrechSprite(gfxdata->getTexture(SHROUD), src_pos, dest_pos);
                }
            }
        }
    }
}

void cMapDrawer::drawTerrain()
{
    float tileWidth = mapCamera->getZoomedTileWidth();
    float tileHeight = mapCamera->getZoomedTileHeight();

    int iTileHeight = (tileHeight + 1);
    int iTileWidth = (tileWidth + 1);

    int iPl = m_player->getId();
    int mouseCell = m_player->getGameControlsContext()->getMouseCell();
    cTextDrawer textDrawer = cTextDrawer(bene_font);

    // cache for invalid terran
    auto invalidTile = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, SDL_PIXELFORMAT_ARGB8888);
    Uint32 mappedColor = SDL_MapRGBA(invalidTile->format, 245, 245, 245, 255);
    SDL_FillRect(invalidTile, nullptr, mappedColor);

    // draw vertical rows..
    for (int viewportX = m_camera->getViewportStartX(); viewportX < m_camera->getViewportEndX() + 32; viewportX += 32) {

        // new row
        for (int viewportY = m_camera->getViewportStartY(); viewportY < m_camera->getViewportEndY() + 32; viewportY += 32) {
            int iCell = mapCamera->getCellFromAbsolutePosition(viewportX, viewportY);
            if (iCell < 0) continue;

            // not visible for player, so do not draw
            if (!m_map->isVisible(iCell, iPl)) {
                continue;
            }

            // skip outer border cells
            if (!m_map->isWithinBoundaries(iCell)) {
                continue;
            }

            tCell *cell = m_map->getCell(iCell);

            if (cell == nullptr) {
                continue;
            }

            int absoluteXCoordinateOnMap = m_map->getAbsoluteXPositionFromCell(iCell);
            float fDrawX = mapCamera->getWindowXPosition(absoluteXCoordinateOnMap);

            int absoluteYCoordinateOnMap = m_map->getAbsoluteYPositionFromCell(iCell);
            float fDrawY = mapCamera->getWindowYPosition(absoluteYCoordinateOnMap);

            int iDrawX = round(fDrawX);
            int iDrawY = round(fDrawY);

            // Draw terrain
            if (cell->type < TERRAIN_BLOOM || cell->type > TERRAIN_WALL) {
                // somehow, invalid type
                renderDrawer->renderRectFillColor(iDrawX, iDrawY, iTileWidth, iTileHeight, 245,245,245,255);
            }
            else {
                // valid type
                const cRectangle src_pos = {cell->tile * 32, 0,32, 32};
                cRectangle dest_pos = {iDrawX, iDrawY, iTileWidth, iTileHeight};
                renderDrawer->renderStrechSprite(gfxdata->getTexture(cell->type), src_pos, dest_pos);
            }

            // draw Smudge if necessary
            if (cell->smudgetype > -1 && cell->smudgetile > -1) {
                const cRectangle src_pos = {cell->smudgetile * 32, cell->smudgetype * 32,32, 32};
                cRectangle dest_pos = {iDrawX, iDrawY, iTileWidth, iTileHeight};
                renderDrawer->renderStrechSprite(gfxdata->getTexture(SMUDGE), src_pos, dest_pos);
            }

            // Draw debugging information
            if (game.isDebugMode()) {
                if (mouseCell > -1) {
                    int cellX = (viewportX / 32);
                    int cellY = (viewportY / 32);
                    int mcX = m_map->getCellX(mouseCell);
                    int mcY = m_map->getCellY(mouseCell);

                    if (mcX == cellX && mcY == cellY) {
                        renderDrawer->renderRectFillColor(iDrawX, iDrawY, iTileWidth, iTileHeight,255, 255, 0,96);
                    }
                }

                if (m_drawGrid) {
                    renderDrawer->renderRectColor(iDrawX, iDrawY, iTileWidth, iTileHeight, Color{128, 128, 128,255});
                }
            }

            // Draw more debugging information
            if (m_drawWithoutShroudTiles) {
                // drawCellAsColoredTile(tileWidth, tileHeight, iCell, fDrawX, fDrawY);
            }
        }
    }

    if (game.isDebugMode()) {
//        int absoluteXCoordinate = mapCamera->getAbsMapMouseX(mouse_x);
//        int absoluteYCoordinate = mapCamera->getAbsMapMouseY(mouse_y);
//        cTextDrawer textDrawer = cTextDrawer(bene_font);
//        char msg[255];
//        sprintf(msg, "absMouseX=%d, absMouseY=%d, window mouseX = %d, window mouseY = %d", absoluteXCoordinate, absoluteYCoordinate, mouse_x, mouse_y);
//        logbook(msg);
//        textDrawer.drawText(0, 100, msg);
//
//        _rect(bmp_screen, startX, startY, startX + mapCamera->getAbsViewportWidth(),
//             startY + mapCamera->getAbsViewportHeight(), Color{255, 255, 0));
//
//        cTextDrawer textDrawer = cTextDrawer(bene_font);
//        char msg[255];
//        sprintf(msg, "renderStartX=%d, renderStartY=%d, renderEndX=%d, renderEndY=%d, zoom=%f",
//                mapCamera->getViewportStartX(), mapCamera->getViewportStartY(),
//                mapCamera->getRenderAbsEndX(), mapCamera->getRenderAbsEndY(),
//                mapCamera->getZoomLevel());
//
//        logbook(msg);
    }
}

void cMapDrawer::drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY)
{
    Color iClr = Color{255, 0, 0,255};

    bool bDraw = false;

    if (!m_map->isCellPassable(iCell)) {
        iClr = Color{0, 255, 0,255};
        if (!m_map->isCellPassableFoot(iCell)) {
            iClr = Color{0, 198, 0,255};
        }
        bDraw = true;
    }
    else if (!m_map->isCellPassableFoot(iCell)) {
        iClr = Color{0, 198, 0,255};
        bDraw = true;
    }

    if (m_map->getCellIdStructuresLayer(iCell) > -1) {
        iClr = Color{0, 255, 0,255};
        bDraw = true;
    }

    if (m_map->getCellIdUnitLayer(iCell) > -1) {
        iClr = Color{0, 0, 255,255};
        bDraw = true;
    }

    if (m_map->getCellIdAirUnitLayer(iCell) > -1) {
        iClr = Color{255, 255, 0,255};
        bDraw = true;
    }

    if (bDraw) {
        renderDrawer->renderRectColor(fDrawX, fDrawY, tileWidth, tileHeight, iClr);
    }
}

int cMapDrawer::determineWhichShroudTileToDraw(int cll, int playerId) const
{
    if (cll < 0) return -1;
    if (playerId < HUMAN || playerId >= MAX_PLAYERS) return -1;

    int tile; // Visible stuff, now check for not visible stuff. When found, assign the proper border
    // of shroud to it.
    int above = m_map->getCellAbove(cll);
    int under = m_map->getCellBelow(cll);
    int left = m_map->getCellLeft(cll);
    int right = m_map->getCellRight(cll);

    bool a, u, l, r;
    a = u = l = r = true;

    if (above > -1) {
        if (m_map->isVisible(above, playerId)) {
            a = false;  // visible
        }
    }
    else {
        a = false;
    }

    if (under > -1) {
        if (m_map->isVisible(under, playerId)) {
            u = false;  // visible
        }
    }
    else {
        u = false;
    }

    if (left > -1) {
        if (m_map->isVisible(left, playerId)) {
            l = false;  // visible
        }
    }
    else {
        l = false;
    }

    if (right > -1) {
        if (m_map->isVisible(right, playerId)) {
            r = false;  // visible
        }
    }
    else {
        r = false;
    }

    int t = -1;    // tile id to draw... (x axis)

    // when above is not visible then change this border tile
    if (a == true && u == false && l == false && r == false)
        t = 3;

    if (a == false && u == true && l == false && r == false)
        t = 7;

    if (a == false && u == false && l == true && r == false)
        t = 9;

    if (a == false && u == false && l == false && r == true)
        t = 5;
    // corners

    if (a == true && u == false && l == true && r == false)
        t = 2;

    if (a == true && u == false && l == false && r == true)
        t = 4;

    if (a == false && u == true && l == true && r == false)
        t = 8;

    if (a == false && u == true && l == false && r == true)
        t = 6;

    // 3 connections
    if (a == true && u == true && l == true && r == false)
        t = 10;

    if (a == true && u == true && l == false && r == true)
        t = 12;

    if (a == true && u == false && l == true && r == true)
        t = 11;

    if (a == false && u == true && l == true && r == true)
        t = 13;

    if (a == true && u == true && l == true && r == true)
        t = 1;

    tile = t - 1;
    return tile;
}

void cMapDrawer::setPlayer(cPlayer *thePlayer)
{
    m_player = thePlayer;
}
