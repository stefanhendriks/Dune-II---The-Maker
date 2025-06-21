#include "cMapDrawer.h"

#include "controls/cGameControlsContext.h"
#include "data/gfxdata.h"
#include "d2tmc.h"
#include "drawers/cAllegroDrawer.h"
#include "drawers/cTextDrawer.h"
#include "player/cPlayer.h"

#include <allegro.h>
#include <SDL2/SDL.h>

#include <cmath>

cMapDrawer::cMapDrawer(cMap *map, cPlayer *player, cMapCamera *camera) :
    m_map(map),
    m_player(player),
    m_camera(camera),
    m_BmpTemp(nullptr),
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
    if (m_BmpTemp) {
        destroy_bitmap(m_BmpTemp);
    }
}

void cMapDrawer::drawShroud()
{
    set_trans_blender(0, 0, 0, 128);

    float tileWidth = mapCamera->getZoomedTileWidth();
    float tileHeight = mapCamera->getZoomedTileHeight();

    int iTileHeight = (tileHeight + 1);
    int iTileWidth = (tileWidth + 1);

    SDL_Surface *temp = SDL_CreateRGBSurface(0,iTileWidth, iTileHeight,32,0,0,0,255);

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

            if (m_drawWithoutShroudTiles) {
                if (m_map->isVisible(iCell, iPl)) {
                    // do nothing
                }
                else {
                    //_rectfill(bmp_screen, fDrawX, fDrawY, fDrawX + tileWidth, fDrawY + tileHeight, SDL_Color{0, 0, 0));
                    renderDrawer->drawRectFilled(bmp_screen, fDrawX, fDrawY, tileWidth, tileHeight, SDL_Color{0, 0, 0,255});
                }
            }
            else {
                if (m_map->isVisible(iCell, iPl)) {
                    int tile = determineWhichShroudTileToDraw(iCell, iPl);

                    if (tile > -1) {
                        renderDrawer->maskedStretchBlitFromGfxData(SHROUD, bmp_screen, tile * 32, 0, 32, 32, fDrawX,
                                fDrawY, iTileWidth, iTileHeight);
                        clear_to_color(temp, makecol(255, 0, 255));

                        renderDrawer->maskedStretchBlitFromGfxData(SHROUD_SHADOW, temp, tile * 32, 0, 32, 32, 0, 0,
                                iTileWidth, iTileHeight);
                        draw_trans_sprite(bmp_screen, temp, fDrawX, fDrawY);
                    }
                }
                else {
                    // NOT VISIBLE, DO NOT DRAW A THING THEN!
                    // Except when there is a building here, that should not be visible ;)
                    // tile 0 of shroud is entirely black... (effectively the same as drawing a rect here)
                    renderDrawer->maskedStretchBlitFromGfxData(SHROUD, bmp_screen, 0, 0, 32, 32, fDrawX, fDrawY,
                            iTileWidth, iTileHeight);
                }
            }
        }
    }

    destroy_bitmap(temp);
}

void cMapDrawer::drawTerrain()
{
    if (m_BmpTemp == nullptr) {
        m_BmpTemp = SDL_CreateRGBSurface(0,32, 32,32,0,0,0,255);
    }

    float tileWidth = mapCamera->getZoomedTileWidth();
    float tileHeight = mapCamera->getZoomedTileHeight();

    int iTileHeight = (tileHeight + 1);
    int iTileWidth = (tileWidth + 1);

    int iPl = m_player->getId();
    int mouseCell = m_player->getGameControlsContext()->getMouseCell();
    cTextDrawer textDrawer = cTextDrawer(bene_font);

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

            // Draw terrain
            if (cell->type < TERRAIN_BLOOM || cell->type > TERRAIN_WALL) {
                // somehow, invalid type
                cRectangle rectangle = cRectangle(0, 0, 32, 32);
                renderDrawer->drawRectFilled(m_BmpTemp, rectangle, SDL_Color{245, 245, 245, 255});
            }
            else {
                // valid type
                blit(gfxdata->getSurface(cell->type),
                     m_BmpTemp,
                     cell->tile * 32, 0, // keep 32 here, because in BMP this is the size of the tiles
                     0, 0,
                     32, 32
                    );
            }

            // draw Smudge if necessary
            if (cell->smudgetype > -1 && cell->smudgetile > -1) {
                // no need to stretch here, we stretch m_BmpTemp below
                renderDrawer->maskedBlitFromGfxData(SMUDGE, m_BmpTemp,
                                                    cell->smudgetile * 32,
                                                    cell->smudgetype * 32,
                                                    0,
                                                    0,
                                                    32,
                                                    32);
            }

            int iDrawX = round(fDrawX);
            int iDrawY = round(fDrawY);
            renderDrawer->stretchBlit(m_BmpTemp, bmp_screen, 0, 0, 32, 32, iDrawX, iDrawY, iTileWidth, iTileHeight);

            // Draw debugging information
            if (game.isDebugMode()) {
                if (mouseCell > -1) {
                    int cellX = (viewportX / 32);
                    int cellY = (viewportY / 32);
                    int mcX = m_map->getCellX(mouseCell);
                    int mcY = m_map->getCellY(mouseCell);

                    if (mcX == cellX && mcY == cellY) {
                        renderDrawer->drawRectTransparentFilled(bmp_screen, {iDrawX, iDrawY, iTileWidth, iTileHeight},
                                                                SDL_Color{255, 255, 0,96});
                    }
                }

                if (m_drawGrid) {
                    //_rect(bmp_screen, iDrawX, iDrawY, iDrawX + iTileWidth, iDrawY + iTileHeight, SDL_Color{128, 128, 128));
                    renderDrawer->drawRect(bmp_screen, iDrawX, iDrawY, iTileWidth, iTileHeight, SDL_Color{128, 128, 128,255});
                }
            }

            // Draw more debugging information
            if (m_drawWithoutShroudTiles) {
                drawCellAsColoredTile(tileWidth, tileHeight, iCell, fDrawX, fDrawY);
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
//             startY + mapCamera->getAbsViewportHeight(), SDL_Color{255, 255, 0));
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
    SDL_Color iClr = SDL_Color{255, 0, 0,255};

    bool bDraw = false;

    if (!m_map->isCellPassable(iCell)) {
        iClr = SDL_Color{0, 255, 0,255};
        if (!m_map->isCellPassableFoot(iCell)) {
            iClr = SDL_Color{0, 198, 0,255};
        }
        bDraw = true;
    }
    else if (!m_map->isCellPassableFoot(iCell)) {
        iClr = SDL_Color{0, 198, 0,255};
        bDraw = true;
    }

    if (m_map->getCellIdStructuresLayer(iCell) > -1) {
        iClr = SDL_Color{0, 255, 0,255};
        bDraw = true;
    }

    if (m_map->getCellIdUnitLayer(iCell) > -1) {
        iClr = SDL_Color{0, 0, 255,255};
        bDraw = true;
    }

    if (m_map->getCellIdAirUnitLayer(iCell) > -1) {
        iClr = SDL_Color{255, 255, 0,255};
        bDraw = true;
    }

    if (bDraw) {
        //_rect(bmp_screen, fDrawX, fDrawY, fDrawX + (tileWidth - 1), fDrawY + (tileHeight - 1), iClr);
        renderDrawer->drawRect(bmp_screen, fDrawX, fDrawY, tileWidth, tileHeight, iClr);
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
