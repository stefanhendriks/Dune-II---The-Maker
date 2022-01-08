#include <cmath>
#include "../include/d2tmh.h"
#include "cMapDrawer.h"


cMapDrawer::cMapDrawer(cMap *theMap, cPlayer *thePlayer, cMapCamera *theCamera) : player(thePlayer) {
    assert(theMap);
    assert(theCamera);
    map = theMap;
    camera = theCamera;
    bmp_temp = nullptr;
}

cMapDrawer::~cMapDrawer() {
    map = nullptr;
    camera = nullptr;
    player = nullptr;
    if (bmp_temp) {
        destroy_bitmap(bmp_temp);
    }
}

void cMapDrawer::drawShroud() {
    set_trans_blender(0, 0, 0, 128);

    float tileWidth = mapCamera->getZoomedTileWidth();
    float tileHeight = mapCamera->getZoomedTileHeight();

    int iTileHeight = (tileHeight + 1);
    int iTileWidth = (tileWidth + 1);

    int colorDepthScreen = bitmap_color_depth(bmp_screen);
    BITMAP *temp = create_bitmap_ex(colorDepthScreen, iTileWidth, iTileHeight);

    int iPl = player->getId();

    for (int viewportX = camera->getViewportStartX(); viewportX < camera->getViewportEndX() + 32; viewportX += 32) {

        // new row
        for (int viewportY = camera->getViewportStartY(); viewportY < camera->getViewportEndY() + 32; viewportY += 32) {
            int iCell = mapCamera->getCellFromAbsolutePosition(viewportX, viewportY);

            if (iCell < 0) continue;

            int absoluteXCoordinateOnMap = map->getAbsoluteXPositionFromCell(iCell);
            float fDrawX = mapCamera->getWindowXPosition(absoluteXCoordinateOnMap);

            int absoluteYCoordinateOnMap = map->getAbsoluteYPositionFromCell(iCell);
            float fDrawY = mapCamera->getWindowYPosition(absoluteYCoordinateOnMap);

            if (DEBUGGING && key[KEY_D] && key[KEY_TAB]) {
                if (map->isVisible(iCell, iPl)) {
                    // do nothing
                } else {
                    rectfill(bmp_screen, fDrawX, fDrawY, fDrawX + tileWidth, fDrawY + tileHeight, makecol(0, 0, 0));
                }
            } else {
                if (map->isVisible(iCell, iPl)) {
                    int tile = determineWhichShroudTileToDraw(iCell, iPl);

                    if (tile > -1) {
                        allegroDrawer->maskedStretchBlitFromGfxData(SHROUD, bmp_screen, tile * 32, 0, 32, 32, fDrawX,
                                                                    fDrawY, iTileWidth, iTileHeight);
                        clear_to_color(temp, makecol(255, 0, 255));

                        allegroDrawer->maskedStretchBlitFromGfxData(SHROUD_SHADOW, temp, tile * 32, 0, 32, 32, 0, 0,
                                                                    iTileWidth, iTileHeight);
                        draw_trans_sprite(bmp_screen, temp, fDrawX, fDrawY);
                    }
                } else {
                    // NOT VISIBLE, DO NOT DRAW A THING THEN!
                    // Except when there is a building here, that should not be visible ;)
                    // tile 0 of shroud is entirely black... (effectively the same as drawing a rect here)
                    allegroDrawer->maskedStretchBlitFromGfxData(SHROUD, bmp_screen, 0, 0, 32, 32, fDrawX, fDrawY,
                                                                iTileWidth, iTileHeight);
                }
            }
        }
    }

    destroy_bitmap(temp);
}

void cMapDrawer::drawTerrain() {
    if (bmp_temp == nullptr) {
        int colorDepthScreen = bitmap_color_depth(bmp_screen);
        bmp_temp = create_bitmap_ex(colorDepthScreen, 32, 32);
    }

    float tileWidth = mapCamera->getZoomedTileWidth();
    float tileHeight = mapCamera->getZoomedTileHeight();

    int iTileHeight = (tileHeight + 1);
    int iTileWidth = (tileWidth + 1);

    int iPl = player->getId();
    int mouseCell = player->getGameControlsContext()->getMouseCell();
    cTextDrawer textDrawer = cTextDrawer(bene_font);

    // draw vertical rows..
    for (int viewportX = camera->getViewportStartX(); viewportX < camera->getViewportEndX() + 32; viewportX += 32) {

        // new row
        for (int viewportY = camera->getViewportStartY(); viewportY < camera->getViewportEndY() + 32; viewportY += 32) {
            int iCell = mapCamera->getCellFromAbsolutePosition(viewportX, viewportY);
            if (iCell < 0) continue;

            // not visible for player, so do not draw
            if (!map->isVisible(iCell, iPl)) {
                continue;
            }

            // skip outer border cells
            if (!map->isWithinBoundaries(iCell)) {
                continue;
            }

            tCell *cell = map->getCell(iCell);

            if (cell == nullptr) {
                continue;
            }

            int absoluteXCoordinateOnMap = map->getAbsoluteXPositionFromCell(iCell);
            float fDrawX = mapCamera->getWindowXPosition(absoluteXCoordinateOnMap);

            int absoluteYCoordinateOnMap = map->getAbsoluteYPositionFromCell(iCell);
            float fDrawY = mapCamera->getWindowYPosition(absoluteYCoordinateOnMap);

            // Draw terrain
            if (cell->type < TERRAIN_BLOOM || cell->type > TERRAIN_WALL) {
                // somehow, invalid type
                cRectangle rectangle = cRectangle(0, 0, 32, 32);
                allegroDrawer->drawRectangleFilled(bmp_temp, rectangle, makecol(245, 245, 245));
            } else {
                // valid type
                blit((BITMAP *) gfxdata[cell->type].dat,
                     bmp_temp,
                     cell->tile * 32, 0, // keep 32 here, because in BMP this is the size of the tiles
                     0, 0,
                     32, 32
                );
            }

            // draw Smudge if necessary
            if (cell->smudgetype > -1 && cell->smudgetile > -1) {
                // no need to stretch here, we stretch bmp_temp below
                allegroDrawer->maskedBlitFromGfxData(SMUDGE, bmp_temp,
                                                     cell->smudgetile * 32,
                                                     cell->smudgetype * 32,
                                                     0,
                                                     0,
                                                     32,
                                                     32);
            }

            int iDrawX = round(fDrawX);
            int iDrawY = round(fDrawY);
            allegroDrawer->stretchBlit(bmp_temp, bmp_screen, 0, 0, 32, 32, iDrawX, iDrawY, iTileWidth, iTileHeight);

            // Draw debugging information
            if (DEBUGGING) {
                if (mouseCell > -1) {
                    int cellX = (viewportX / 32);
                    int cellY = (viewportY / 32);
                    int mcX = map->getCellX(mouseCell);
                    int mcY = map->getCellY(mouseCell);

                    if (mcX == cellX && mcY == cellY) {
                      allegroDrawer->drawRectangleTransparentFilled(bmp_screen, {iDrawX, iDrawY, iTileWidth, iTileHeight},
                                                                    makecol(255, 255, 0), 96);
                    }
                }

                if (key[KEY_G]) {
                    rect(bmp_screen, iDrawX, iDrawY, iDrawX + iTileWidth, iDrawY + iTileHeight, makecol(128, 128, 128));
                }
            }

            // Draw more debugging information
            if (DEBUGGING && key[KEY_D]) {
                drawCellAsColoredTile(tileWidth, tileHeight, iCell, fDrawX, fDrawY);
//                textDrawer.drawText(fDrawX, fDrawY, "%d", cell->type);
            }
        }
    }

    if (DEBUGGING) {
//        int absoluteXCoordinate = mapCamera->getAbsMapMouseX(mouse_x);
//        int absoluteYCoordinate = mapCamera->getAbsMapMouseY(mouse_y);
//        cTextDrawer textDrawer = cTextDrawer(bene_font);
//        char msg[255];
//        sprintf(msg, "absMouseX=%d, absMouseY=%d, window mouseX = %d, window mouseY = %d", absoluteXCoordinate, absoluteYCoordinate, mouse_x, mouse_y);
//        logbook(msg);
//        textDrawer.drawText(0, 100, msg);
//
//        rect(bmp_screen, startX, startY, startX + mapCamera->getAbsViewportWidth(),
//             startY + mapCamera->getAbsViewportHeight(), makecol(255, 255, 0));
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

void cMapDrawer::drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY) {
    int iClr = makecol(255, 0, 0);

    bool bDraw = false;

    if (!map->isCellPassable(iCell)) {
        iClr = makecol(0, 255, 0);
        if (!map->isCellPassableFoot(iCell)) {
            iClr = makecol(0, 198, 0);
        }
        bDraw = true;
    } else if (!map->isCellPassableFoot(iCell)) {
        iClr = makecol(0, 198, 0);
        bDraw = true;
    }

    if (map->getCellIdStructuresLayer(iCell) > -1) {
        iClr = makecol(0, 255, 0);
        bDraw = true;
    }

    if (map->getCellIdUnitLayer(iCell) > -1) {
        iClr = makecol(0, 0, 255);
        bDraw = true;
    }

    if (map->getCellIdAirUnitLayer(iCell) > -1) {
        iClr = makecol(255, 255, 0);
        bDraw = true;
    }

    if (bDraw) {
        rect(bmp_screen, fDrawX, fDrawY, fDrawX + (tileWidth - 1), fDrawY + (tileHeight - 1), iClr);
    }
}

int cMapDrawer::determineWhichShroudTileToDraw(int cll, int playerId) const {
    if (cll < 0) return -1;
    if (playerId < HUMAN || playerId >= MAX_PLAYERS) return -1;

    int tile; // Visible stuff, now check for not visible stuff. When found, assign the proper border
    // of shroud to it.
    int above = CELL_ABOVE(cll);
    int under = CELL_UNDER(cll);
    int left = CELL_LEFT(cll);
    int right = CELL_RIGHT(cll);

    bool a, u, l, r;
    a = u = l = r = true;

    if (above > -1) {
        if (map->isVisible(above, playerId)) {
            a = false;  // visible
        }
    } else {
        a = false;
    }

    if (under > -1) {
        if (map->isVisible(under, playerId)) {
            u = false;  // visible
        }
    } else {
        u = false;
    }

    if (left > -1) {
        if (map->isVisible(left, playerId)) {
            l = false;  // visible
        }
    } else {
        l = false;
    }

    if (right > -1) {
        if (map->isVisible(right, playerId)) {
            r = false;  // visible
        }
    } else {
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

void cMapDrawer::setPlayer(cPlayer *thePlayer) {
    player = thePlayer;
}
