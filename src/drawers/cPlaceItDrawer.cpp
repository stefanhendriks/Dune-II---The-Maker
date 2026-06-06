#include "cPlaceItDrawer.h"
#include "utils/cStructureUtils.h"

#include "include/sGameServices.h"
#include "context/GameContext.hpp"
#include "game/cGameInterface.h"
#include "data/gfxdata.h"
#include "drawers/SDLDrawer.hpp"
#include "gameobjects/map/cMapCamera.h"
#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/players/cPlayer.h"
#include "utils/Graphics.hpp"
#include "gameobjects/map/cMap.h"
#include "gameobjects/structures/cStructures.h"
#include "gameobjects/units/cUnits.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include <SDL3/SDL.h>

#include "include/cAssert.h"

cPlaceItDrawer::cPlaceItDrawer(GameContext *ctx, cPlayer *thePlayer, cStructureUtils *structureUtils) : m_structureUtils(structureUtils), m_player(thePlayer), m_ctx(ctx), m_renderDrawer(ctx->getSDLDrawer())
{
    d2tm_assert(thePlayer != nullptr);
    d2tm_assert(ctx != nullptr);
    m_gfxdata = m_ctx->getGraphicsContext()->gfxdata.get();
}

cPlaceItDrawer::~cPlaceItDrawer()
{
    m_player = nullptr;
}

void cPlaceItDrawer::serviceInit(sGameServices* services)
{
    m_objects = services->objects;
    m_infos = services->info;
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
    d2tm_assert(itemToPlace);
    if (mouseCell < 0) return;
    Color itemToPlaceColor;

    int structureId = itemToPlace->getBuildId();
    d2tm_assert(structureId > -1);

    bool bWithinBuildDistance = false;
    int cellWidth = m_structureUtils->getWidthOfStructureTypeInCells(structureId);
    int cellHeight = m_structureUtils->getHeightOfStructureTypeInCells(structureId);

#define SCANWIDTH	1

    int iCellX = m_objects->getMapGeometry()->getCellX(mouseCell);
    int iCellY = m_objects->getMapGeometry()->getCellY(mouseCell);

    // check
    int iStartX = iCellX-SCANWIDTH;
    int iStartY = iCellY-SCANWIDTH;

    int iEndX = iCellX + SCANWIDTH + cellWidth;
    int iEndY = iCellY + SCANWIDTH + cellHeight;

    // Fix up the boundaries
    cPoint::split(iStartX, iStartY) = m_objects->getMapGeometry()->fixCoordinatesToBeWithinMap(iStartX, iStartY);
    cPoint::split(iEndX, iEndY) = m_objects->getMapGeometry()->fixCoordinatesToBeWithinMap(iEndX, iEndY);

    // Determine if structure to be placed is within build distance
    for (int iX=iStartX; iX < iEndX; iX++) {
        for (int iY=iStartY; iY < iEndY; iY++) {
            int iCll = m_objects->getMapGeometry()->getCellWithMapDimensions(iX, iY);

            if (iCll > -1) {
                int idOfStructureAtCell = m_objects->getMap()->getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    int iID = idOfStructureAtCell;

                    if (m_objects->getStructures()[iID]->getOwner() == HUMAN) {
                        bWithinBuildDistance = true; // connection!
                        break;
                    }

                    // TODO: Allow placement nearby allies?
                }

                if (m_objects->getMap()->getCellType(iCll) == TERRAIN_WALL ||
                        m_objects->getMap()->getCellType(iCll) == TERRAIN_SLAB) {
                    bWithinBuildDistance=true;
                    // TODO: here we should actually find out if the slab is ours or not??
                    break;
                }
            }
        }
    }

    int iDrawX = m_objects->getMap()->mouse_draw_x();
    int iDrawY = m_objects->getMap()->mouse_draw_y();

    if (!bWithinBuildDistance) {
        itemToPlaceColor = Color::PlaceNeutral;
    }
    else {
        // Draw over it the mask for good/bad placing (decorates temp bitmap)
        for (int iX=0; iX < cellWidth; iX++) {
            for (int iY=0; iY < cellHeight; iY++) {
                itemToPlaceColor = Color::PlaceNeutral;

                int cellX = iCellX + iX;
                int cellY = iCellY + iY;

                if (!m_objects->getMapGeometry()->isWithinBoundaries(cellX, cellY)) {
                    continue;
                }

                int iCll = m_objects->getMapGeometry()->makeCell(cellX, cellY);

                if (!m_objects->getMap()->isCellPassable(iCll) || m_objects->getMap()->getCellType(iCll) != TERRAIN_ROCK) {
                    itemToPlaceColor = Color::PlaceBad;
                }

                if (m_objects->getMap()->getCellType(iCll) == TERRAIN_SLAB) {
                    itemToPlaceColor = Color::PlaceGood;
                }

                // occupied by units or structures
                int idOfStructureAtCell = m_objects->getMap()->getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    itemToPlaceColor = Color::PlaceBad;
                }

                int unitIdOnMap = m_objects->getMap()->getCellIdUnitLayer(iCll);
                if (unitIdOnMap > -1) {
                    // temporarily dead units do not block, but alive units (non-dead) do block placement
                    if (!m_objects->getUnit(unitIdOnMap)->isDead()) {
                        itemToPlaceColor = Color::PlaceBad;
                    }
                    // TODO: Allow placement, let units move aside when clicking before placement?
                }

                // Draw bad gfx on spot
                float desiredWidth = m_ctx->getGameInterface()->getMapCamera()->getZoomedTileWidth();
                float desiredHeight = m_ctx->getGameInterface()->getMapCamera()->getZoomedTileHeight();
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
    d2tm_assert(itemToPlace);

    int structureId = itemToPlace->getBuildId();

    int iDrawX = m_objects->getMap()->mouse_draw_x();
    int iDrawY = m_objects->getMap()->mouse_draw_y();

    int width = m_infos->getStructureInfo(structureId).bmp_width;
    int height = m_infos->getStructureInfo(structureId).bmp_height;

    int scaledWidth = m_ctx->getGameInterface()->getMapCamera()->factorZoomLevel(width);
    int scaledHeight = m_ctx->getGameInterface()->getMapCamera()->factorZoomLevel(height);

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
        bmp = m_player->getStructureBitmap(structureId);
    }
    // We need to take a source rectangle, as the structure BMP contains multiple frames of the structure
    // therefore we cannot use renderStrechFullSprite (as that assumes the source texture can be used as a whole
    // which cannot be used in this case)
    cRectangle src = { 0, 0, width, height}; // takes first frame
    cRectangle dest= {iDrawX, iDrawY, scaledWidth, scaledHeight};
    m_renderDrawer->renderStrechSprite(bmp, src, dest,96);
}
