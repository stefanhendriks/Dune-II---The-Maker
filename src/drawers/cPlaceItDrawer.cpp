#include "cPlaceItDrawer.h"

#include "d2tmc.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMapCamera.h"
#include "map/MapGeometry.hpp"
#include "player/cPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>

#include <cassert>

cPlaceItDrawer::cPlaceItDrawer(cPlayer *thePlayer) : player(thePlayer)
{
}

cPlaceItDrawer::~cPlaceItDrawer()
{
    player = nullptr;
}

void cPlaceItDrawer::draw(cBuildingListItem *itemToPlace, int mouseCell)
{
    if (itemToPlace == nullptr) {
        return;
    }

    if (mouseCell < 0) {
        return;
    }

    drawStructureIdAtMousePos(itemToPlace);
    drawStatusOfStructureAtCell(itemToPlace, mouseCell);
}

void cPlaceItDrawer::drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell)
{
    assert(itemToPlace);
    if (mouseCell < 0) return;
    Color itemToPlaceColor;

    int structureId = itemToPlace->getBuildId();
    assert(structureId > -1);

    bool bWithinBuildDistance = false;

    // int width = sStructureInfo[structureId].bmp_width;
    // int height = sStructureInfo[structureId].bmp_height;
    // int scaledWidth = mapCamera->factorZoomLevel(width);
    // int scaledHeight = mapCamera->factorZoomLevel(height);
    int cellWidth = structureUtils.getWidthOfStructureTypeInCells(structureId);
    int cellHeight = structureUtils.getHeightOfStructureTypeInCells(structureId);

#define SCANWIDTH	1

    int iCellX = global_map.getCellX(mouseCell);
    int iCellY = global_map.getCellY(mouseCell);

    // check
    int iStartX = iCellX-SCANWIDTH;
    int iStartY = iCellY-SCANWIDTH;

    int iEndX = iCellX + SCANWIDTH + cellWidth;
    int iEndY = iCellY + SCANWIDTH + cellHeight;

    // Fix up the boundaries
    cPoint::split(iStartX, iStartY) = global_map.fixCoordinatesToBeWithinMap(iStartX, iStartY);
    cPoint::split(iEndX, iEndY) = global_map.fixCoordinatesToBeWithinMap(iEndX, iEndY);

    // Determine if structure to be placed is within build distance
    for (int iX=iStartX; iX < iEndX; iX++) {
        for (int iY=iStartY; iY < iEndY; iY++) {
            int iCll = global_map.getCellWithMapDimensions(iX, iY);

            if (iCll > -1) {
                int idOfStructureAtCell = global_map.getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    int iID = idOfStructureAtCell;

                    if (structure[iID]->getOwner() == HUMAN) {
                        bWithinBuildDistance = true; // connection!
                        break;
                    }

                    // TODO: Allow placement nearby allies?
                }

                if (global_map.getCellType(iCll) == TERRAIN_WALL ||
                        global_map.getCellType(iCll) == TERRAIN_SLAB) {
                    bWithinBuildDistance=true;
                    // TODO: here we should actually find out if the slab is ours or not??
                    break;
                }
            }
        }
    }

    int iDrawX = global_map.mouse_draw_x();
    int iDrawY = global_map.mouse_draw_y();

    if (!bWithinBuildDistance) {
        itemToPlaceColor = game.getColorPlaceBad();
    }
    else {
        // Draw over it the mask for good/bad placing (decorates temp bitmap)
        for (int iX=0; iX < cellWidth; iX++) {
            for (int iY=0; iY < cellHeight; iY++) {
                itemToPlaceColor = game.getColorPlaceNeutral();

                int cellX = iCellX + iX;
                int cellY = iCellY + iY;

                if (!global_map.isWithinBoundaries(cellX, cellY)) {
                    continue;
                }

                int iCll = global_map.getGeometry()->makeCell(cellX, cellY);

                if (!global_map.isCellPassable(iCll) || global_map.getCellType(iCll) != TERRAIN_ROCK) {
                    itemToPlaceColor = game.getColorPlaceBad();
                }

                if (global_map.getCellType(iCll) == TERRAIN_SLAB) {
                    itemToPlaceColor = game.getColorPlaceGood();
                }

                // occupied by units or structures
                int idOfStructureAtCell = global_map.getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    itemToPlaceColor = game.getColorPlaceBad();
                }

                int unitIdOnMap = global_map.getCellIdUnitLayer(iCll);
                if (unitIdOnMap > -1) {
                    // temporarily dead units do not block, but alive units (non-dead) do block placement
                    if (!unit[unitIdOnMap].isDead()) {
                        itemToPlaceColor = game.getColorPlaceBad();
                    }
                    // TODO: Allow placement, let units move aside when clicking before placement?
                }

                // Draw bad gfx on spot
                float desiredWidth = mapCamera->getZoomedTileWidth();
                float desiredHeight = mapCamera->getZoomedTileHeight();
                float posX = iX * desiredWidth;
                float posY = iY * desiredHeight;
                // cRectangle rectangle = cRectangle(posX, posY, desiredWidth, desiredHeight);
                renderDrawer->renderRectFillColor(iDrawX+posX, iDrawY+posY, desiredWidth, desiredHeight,itemToPlaceColor);
            }
        }
    }
}

void cPlaceItDrawer::drawStructureIdAtMousePos(cBuildingListItem *itemToPlace)
{
    assert(itemToPlace);

    int structureId = itemToPlace->getBuildId();

    int iDrawX = global_map.mouse_draw_x();
    int iDrawY = global_map.mouse_draw_y();

    int width = sStructureInfo[structureId].bmp_width;
    int height = sStructureInfo[structureId].bmp_height;

    int scaledWidth = mapCamera->factorZoomLevel(width);
    int scaledHeight = mapCamera->factorZoomLevel(height);

    Texture *bmp = nullptr;
    if (structureId == SLAB1) {
        bmp = gfxdata->getTexture(PLACE_SLAB1);
    }
    else if (structureId == SLAB4) {
        bmp = gfxdata->getTexture(PLACE_SLAB4);
    }
    else if (structureId == WALL) {
        bmp = gfxdata->getTexture(PLACE_WALL);
    }
    else {
        bmp = player->getStructureBitmap(structureId);
    }
    cRectangle src = { 0, 0, width, height};
    cRectangle dest= {iDrawX, iDrawY, scaledWidth, scaledHeight};
    renderDrawer->renderStrechSprite(bmp, src, dest,96);
}
