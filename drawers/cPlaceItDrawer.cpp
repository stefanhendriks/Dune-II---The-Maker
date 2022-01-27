#include "../include/d2tmh.h"

#include <allegro.h>

cPlaceItDrawer::cPlaceItDrawer(cPlayer * thePlayer) : player(thePlayer) {
}

cPlaceItDrawer::~cPlaceItDrawer() {
    player = nullptr;
}

void cPlaceItDrawer::draw(cBuildingListItem *itemToPlace, int mouseCell) {
    if (itemToPlace == nullptr) {
        return;
    }

	if (mouseCell < 0) {
		return;
	}

	drawStructureIdAtCell(itemToPlace, mouseCell);
	drawStatusOfStructureAtCell(itemToPlace, mouseCell);
}

void cPlaceItDrawer::drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int mouseCell) {
	assert(itemToPlace);
	if (mouseCell < 0) return;

	int structureId = itemToPlace->getBuildId();
	assert(structureId > -1);

	bool bWithinBuildDistance = false;

    int width = sStructureInfo[structureId].bmp_width;
    int height = sStructureInfo[structureId].bmp_height;
    int scaledWidth = mapCamera->factorZoomLevel(width);
    int scaledHeight = mapCamera->factorZoomLevel(height);
    int cellWidth = structureUtils.getWidthOfStructureTypeInCells(structureId);
    int cellHeight = structureUtils.getHeightOfStructureTypeInCells(structureId);

#define SCANWIDTH	1

	int iCellX = map.getCellX(mouseCell);
	int iCellY = map.getCellY(mouseCell);

	// check
	int iStartX = iCellX-SCANWIDTH;
	int iStartY = iCellY-SCANWIDTH;

	int iEndX = iCellX + SCANWIDTH + cellWidth;
	int iEndY = iCellY + SCANWIDTH + cellHeight;

	// Fix up the boundaries
	FIX_POS(iStartX, iStartY);
	FIX_POS(iEndX, iEndY);

	// Determine if structure to be placed is within build distance
	for (int iX=iStartX; iX < iEndX; iX++) {
		for (int iY=iStartY; iY < iEndY; iY++) {
            int iCll = map.getCellWithMapDimensions(iX, iY);

			if (iCll > -1) {
                int idOfStructureAtCell = map.getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    int iID = idOfStructureAtCell;

                    if (structure[iID]->getOwner() == HUMAN) {
                        bWithinBuildDistance = true; // connection!
                        break;
                    }

                    // TODO: Allow placement nearby allies?
                }

                if (map.getCellType(iCll) == TERRAIN_WALL ||
                    map.getCellType(iCll) == TERRAIN_SLAB) {
                    bWithinBuildDistance=true;
                    // TODO: here we should actually find out if the slab is ours or not??
                    break;
                }
            }
		}
	}

	int iDrawX = map.mouse_draw_x();
	int iDrawY = map.mouse_draw_y();

    BITMAP *temp = create_bitmap(scaledWidth+1, scaledHeight+1);
    if (!bWithinBuildDistance) {
        clear_to_color(temp, game.getColorPlaceBad());
    } else {
        clear_bitmap(temp);

        // Draw over it the mask for good/bad placing (decorates temp bitmap)
        for (int iX=0; iX < cellWidth; iX++) {
            for (int iY=0; iY < cellHeight; iY++) {
                int placeColor = game.getColorPlaceNeutral();

                int cellX = iCellX + iX;
                int cellY = iCellY + iY;

                if (!map.isWithinBoundaries(cellX, cellY)) {
                    continue;
                }

                int iCll = map.makeCell(cellX, cellY);

                if (!map.isCellPassable(iCll) || map.getCellType(iCll) != TERRAIN_ROCK) {
                    placeColor = game.getColorPlaceBad();
                }

                if (map.getCellType(iCll) == TERRAIN_SLAB) {
                    placeColor = game.getColorPlaceGood();
                }

                // occupied by units or structures
                int idOfStructureAtCell = map.getCellIdStructuresLayer(iCll);
                if (idOfStructureAtCell > -1) {
                    placeColor = game.getColorPlaceBad();
                }

                int unitIdOnMap = map.getCellIdUnitLayer(iCll);
                if (unitIdOnMap > -1) {
                    // temporarily dead units do not block, but alive units (non-dead) do block placement
                    if (!unit[unitIdOnMap].isDead()) {
                        placeColor = game.getColorPlaceBad();
                    }
                    // TODO: Allow placement, let units move aside when clicking before placement?
                }

                // Draw bad gfx on spot

                float desiredWidth = mapCamera->getZoomedTileWidth();
                float desiredHeight = mapCamera->getZoomedTileHeight();
                float posX = iX * desiredWidth;
                float posY = iY * desiredHeight;
                cRectangle rectangle = cRectangle(posX, posY, desiredWidth, desiredHeight);
                allegroDrawer->drawRectangleFilled(temp, rectangle, placeColor);
            }
        }
    }

	// draw temp bitmap
	set_trans_blender(0, 0, 0, 64);

	draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

	// reset to normal
	set_trans_blender(0, 0, 0, 128);

	destroy_bitmap(temp);
}

void cPlaceItDrawer::drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell) {
	assert(itemToPlace);
	assert(cell >= 0);

	int structureId = itemToPlace->getBuildId();

	int iDrawX = map.mouse_draw_x();
	int iDrawY = map.mouse_draw_y();

    int width = sStructureInfo[structureId].bmp_width;
    int height = sStructureInfo[structureId].bmp_height;

    int scaledWidth = mapCamera->factorZoomLevel(width);
    int scaledHeight = mapCamera->factorZoomLevel(height);

    BITMAP *temp = create_bitmap(scaledWidth+1, scaledHeight+1);
    clear_to_color(temp, makecol(255, 0, 255));

    BITMAP *bmp = nullptr;
    if (structureId == SLAB1) {
		bmp = (BITMAP *)gfxdata[PLACE_SLAB1].dat;
	} else if (structureId == SLAB4) {
        bmp = (BITMAP *)gfxdata[PLACE_SLAB4].dat;
	} else if (structureId == WALL) {
        bmp = (BITMAP *)gfxdata[PLACE_WALL].dat;
	} else {
        bmp = player->getStructureBitmap(structureId);
	}

    allegroDrawer->stretchBlit(bmp, temp, 0, 0, width, height, 0, 0, scaledWidth, scaledHeight);

    draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

    destroy_bitmap(temp);
}
