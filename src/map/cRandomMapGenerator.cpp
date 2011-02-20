/*
 * cRandomMapGenerator.cpp
 *
 *  Created on: 16 nov. 2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cRandomMapGenerator::cRandomMapGenerator() {
}

cRandomMapGenerator::~cRandomMapGenerator() {

}

void cRandomMapGenerator::generateRandomMap() {
	// create random map
	map.init();

  // <> is MAP_WIDTH+MAP_HEIGHT / <a number>

  int a_spice=rnd((game.iSkirmishStartPoints * 2))+(game.iSkirmishStartPoints * 2);
  int a_rock=6+rnd(game.iSkirmishStartPoints);
  int a_hill=2+rnd(8);



  // rock terrain is placed not near centre, also, we want 4 plateaus be
  // placed not too near to each other
  int iSpotRock[6]; // first 4
  memset(iSpotRock, -1, sizeof(iSpotRock));

  int iSpot=0;
  int iFails=0;

  memset(PreviewMap[0].iStartCell, -1, sizeof(PreviewMap[0].iStartCell));

  int iDistance=16;

  if (game.iSkirmishStartPoints == 2)  iDistance=32;
  if (game.iSkirmishStartPoints == 3)  iDistance=26;
  if (game.iSkirmishStartPoints == 4)  iDistance=22;

  int iProgress=0;

  // x = 160
  // y = 180
  draw_sprite(bmp_screen, (BITMAP *)gfxinter[BMP_GENERATING].dat, 160, 180);

  cCellCalculator * cellCalculator = map.getCellCalculator();

  // draw

	while (a_rock > 0)
	{
	  int iCll = cellCalculator->getCellWithMapBorders(4+rnd(game.map_width-8), 4+rnd(game.map_height-8));

	  bool bOk=true;
	  if (iSpot < 6)
	  {
		  for (int s=0; s < 6; s++)
		  {
			  if (iSpotRock[s] > -1)
			  {
				  if (ABS_length(iCellGiveX(iCll), iCellGiveY(iCll), iCellGiveX(iSpotRock[s]), iCellGiveY(iSpotRock[s])) < iDistance) {
					  bOk=false;
				  } else {
					  iFails++;
				  }
			  }
		  }
	  }

	  if (iFails > 10)
	  {
		  iFails=0;
		  bOk=true;
	  }

	  if (bOk) {
		  iProgress+=5;
		  mapEditor.createField(iCll, TERRAIN_ROCK, 5500+rnd(1500));

		  if (iSpot < game.iSkirmishStartPoints)
		  {
			  PreviewMap[0].iStartCell[iSpot] = iCll;
		  }
		  else
		  {
			  mapEditor.createField(iCll, TERRAIN_MOUNTAIN, 5+rnd(15));
		  }

		  if (iSpot < 6) {
			  iSpotRock[iSpot]=iCll;
		  }

		  iSpot++;
		  a_rock--;

		  // blit on screen
		  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
		  blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

	  }

	  // take screenshot
	  if (key[KEY_F11])
	  {
		  char filename[25];
		  sprintf(filename, "%dx%d_%d.bmp", game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight(), game.screenshot);
		  save_bmp(filename, bmp_screen, general_palette);
		  game.screenshot++;
	  }

	}

	// soft out rock a bit
	mapEditor.removeSingleRockSpots();

	// blit on screen
	rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

	mapEditor.removeSingleRockSpots();


	// blit on screen
	rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

	mapEditor.removeSingleRockSpots();

	// blit on screen
	rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

	while (a_spice > 0)
	{
	  int iCll=iCellMake(rnd(game.map_width), rnd(game.map_height));
	  mapEditor.createField(iCll, TERRAIN_SPICE, 2500);
	  a_spice--;
	  // blit on screen
	  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	  blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

	}

	while (a_hill > 0)
	{
	  int cell = iCellMake(rnd(game.map_width), rnd(game.map_height));
	  mapEditor.createField(cell, TERRAIN_HILL, 500+rnd(500));
	  a_hill--;
	  iProgress+=10;
	  // blit on screen
	  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	  blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
	}

	// end of map creation

	mapEditor.smoothMap();

	// blit on screen
	iProgress+=25;
	rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());

	clear_to_color(PreviewMap[0].terrain, makecol(0,0,0));

	// now put in previewmap 0
	for (int x=0; x < 64; x++)
	  for (int y=0; y < 64; y++)
	  {

		if (iProgress < 210 && rnd(100) < 3)
		{
			rectfill(screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
		}

		int cll=iCellMake(x,y);
		int iColor=makecol(194, 125, 60);

		// rock
		if (map.cell[cll].type == TERRAIN_ROCK) iColor = makecol(80,80,60);
		if (map.cell[cll].type == TERRAIN_MOUNTAIN) iColor = makecol(48, 48, 36);
		if (map.cell[cll].type == TERRAIN_SPICEHILL) iColor = makecol(180,90,25); // bit darker
		if (map.cell[cll].type == TERRAIN_SPICE) iColor = makecol(186,93,32);
		if (map.cell[cll].type == TERRAIN_HILL) iColor = makecol(188, 115, 50);

		PreviewMap[0].mapdata[cll] = map.cell[cll].type;

		for (int s=0; s < 4; s++)
		{
			if (PreviewMap[0].iStartCell[s] > -1)
			{
			int sx = iCellGiveX(PreviewMap[0].iStartCell[s]);
			int sy = iCellGiveY(PreviewMap[0].iStartCell[s]);

			if (sx == x && sy == y)
				iColor = makecol(255,255,255);
			}
		}

		putpixel(PreviewMap[0].terrain, (x*2), (y*2), iColor);
		putpixel(PreviewMap[0].terrain, (x*2)+1, (y*2), iColor);
		putpixel(PreviewMap[0].terrain, (x*2)+1, (y*2)+1, iColor);
		putpixel(PreviewMap[0].terrain, (x*2), (y*2)+1, iColor);
	  }

	// blit on screen
	iProgress=211;
	rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	blit(bmp_screen, screen, 0, 0, 0, 0, game.getScreenResolution()->getWidth(), game.getScreenResolution()->getHeight());
}
