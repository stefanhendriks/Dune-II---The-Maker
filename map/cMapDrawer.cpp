/*
 * cMapDrawer.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cMapDrawer::cMapDrawer(cMap * theMap, cPlayer * thePlayer, cMapCamera * theCamera) {
	assert(theMap);
	assert(thePlayer);
	assert(theCamera);
	map = theMap;
	player = thePlayer;
	camera = theCamera;
}

cMapDrawer::~cMapDrawer() {
	map = NULL;
	player = NULL;
	camera = NULL;
}

// draw function, called by the drawManager
void cMapDrawer::draw() {
	drawTerrain();

	map->draw();

	// Only draw units/structures, etc, when we do NOT press D
	if (!key[KEY_D] || !key[KEY_TAB])
	{
		map->draw_structures(0);
	}

	// draw layer 1 (beneath units, on top of terrain
	map->draw_particles(1);

	map->draw_units();

	map->draw_bullets();

	map->draw_structures(2); // draw layer 2
	map->draw_structures_health();
	map->draw_units_2nd();

	map->draw_particles(0);

	map->draw_shroud();

	map->draw_minimap();
}

void cMapDrawer::drawTerrain() {
	// draw only what is visible

	// Scrolling:
	// - is X,Y position of map, not smoothed!
	// - all we need is the end of the 'to be drawn area' , which is:
	//   WIDTH OF SCREEN / 32

	cCellCalculator cellCalculator;

	int iDrawX=0;
	int iDrawY=42;

	int iCell=-1;

	int iPl = player->getId();

	char msg[255];
	sprintf(msg, "Drawing from %d, %d to %d, %d", camera->getX(), camera->getY(), camera->getEndX(), camera->getEndY());
	logbook(msg);

	// draw vertical rows..
	for (int iStartX = camera->getX(); iStartX < camera->getEndX(); iStartX++) {
		iDrawY=42;

		// new row
		for (int iStartY=camera->getY(); iStartY < camera->getEndY(); iStartY++) {
			iCell = iCellMake(iStartX, iStartY);

			if (map->iVisible[iCell][iPl] == false) {
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
				if (map->mouse_cell() > -1)
				{
					int mc = map->mouse_cell();
					if (cellCalculator.getX(mc) == iStartX && cellCalculator.getY(mc) == iStartY)
						rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(64,64,64));

				}

				rect(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(128,128,128));
			}

			if (game.selected_structure > -1)
			{
				// show draw a target on this cell so we know this is the rally point.
				if (structure[game.selected_structure]->getRallyPoint() > -1)
					if (structure[game.selected_structure]->getRallyPoint() == iCell)
					{
						// draw this thing ...
						set_trans_blender(0,0,0,128);
						draw_trans_sprite(bmp_screen, (BITMAP *)gfxdata[MOUSE_MOVE].dat, iDrawX, iDrawY);
					}
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
