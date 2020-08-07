/*
 * cMapDrawer.cpp
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"


cMapDrawer::cMapDrawer(cMap * theMap, const cPlayer& thePlayer, cMapCamera * theCamera) : player(thePlayer) {
	assert(theMap);
	assert(theCamera);
	map = theMap;
	camera = theCamera;
	cellCalculator = new cCellCalculator(map);
	bmp_temp = nullptr;
}

cMapDrawer::~cMapDrawer() {
	map = NULL;
	camera = NULL;
	delete cellCalculator;
	cellCalculator = NULL;
	if (bmp_temp) {
	    destroy_bitmap(bmp_temp);
	}
}

void cMapDrawer::drawShroud() {
	set_trans_blender(0,0,0,128);

    int tileWidth = mapCamera->getTileWidth();
    int tileHeight = mapCamera->getTileHeight();

    int colorDepthScreen = bitmap_color_depth(bmp_screen);
//    bmp_temp=create_bitmap_ex(colorDepthScreen, 32,32);
    BITMAP *temp = create_bitmap_ex(colorDepthScreen, tileWidth,tileHeight);

	int iDrawX=0;
	int iDrawY=42;

	int cll=-1;
	int tile=-1;

	int iPl = player.getId();

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
					rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+tileWidth, iDrawY+tileHeight, makecol(0,0,0));
				}
			} else {
				if (mapUtils->isCellVisibleForPlayerId(iPl, cll)) {
                    tile = determineWhichShroudTileToDraw(cll, iPl);

                    if (tile > -1) {
						masked_stretch_blit((BITMAP *)gfxdata[SHROUD].dat, bmp_screen, tile * 32, 0, 32, 32, iDrawX, iDrawY, tileWidth, tileHeight);
                        clear_to_color(temp, makecol(255, 0, 255));

                        masked_stretch_blit((BITMAP *) gfxdata[SHROUD_SHADOW].dat, temp, tile * 32, 0, 32, 32, 0, 0, tileWidth, tileHeight);
                        draw_trans_sprite(bmp_screen, temp, iDrawX, iDrawY);
                    }
				  }
				  else
				  {
                      // NOT VISIBLE, DO NOT DRAW A THING THEN!
                      // Except when there is a building here, that should not be visible ;)
                      // tile 0 of shroud is entirely black... (effectively the same as drawing a rect here)
                      masked_stretch_blit((BITMAP *)gfxdata[SHROUD].dat, bmp_screen, 0, 0, 32, 32, iDrawX, iDrawY, tileWidth, tileHeight);
				  }
			  }
			iDrawY+=tileHeight;
		}
		iDrawX+=tileWidth;
	}

	destroy_bitmap(temp);
}

int cMapDrawer::determineWhichShroudTileToDraw(int cll, int iPl) const {
    int tile;// Visible stuff, now check for not visible stuff. When found, assign the proper border
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

    int t=-1;    // tile id to draw... (x axis)

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
    return tile;
}

void cMapDrawer::drawTerrain() {
    if (bmp_temp == nullptr) {
        int colorDepthScreen = bitmap_color_depth(bmp_screen);
        bmp_temp=create_bitmap_ex(colorDepthScreen, 32,32);
    }

    int tileWidth = mapCamera->getTileWidth();
    int tileHeight = mapCamera->getTileHeight();

	// draw only what is visible

	// Scrolling:
	// - is X,Y position of map, not smoothed!
	// - all we need is the end of the 'to be drawn area' , which is:
	//   WIDTH OF SCREEN / 32

	int iDrawX=0;
	int iDrawY=42;

	int iCell=-1;

	int iPl = player.getId();


//	char msg[255];
//	sprintf(msg, "Drawing from %d, %d to %d, %d", camera->getX(), camera->getY(), camera->getEndX(), camera->getEndY());
//	logbook(msg);

	// draw vertical rows..
	for (int iStartX = camera->getX(); iStartX < camera->getEndX(); iStartX++) {
		iDrawY=42;

		// new row
		for (int iStartY=camera->getY(); iStartY < camera->getEndY(); iStartY++) {
			iCell = iCellMake(iStartX, iStartY);

			// not visible for player, so do not draw
			if (!mapUtils->isCellVisibleForPlayerId(iPl, iCell)) {
				iDrawY+=tileHeight;
				continue;
			}

			// Draw terrain
			blit((BITMAP *)gfxdata[map->cell[iCell].type].dat,
                 bmp_temp,
			        map->cell[iCell].tile * 32, 0,
			        0, 0,
			        32, 32
                );

            stretch_blit(bmp_temp, bmp_screen, 0, 0, 32, 32, iDrawX, iDrawY, tileWidth, tileHeight);

			// draw Smudge if necessary
			if (map->cell[iCell].smudgetype > -1 && map->cell[iCell].smudgetile > -1) {
                masked_blit((BITMAP *)gfxdata[SMUDGE].dat, bmp_temp,
						map->cell[iCell].smudgetile * 32,
						map->cell[iCell].smudgetype * 32,
						0,
						0,
						32,
						32);

                stretch_blit(bmp_temp, bmp_screen, 0, 0, 32, 32, iDrawX, iDrawY, tileWidth, tileHeight);

                //				masked_blit((BITMAP *)gfxdata[SMUDGE].dat, bmp_screen,
//						map->cell[iCell].smudgetile * 32,
//						map->cell[iCell].smudgetype * 32,
//						iDrawX,
//						iDrawY,
//						32,

            }

			// Draw debugging information
			if (DEBUGGING)
			{
				if (player.getGameControlsContext()->getMouseCell() > -1)
				{
					int mc = player.getGameControlsContext()->getMouseCell();
					if (cellCalculator->getX(mc) == iStartX && cellCalculator->getY(mc) == iStartY)
						rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(64,64,64));

				}

				rect(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(128,128,128));
			}

			// Draw more debugging information
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

			// increase height
			iDrawY+=tileHeight;
		}
		iDrawX+=tileWidth;
	}
}
