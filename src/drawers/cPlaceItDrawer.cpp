#include "cPlaceItDrawer.h"

#include "d2tmc.h"
#include "game/cGame.h"
#include "context/GameContext.hpp"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "map/cMapCamera.h"
#include "map/MapGeometry.hpp"
#include "player/cPlayer.h"
#include "utils/Graphics.hpp"
#include <SDL2/SDL.h>

#include <cassert>

cPlaceItDrawer::cPlaceItDrawer(GameContext *ctx, cPlayer *thePlayer) : player(thePlayer), m_ctx(ctx), m_renderDrawer(ctx->getSDLDrawer())
{
    m_gfxdata = m_ctx->getGraphicsContext()->gfxdata.get();
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
    int cellWidth = structureUtils.getWidthOfStructureTypeInCells(structureId);
    int cellHeight = structureUtils.getHeightOfStructureTypeInCells(structureId);

#define SCANWIDTH	1

    int iCellX = game.getMap().getCellX(mouseCell);
    int iCellY = game.getMap().getCellY(mouseCell);

    // check
    int iStartX = iCellX-SCANWIDTH;
    int iStartY = iCellY-SCANWIDTH;

    int iEndX = iCellX + SCANWIDTH + cellWidth;
    int iEndY = iCellY + SCANWIDTH + cellHeight;

    // Fix up the boundaries
    cPoint::split(iStartX, iStartY) = game.getMap().fixCoordinatesToBeWithinMap(iStartX, iStartY);
    cPoint::split(iEndX, iEndY) = game.getMap().fixCoordinatesToBeWithinMap(iEndX, iEndY);

    // Determine if structure to be placed is within build distance
    for (int iX=iStartX; iX < iEndX; iX++) {
        for (int iY=iStartY; iY < iEndY; iY++) {
            int iCll = game.getMap().getGeometry().getCellWithMapDimensions(iX, iY);

            if (iCll > -1) {
                int idOfStructureAtCell = game.getMap().getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    int iID = idOfStructureAtCell;

                    if (game.getStructures()[iID]->getOwner() == HUMAN) {
                        bWithinBuildDistance = true; // connection!
                        break;
                    }

                    // TODO: Allow placement nearby allies?
                }

                if (game.getMap().getCellType(iCll) == TERRAIN_WALL ||
                        game.getMap().getCellType(iCll) == TERRAIN_SLAB) {
                    bWithinBuildDistance=true;
                    // TODO: here we should actually find out if the slab is ours or not??
                    break;
                }
            }
        }
    }

    int iDrawX = game.getMap().mouse_draw_x();
    int iDrawY = game.getMap().mouse_draw_y();

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

                if (!game.getMap().isWithinBoundaries(cellX, cellY)) {
                    continue;
                }

                int iCll = game.getMap().getGeometry().makeCell(cellX, cellY);

                if (!game.getMap().isCellPassable(iCll) || game.getMap().getCellType(iCll) != TERRAIN_ROCK) {
                    itemToPlaceColor = game.getColorPlaceBad();
                }

                if (game.getMap().getCellType(iCll) == TERRAIN_SLAB) {
                    itemToPlaceColor = game.getColorPlaceGood();
                }

                // occupied by units or structures
                int idOfStructureAtCell = game.getMap().getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    itemToPlaceColor = game.getColorPlaceBad();
                }

                int unitIdOnMap = game.getMap().getCellIdUnitLayer(iCll);
                if (unitIdOnMap > -1) {
                    // temporarily dead units do not block, but alive units (non-dead) do block placement
                    if (!game.getUnit(unitIdOnMap).isDead()) {
                        itemToPlaceColor = game.getColorPlaceBad();
                    }
                    // TODO: Allow placement, let units move aside when clicking before placement?
                }

                // Draw bad gfx on spot
                float desiredWidth = global_mapCamera->getZoomedTileWidth();
                float desiredHeight = global_mapCamera->getZoomedTileHeight();
                float posX = iX * desiredWidth;
                float posY = iY * desiredHeight;
                // cRectangle rectangle = cRectangle(posX, posY, desiredWidth, desiredHeight);
                m_renderDrawer->renderRectFillColor(iDrawX+posX, iDrawY+posY, desiredWidth, desiredHeight,itemToPlaceColor);
            }
        }
    }
}

void cPlaceItDrawer::drawStructureIdAtMousePos(cBuildingListItem *itemToPlace)
{
    assert(itemToPlace);

    int structureId = itemToPlace->getBuildId();

    int iDrawX = game.getMap().mouse_draw_x();
    int iDrawY = game.getMap().mouse_draw_y();

    int width = sStructureInfo[structureId].bmp_width;
    int height = sStructureInfo[structureId].bmp_height;

    int scaledWidth = global_mapCamera->factorZoomLevel(width);
    int scaledHeight = global_mapCamera->factorZoomLevel(height);

    Texture *bmp = nullptr;
    if (structureId == SLAB1) {
        bmp = m_gfxdata->getTexture(PLACE_SLAB1);
    }
    else if (structureId == SLAB4) {
        bmp = m_gfxdata->getTexture(PLACE_SLAB4);
    }
    else if (structureId == WALL) {
        bmp = m_gfxdata->getTexture(PLACE_WALL);
    }
    else {
        bmp = player->getStructureBitmap(structureId);
    }
    // We need to take a source rectangle, as the structure BMP contains multiple frames of the structure
    // therefore we cannot use renderStrechFullSprite (as that assumes the source texture can be used as a whole
    // which cannot be used in this case)
    cRectangle src = { 0, 0, width, height}; // takes first frame
    cRectangle dest= {iDrawX, iDrawY, scaledWidth, scaledHeight};
    m_renderDrawer->renderStrechSprite(bmp, src, dest,96);
}
