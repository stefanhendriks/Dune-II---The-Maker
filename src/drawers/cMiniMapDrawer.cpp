#include "../include/d2tmh.h"

cMiniMapDrawer::cMiniMapDrawer(cMap *theMap, cPlayer *thePlayer, cMapCamera * theMapCamera) {
	assert(theMap);
	assert(thePlayer);
	assert(theMapCamera);
	map = theMap;
	player = thePlayer;
	mapCamera = theMapCamera;
	iStaticFrame = STAT14;
	iStatus = -1;
	iTrans = 0;
}

cMiniMapDrawer::~cMiniMapDrawer() {
	player = NULL;
	map = NULL;
	mapCamera = NULL;
	iStaticFrame = STAT14;
	iStatus = -1;
}

void cMiniMapDrawer::drawViewPortRectangle() {
	 // Draw the magic rectangle (viewport)
	 int iWidth=((game.getScreenResolution()->getWidth()-160)/32);
	 int iHeight=((game.getScreenResolution()->getWidth()-42)/32)+1;

	 iWidth--;
	 iHeight--;
	 rect(bmp_screen, getDrawStartX()+(mapCamera->getX()*2), getDrawStartY()+(mapCamera->getY()*2), ((getDrawStartX()+(mapCamera->getX()*2))+iWidth*2)+1, (getDrawStartY()+(mapCamera->getY()*2)+iHeight*2)+1, makecol(255,255,255));
}

int cMiniMapDrawer::getDrawStartX() {
	return game.getScreenResolution()->getWidth() - 129;
}

int cMiniMapDrawer::getDrawStartY() {
	return game.getScreenResolution()->getHeight() - 129;
}

void cMiniMapDrawer::drawTerrain() {
	// startX = MAX_SCREEN_X - 129
	int iDrawX=getDrawStartX();
	int iDrawY=getDrawStartY();

	int iColor=makecol(0,0,0);

	for (int x = 0; x < (map->getWidth()); x++) {

		iDrawY = getDrawStartY(); // reset Y coordinate for drawing for each column

		for (int y = 0; y < (map->getHeight()); y++) {
			iColor = makecol(0, 0, 0);
			int iCll = iCellMake(x, y);

			if (map->isVisible(iCll, player->getId())) {
				iColor = getRGBColorForTerrainType(map->cell[iCll].type);
			}

			// TODO: make flexible map borders
			// do not show the helper border
			if (x == 0 || y == 0) {
				iColor = makecol(0, 0, 0);
			}

			if (x == 63 || y == 63) {
				iColor = makecol(0, 0, 0);
			}

			// double sized 'pixels'.
			drawDot(iDrawX + x, iDrawY + y, iColor);

			iDrawY += 1;
		}

		iDrawX += 1;
	}
}

void cMiniMapDrawer::drawUnitsAndStructures() {
	int iDrawX=getDrawStartX();
	int iDrawY=getDrawStartY();

	int iColor=makecol(0,0,0);
	cMapUtils * mapUtils = new cMapUtils(map);

	for (int x = 0; x < map->getWidth(); x++) {

		iDrawY = getDrawStartY(); // reset Y coordinate for drawing for each column

		for (int y = 0; y < map->getHeight(); y++) {
			iColor = makecol(0, 0, 0);
			int iCll = iCellMake(x, y);
			bool drawADot = false;

			if (map->isVisible(iCll, player->getId())) {
				if (map->cell[iCll].id[MAPID_STRUCTURES] > -1) {
					int	iPlr = structure[map->cell[iCll].id[MAPID_STRUCTURES]]->getOwner();
					iColor = player[iPlr].getMinimapColor();
					drawADot = true;
				}

				if (map->cell[iCll].id[MAPID_UNITS] > -1) {
					int iPlr = unit[map->cell[iCll].id[MAPID_UNITS]].iPlayer;
					iColor = player[iPlr].getMinimapColor();
					drawADot = true;
				}

				if (map->cell[iCll].id[MAPID_AIR] > -1) {
					int iPlr = unit[map->cell[iCll].id[MAPID_AIR]].iPlayer;
					iColor = player[iPlr].getMinimapColor();
					drawADot = true;
				}

				if (map->cell[iCll].id[MAPID_WORMS] > -1) {
					iColor = makecol(game.fade_select, game.fade_select, game.fade_select);
					drawADot = true;
				}
			}

			// TODO: make flexible map borders
			// do not show the helper border
			if (x == 0 || y == 0) {
				iColor = makecol(0, 0, 0);
			}

			if (x == 63 || y == 63) {
				iColor = makecol(0, 0, 0);
			}

			if (drawADot) {
				// double sized 'pixels'.
				drawDot(iDrawX + x, iDrawY + y, iColor);
			}

			iDrawY += 1;
		}

		iDrawX += 1;
	}

	delete mapUtils;
}



int cMiniMapDrawer::getRGBColorForTerrainType(int terrainType) {
	// get color for terrain type (for minimap)
	switch (terrainType) {
		case TERRAIN_ROCK:
			return makecol(80,80,60);
		case TERRAIN_SPICE:
			return makecol(186,93,32);
		case TERRAIN_SPICEHILL:
			return makecol(180,90,25);
		case TERRAIN_HILL:
			return makecol(188, 115, 50);
		case TERRAIN_MOUNTAIN:
			return makecol(48, 48, 36);
		case TERRAIN_SAND:
			return makecol(194, 125, 60);
		case TERRAIN_WALL:
			return makecol(192, 192, 192);
		case TERRAIN_SLAB:
			return makecol(80,80,80);
		case TERRAIN_BLOOM:
			return makecol(214,145,100);
		default:
			return makecol(255, 0, 255);
	}
}

void cMiniMapDrawer::interact() {
	int iWidth = mapCamera->getViewportWidth();
	int iHeight = mapCamera->getViewportHeight();

	// interact with mouse
	if (mouse_x >= getDrawStartX() && mouse_y >= getDrawStartY()) {
		// pressed the mouse and not boxing anything..
		if (MOUSE_BTN_LEFT() && mouse_co_x1 < 0 && mouse_co_y1 < 0) {

			cGameControlsContext * context = player[HUMAN].getGameControlsContext();
			int mouseCellOnMiniMap = context->getMouseCellFromMiniMap();
			mapCamera->centerAndJumpViewPortToCell(mouseCellOnMiniMap);
		}
	}
}

void cMiniMapDrawer::draw() {
	assert(player);
	assert(map);

	cPlayerUtils playerUtils;

	bool hasRadarAndEnoughPower = (playerUtils.getAmountOfStructuresForStructureTypeForPlayer(player, RADAR) > 0) && player->bEnoughPower();


	if (hasRadarAndEnoughPower) {
		drawTerrain();
		drawUnitsAndStructures();
	}

	drawViewPortRectangle();

	if (hasRadarAndEnoughPower) {
		if (iStatus < 0) {
			play_sound_id(SOUND_RADAR, -1);
			play_voice(SOUND_VOICE_03_ATR);
		}
		iStatus = 0;
	} else {
		if (iStatus > -1) {
			play_voice(SOUND_VOICE_04_ATR);
		}

		iStatus = -1;
	}

	drawStaticFrame();
}

void cMiniMapDrawer::drawStaticFrame() {
	// Draw static info
	 if (iStatus < 0) {
		 draw_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, getDrawStartX(), getDrawStartY());
	 } else {
		 if (iStaticFrame < STAT10) {
			 iTrans = 255 - health_bar(192, (STAT12-iStaticFrame), 12);
		 } else {
			 iTrans = 255;
		 }

		 if (iStaticFrame != STAT01)
		 {
			 set_trans_blender(0,0,0,iTrans);

			 draw_trans_sprite(bmp_screen, (BITMAP *)gfxinter[iStaticFrame].dat, getDrawStartX(), getDrawStartY());
			 // reset the trans blender
			 set_trans_blender(0,0,0,128);
		 }
	 }
}

void cMiniMapDrawer::drawDot(int x, int y, int color) {
	// draw a double sized 'pixel'
	putpixel(bmp_screen, x, y, color);
	putpixel(bmp_screen, x + 1, y, color);
	putpixel(bmp_screen, x + 1, y + 1, color);
	putpixel(bmp_screen, x, y + 1, color);
}
