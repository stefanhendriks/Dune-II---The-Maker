/*
 * cMapDrawer.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"


cMapDrawer::cMapDrawer(cMap * theMap, cPlayer * thePlayer, cMapCamera * theCamera) {
	assert(theMap);
	assert(thePlayer);
	assert(theCamera);
	map = theMap;
	player = thePlayer;
	camera = theCamera;
	cellCalculator = new cCellCalculator(map);
}

cMapDrawer::~cMapDrawer() {
	map = NULL;
	player = NULL;
	camera = NULL;
	delete cellCalculator;
	cellCalculator = NULL;
}

void cMapDrawer::drawShroud() {
	set_trans_blender(0,0,0,128);

	BITMAP *temp = create_bitmap(32,32);

	int iDrawX=0;
	int iDrawY=42;

	int cll=-1;
	int tile=-1;

	int iPl = player->getId();

	for (int iStartX = camera->getX(); iStartX < camera->getEndX(); iStartX++) {
		iDrawY=42;
		tile=-1;

		// new row
		for (int iStartY=camera->getY(); iStartY < camera->getEndY(); iStartY++) {
			cll = iCellMake(iStartX, iStartY);

			if (DEBUGGING) {
				if (mapUtils->isCellVisibleForPlayerId(iPl, cll)) {
					// do nothing
				} else {
					rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(0,0,0));
				}

			} else {
				if (mapUtils->isCellVisibleForPlayerId(iPl, cll)) {
					// Visible stuff, now check for not visible stuff. When found, assign the proper border
					// of shroud to it.
					int above = CELL_ABOVE(cll);
					int under = CELL_UNDER(cll);
					int left  = CELL_LEFT(cll);
					int right = CELL_RIGHT(cll);

					bool a, u, l, r;
					a=u=l=r=true;

					if (above > -1)	{
						if (mapUtils->isCellVisibleForPlayerId(iPl, above)) {
							a = false;  // visible
						}
					} else {
						a = false;
					}

					if (under > -1) {
						if (mapUtils->isCellVisibleForPlayerId(iPl, under)) {
							u = false;  // visible
						}
					} else {
						u = false;
					}

					if (left > -1) {
						if (mapUtils->isCellVisibleForPlayerId(iPl, left)) {
							l = false;  // visible
						}
					} else {
						l = false;
					}

					if (right > -1) {
						if (mapUtils->isCellVisibleForPlayerId(iPl, right)) {
							r = false;  // visible
						}
					} else {
						r = false;
					}

					int t=-1;	// tile id to draw... (x axis)

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
						t=1;

					tile = t - 1;

					if (tile > -1)
					{

						// Draw cell
						masked_blit((BITMAP *)gfxdata[SHROUD].dat, bmp_screen, tile * 32, 0, iDrawX, iDrawY, 32, 32);

						 clear_to_color(temp, makecol(255,0,255));

							masked_blit((BITMAP *)gfxdata[SHROUD_SHADOW].dat, temp, tile * 32, 0, 0, 0, 32, 32);
							draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);


					//	alfont_textprintf(bmp_screen, game_font, iDrawX,iDrawY, makecol(255,255,255), "%d", tile);
				   }

				  }
				  else
				  {
					  // NOT VISIBLE, DO NOT DRAW A THING THEN!
					//	alfont_textprintf(bmp_screen, game_font, iDrawX,iDrawY, makecol(255,255,255), "%d", tile);

					  // Except when there is a building here, that should not be visible ;)
					  // if (map.cell[cll].id[1] > -1 || map.cell[cll].id[0] > -1 || map.cell[cll].id[2] > -1 || map.cell[cll].id[3] > -1)
						  masked_blit((BITMAP *)gfxdata[SHROUD].dat, bmp_screen, 0, 0, iDrawX, iDrawY, 32, 32);
				  }

			  }
			iDrawY+=32;
		}
		iDrawX+=32;
	}

	destroy_bitmap(temp);
}

void cMapDrawer::drawTerrain() {
	// draw only what is visible

	// Scrolling:
	// - is X,Y position of map, not smoothed!
	// - all we need is the end of the 'to be drawn area' , which is:
	//   WIDTH OF SCREEN / 32

	int iDrawX=0;
	int iDrawY=42;

	int iCell=-1;

	int iPl = player->getId();

//	char msg[255];
//	sprintf(msg, "Drawing from %d, %d to %d, %d", camera->getX(), camera->getY(), camera->getEndX(), camera->getEndY());
//	logbook(msg);

	// draw vertical rows..
	for (int iStartX = camera->getX(); iStartX < camera->getEndX(); iStartX++) {
		iDrawY=42;

		// new row
		for (int iStartY=camera->getY(); iStartY < camera->getEndY(); iStartY++) {
			iCell = iCellMake(iStartX, iStartY);

			if (mapUtils->isCellVisibleForPlayerId(iPl, iCell) == false) {
				iDrawY+=TILESIZE_HEIGHT_PIXELS;
				continue; // do not draw this one
			}

			blit((BITMAP *)gfxdata[map->cell[iCell].type].dat, bmp_screen, map->cell[iCell].tile * 32, 0, iDrawX, iDrawY, 32, 32);

			// draw smudge if nescesary
			if (map->cell[iCell].smudgetype > -1 && map->cell[iCell].smudgetile > -1) {
				masked_blit((BITMAP *)gfxdata[SMUDGE].dat, bmp_screen,
						map->cell[iCell].smudgetile * 32, map->cell[iCell].smudgetype * 32, iDrawX, iDrawY, 32, 32);

			}

			if (DEBUGGING)
			{
				if (player->getGameControlsContext()->getMouseCell() > -1)
				{
					int mc = player->getGameControlsContext()->getMouseCell();
					if (cellCalculator->getX(mc) == iStartX && cellCalculator->getY(mc) == iStartY)
						rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(64,64,64));

				}

				rect(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(128,128,128));
			}

			if (key[KEY_D] && key[KEY_TAB])
			{
				int iClr=makecol(255,0,0);

				bool bDraw=false;

				if (map->cell[iCell].passable == false)
					bDraw=true;

				if (map->cell[iCell].id[MAPID_STRUCTURES] > -1)
				{
					iClr=makecol(0,255,0);
					bDraw=true;
				}

				if (map->cell[iCell].id[MAPID_UNITS] > -1)
				{
					iClr=makecol(0,0,255);
					bDraw=true;
				}

				if (bDraw)
				{
					rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, iClr);
				}

			}

			iDrawY+=TILESIZE_HEIGHT_PIXELS;
		}
		iDrawX+=TILESIZE_WIDTH_PIXELS;
	}
}
