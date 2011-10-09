#include "../include/d2tmh.h"

#include "../utils/CellCalculator.h"

PlaceItDrawer::PlaceItDrawer() {
}

PlaceItDrawer::~PlaceItDrawer() {
}

void PlaceItDrawer::draw(cBuildingListItem *itemToPlace) {
	assert(itemToPlace);
	assert(itemToPlace->getBuildType() == STRUCTURE);

	// this is only done when bPlaceIt=true
	if (player[HUMAN].getSideBar() == NULL) {
		return;
	}

	int iMouseCell = player[HUMAN].getGameControlsContext()->getMouseCell();

	if (iMouseCell < 0) {
		return;
	}

	drawStructureIdAtCell(itemToPlace, iMouseCell);
	drawStatusOfStructureAtCell(itemToPlace, iMouseCell);
}

void PlaceItDrawer::drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int cell) {
	assert(itemToPlace);
	if (cell < 0)
		return;

	cStructureUtils structureUtils;
	int structureId = itemToPlace->getBuildId();
	assert(structureId > -1);

	bool bOutOfBorder = true;
	bool bMayPlace = true;

	int iTile = PLACE_ROCK; // rocky placement = ok, but bad for power

	BITMAP *temp;
	temp = create_bitmap(structures[structureId].bmp_width, structures[structureId].bmp_height);
	clear_bitmap(temp);

	int width = structureUtils.getWidthOfStructureTypeInCells(structureId);
	int height = structureUtils.getHeightOfStructureTypeInCells(structureId);

	int iTotalBlocks = width * height;

	int iTotalRocks = 0.0f;

#define SCANWIDTH	1

	CellCalculator * cellCalculator = new CellCalculator(map);

	int iCellX = cellCalculator->getX(cell);
	int iCellY = cellCalculator->getY(cell);

	delete cellCalculator;

	// check
	int iStartX = iCellX - SCANWIDTH;
	int iStartY = iCellY - SCANWIDTH;

	int iEndX = iCellX + SCANWIDTH + width;
	int iEndY = iCellY + SCANWIDTH + height;

	// Fix up the boundaries
	FIX_POS(iStartX, iStartY);
	FIX_POS(iEndX, iEndY);

	// Determine if the structure may be placed or not (true/false)
	for (int iX = iStartX; iX < iEndX; iX++) {
		for (int iY = iStartY; iY < iEndY; iY++) {
			int iCll = iCellMake(iX, iY);
			if (map->cell[iCll].gameObjectId[MAPID_STRUCTURES] > -1) {
				int iID = map->cell[iCll].gameObjectId[MAPID_STRUCTURES];

				if (structure[iID]->isOwnerHuman()) {
					bOutOfBorder = false; // connection!
				} else {
					bMayPlace = false;
				}
			}

			if (map->cell[iCll].terrainTypeGfxDataIndex == TERRAIN_WALL || map->cell[iCll].terrainTypeGfxDataIndex == TERRAIN_SLAB) {
				bOutOfBorder = false;
				// TODO: here we should actually find out if the slab is ours or not??
			}
		}
	}

	if (bOutOfBorder) {
		bMayPlace = false;
	}

	int iDrawX = map->mouse_draw_x();
	int iDrawY = map->mouse_draw_y();

	// Draw over it the mask for good/bad placing (decorates temp bitmap)
	for (int iX = 0; iX < width; iX++) {
		for (int iY = 0; iY < height; iY++) {
			iTile = PLACE_ROCK;

			if ((iCellX + iX > 62) || (iCellY + iY > 62)) {
				bOutOfBorder = true;
				bMayPlace = false;
				break;
			}

			int iCll = iCellMake((iCellX + iX), (iCellY + iY));

			if (map->cell[iCll].passable == false)
				iTile = PLACE_BAD;

			if (map->cell[iCll].terrainTypeGfxDataIndex != TERRAIN_ROCK)
				iTile = PLACE_BAD;

			if (map->cell[iCll].terrainTypeGfxDataIndex == TERRAIN_SLAB)
				iTile = PLACE_GOOD;

			// occupied by units or structures
			if (map->cell[iCll].gameObjectId[MAPID_STRUCTURES] > -1)
				iTile = PLACE_BAD;

			int unitIdOnMap = map->cell[iCll].gameObjectId[MAPID_UNITS];
			if (unitIdOnMap > -1) {
				if (!unit[unitIdOnMap].bPickedUp) // only when not picked up, take this in (fixes carryall carrying this unit bug)
					iTile = PLACE_BAD;
			}

			// DRAWING & RULER
			if (iTile == PLACE_BAD && structureId != SLAB4)
				bMayPlace = false;

			// Count this as GOOD stuff
			if (iTile == PLACE_GOOD)
				iTotalRocks++;

			// Draw bad gfx on spot
			draw_sprite(temp, (BITMAP *) gfxdata[iTile].dat, iX * 32, iY * 32);
		}

		if (bOutOfBorder) {
			clear_to_color(temp, makecol(160, 0, 0));
		}
	}

	// draw temp bitmap
	set_trans_blender(0, 0, 0, 64);

	draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

	// reset to normal
	set_trans_blender(0, 0, 0, 128);

	destroy_bitmap(temp);

	// clicked mouse button
	if (cMouse::getInstance()->isLeftButtonClicked()) {
		if (bMayPlace && bOutOfBorder == false) {
			int iHealthPercent = 50; // the minimum is 50% (with no slabs)

			if (iTotalRocks > 0) {
				iHealthPercent += health_bar(50, iTotalRocks, iTotalBlocks);
			}

			play_sound_id(SOUND_PLACE, -1);

			//cStructureFactory::getInstance()->createStructure(iMouseCell, iStructureID, 0, iHealthPercent);

			player[HUMAN].getStructurePlacer()->placeStructure(cell, structureId, iHealthPercent);

			game.bPlaceIt = false;

			itemToPlace->decreaseTimesToBuild();
			itemToPlace->setPlaceIt(false);
			itemToPlace->setIsBuilding(false);
			itemToPlace->setProgress(0);
			if (itemToPlace->getTimesToBuild() < 1) {
				player[HUMAN].getItemBuilder()->removeItemFromList(itemToPlace);
			}
		}
	}
	//iDrawX *=32;
	//iDrawY *=32;

	//rect(bmp_screen, iDrawX, iDrawY, iDrawX+(iWidth*32), iDrawY+(iHeight*32), makecol(255,255,255));
}

void PlaceItDrawer::drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell) {
	assert(itemToPlace);
	assert(cell >= 0);

	int structureId = itemToPlace->getBuildId();

	int iDrawX = map->mouse_draw_x();
	int iDrawY = map->mouse_draw_y();

	if (structureId == SLAB1) {
		draw_trans_sprite(bmp_screen, (BITMAP *) gfxdata[PLACE_SLAB1].dat, iDrawX, iDrawY);
	} else if (structureId == SLAB4) {
		draw_trans_sprite(bmp_screen, (BITMAP *) gfxdata[PLACE_SLAB4].dat, iDrawX, iDrawY);
	} else if (structureId == WALL) {
		draw_trans_sprite(bmp_screen, (BITMAP *) gfxdata[PLACE_WALL].dat, iDrawX, iDrawY);
	} else {
		BITMAP *temp;
		temp = create_bitmap(structures[structureId].bmp_width, structures[structureId].bmp_height);
		clear_bitmap(temp);

		blit(structures[structureId].bmp, temp, 0, 0, 0, 0, structures[structureId].bmp_width, structures[structureId].bmp_height);

		draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

		destroy_bitmap(temp);
	}

}
