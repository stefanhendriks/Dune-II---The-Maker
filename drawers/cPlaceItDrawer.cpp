/*
 * cPlaceItDrawer.cpp
 *
 *  Created on: 12-aug-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cPlaceItDrawer::cPlaceItDrawer() {
}

cPlaceItDrawer::~cPlaceItDrawer() {
}

void cPlaceItDrawer::draw(cBuildingListItem *itemToPlace) {
	assert(itemToPlace);
	assert(itemToPlace->getBuildType() == STRUCTURE);

	// this is only done when bPlaceIt=true
	if (player[HUMAN].getSideBar() == NULL) {
		return;
	}

	int iMouseCell = map.mouse_cell();

	if (iMouseCell < 0) {
		return;
	}

	drawStructureIdAtCell(itemToPlace, iMouseCell);
	drawStatusOfStructureAtCell(itemToPlace, iMouseCell);
}

void cPlaceItDrawer::drawStatusOfStructureAtCell(cBuildingListItem *itemToPlace, int cell) {
	assert(itemToPlace);
	cStructureUtils structureUtils;
	cCellCalculator cellCalculator;
	int structureId = itemToPlace->getBuildId();


	bool bOutOfBorder=true;
	bool bMayPlace=true;

	int iTile = PLACE_ROCK;	// rocky placement = ok, but bad for power

	BITMAP *temp;
	temp = create_bitmap(structures[structureId].bmp_width, structures[structureId].bmp_height);
	clear_bitmap(temp);

	int width = structureUtils.getWidthOfStructureTypeInCells(structureId);
	int height = structureUtils.getHeightOfStructureTypeInCells(structureId);

	int iTotalBlocks = width * height;

	int iTotalRocks=0.0f;

#define SCANWIDTH	1

	int iCellX = cellCalculator.getX(cell);
	int iCellY = cellCalculator.getX(cell);

	// check
	int iStartX = iCellX-SCANWIDTH;
	int iStartY = iCellY-SCANWIDTH;

	int iEndX = iCellX + SCANWIDTH + width;
	int iEndY = iCellY + SCANWIDTH + height;

	// Fix up the boundries
	FIX_POS(iStartX, iStartY);
	FIX_POS(iEndX, iEndY);


	for (int iX=iStartX; iX < iEndX; iX++)
		for (int iY=iStartY; iY < iEndY; iY++)
		{
			int iCll=iCellMake(iX, iY);
			if (map.cell[iCll].id[MAPID_STRUCTURES] > -1)
			{
				int iID = map.cell[iCll].id[MAPID_STRUCTURES];

				if (structure[iID]->getOwner() == 0)
					bOutOfBorder=false; // connection!
				else
					bMayPlace=false;
			}

			if (map.cell[iCll].type == TERRAIN_WALL ||
				map.cell[iCll].type == TERRAIN_SLAB)
			{
				bOutOfBorder=false;
				// here we should actually find out if the slab is ours or not...
			}
		}

	if (bOutOfBorder) {
		bMayPlace=false;
	}

	/*


	// Find closest building to X,Y, position.
	for (int i=0; i < MAX_STRUCTURES; i++)
		if (structure[i].isValid())
			if (structure[i].iPlayer == 0)
			{
				int iCell=structure[i].iCell;
				int x, y;

				x=iCellGiveX(iCell);
				y=iCellGiveY(iCell);

				int iDist = ABS_length(iCellX, iCellY, x, y);




						if (iDist < iDistanceToBuilding)
						{
							iClosestBuilding=i;
							iDistanceToBuilding=iDist;
						}
			}

			/*
	char msg[255];
	sprintf(msg, "The closest building is %s, distance %d", structures[structure[iClosestBuilding].iType].name, iDistanceToBuilding);
	logbook(msg);*/


	//if (iClosestBuilding < 0 || iDistanceToBuilding > 2)
	//	bOutOfBorder=true;
	int iDrawX = map.mouse_draw_x();
	int iDrawY = map.mouse_draw_y();


	// Draw over it the mask for good/bad placing
	for (int iX=0; iX < width; iX++) {
		for (int iY=0; iY < height; iY++)
		{
			iTile = PLACE_ROCK;

			if ((iCellX+iX > 62) || (iCellY + iY > 62))
			{
				bOutOfBorder=true;
				bMayPlace=false;
				break;
			}

			int iCll=iCellMake((iCellX+iX), (iCellY+ iY));

			if (map.cell[iCll].passable == false)
				iTile = PLACE_BAD;

			if (map.cell[iCll].type != TERRAIN_ROCK)
				iTile = PLACE_BAD;

			if (map.cell[iCll].type == TERRAIN_SLAB)
				iTile = PLACE_GOOD;

			// occupied by units or structures
			if (map.cell[iCll].id[MAPID_STRUCTURES] > -1)
				iTile = PLACE_BAD;

			int unitIdOnMap = map.cell[iCll].id[MAPID_UNITS];
			if (unitIdOnMap > -1) {
				if (!unit[unitIdOnMap].bPickedUp) // only when not picked up, take this in (fixes carryall carrying this unit bug)
					iTile = PLACE_BAD;
			}


			// DRAWING & RULER
			if (iTile == PLACE_BAD && structureId != SLAB4)
				bMayPlace=false;


			// Count this as GOOD stuff
			if (iTile == PLACE_GOOD)
				iTotalRocks++;


			// Draw bad gfx on spot
			draw_sprite(temp, (BITMAP *)gfxdata[iTile].dat, iX*32, iY*32);
		}

		if (bOutOfBorder) {
			clear_to_color(temp, makecol(160,0,0));
		}


		set_trans_blender(0, 0, 0, 64);

		draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

		// reset to normal
		set_trans_blender(0, 0, 0, 128);

		destroy_bitmap(temp);

		// clicked mouse button
		if (game.bMousePressedLeft) {
			if (bMayPlace && bOutOfBorder == false)	{
				int iHealthPercent =  50; // the minimum is 50% (with no slabs)

				if (iTotalRocks > 0) {
					iHealthPercent += health_bar(50, iTotalRocks, iTotalBlocks);
				}

				play_sound_id(SOUND_PLACE, -1);

				//cStructureFactory::getInstance()->createStructure(iMouseCell, iStructureID, 0, iHealthPercent);

				player[HUMAN].getStructurePlacer()->placeStructure(cell, structureId, iHealthPercent);

				game.bPlaceIt=false;

				itemToPlace->decreaseTimesToBuild();
				itemToPlace->setPlaceIt(false);
				itemToPlace->setIsBuilding(false);
				itemToPlace->setProgress(0);
				if (itemToPlace->getTimesToBuild() < 1) {
					player[HUMAN].getItemBuilder()->removeItemFromList(itemToPlace);
				}
			}
		}
	}

	//iDrawX *=32;
	//iDrawY *=32;

	//rect(bmp_screen, iDrawX, iDrawY, iDrawX+(iWidth*32), iDrawY+(iHeight*32), makecol(255,255,255));
}

void cPlaceItDrawer::drawStructureIdAtCell(cBuildingListItem *itemToPlace, int cell) {
	assert(itemToPlace);
	assert(cell >= 0);

	int structureId = itemToPlace->getBuildId();
	cCellCalculator cellCalculator;
	cStructureUtils structureUtils;

	int iWidth = structureUtils.getWidthOfStructureTypeInCells(structureId);
	int iHeight = structureUtils.getHeightOfStructureTypeInCells(structureId);

	int iDrawX = map.mouse_draw_x();
	int iDrawY = map.mouse_draw_y();
	int iCellX = cellCalculator.getX(cell);
	int iCellY = cellCalculator.getY(cell);

	int drawId = -1;

	if (structureId == SLAB1) {
		draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_SLAB1].dat, iDrawX, iDrawY);
	} else if (structureId == SLAB4) {
		draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_SLAB4].dat, iDrawX, iDrawY);
	} else if (structureId == WALL) {
		draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[PLACE_WALL].dat, iDrawX, iDrawY);
	} else {
		BITMAP *temp;
		temp = create_bitmap(structures[structureId].bmp_width, structures[structureId].bmp_height);
		clear_bitmap(temp);

		blit(structures[structureId].bmp, temp, 0,0, 0,0, structures[structureId].bmp_width, structures[structureId].bmp_height);

		draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);

		destroy_bitmap(temp);
	}


}
