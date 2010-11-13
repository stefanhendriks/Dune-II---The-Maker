/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"

#include <math.h>

#define WATCH_PLAYER 0

cMap::cMap() {
	TIMER_scroll=0;
	iScrollSpeed=10;
	cellCalculator = NULL;
}

void cMap::resetCellCalculator() {
	if (cellCalculator) {
		delete cellCalculator;
	}
	cellCalculator = new cCellCalculator(this);
}

void cMap::init()
{
    INIT_REINFORCEMENT();
    resetCellCalculator();

    // clear out all cells
    cMapUtils * mapUtils = new cMapUtils(this);
    mapUtils->clearAllCells();

    cStructureFactory::getInstance()->clearAllStructures();

	for (int i=0; i < MAX_BULLETS; i++) {
		bullet[i].init();
	}

	for (int i=0; i < MAX_PARTICLES; i++) {
        particle[i].init();
	}

	for (int i=0; i < MAX_UNITS; i++) {
		unit[i].init(i);
	}

	TIMER_scroll=0;
	iScrollSpeed=10;

	delete mapUtils;
}

void cMap::smudge_increase(int iType, int iCell) {
    if (cell[iCell].smudgetype < 0)
        cell[iCell].smudgetype = iType;

    if (cell[iCell].smudgetype == SMUDGE_WALL)
        cell[iCell].smudgetile = 0;

    if (cell[iCell].smudgetype == SMUDGE_ROCK)
    {
        if (cell[iCell].smudgetile < 0)
            cell[iCell].smudgetile = rnd(2);
        else if (cell[iCell].smudgetile + 2 < 6)
            cell[iCell].smudgetile += 2;
    }

    if (cell[iCell].smudgetype == SMUDGE_SAND)
    {
        if (cell[iCell].smudgetile < 0)
            cell[iCell].smudgetile = rnd(2);
        else if (cell[iCell].smudgetile + 2 < 6)
            cell[iCell].smudgetile += 2;
    }
}

/**
 * Is this cell occupied? This returns true when type of terrain is WALL or MOUNTAIN.
 *
 * @param iCell
 * @return
 */
bool cMap::occupiedByType(int iCell) {
	assert(iCell > -1);
	assert(iCell < MAX_CELLS);

	if (map.cell[iCell].type == TERRAIN_WALL) return true;
	if (map.cell[iCell].type == TERRAIN_MOUNTAIN) return true;

	return false;
}

bool cMap::occupiedInDimension(int iCll, int dimension) {
	assert(iCll > -1);
	assert(iCll < MAX_CELLS);
	assert(dimension > -1);
	assert(dimension < MAPID_MAX);
	return map.cell[iCll].id[dimension] > -1;
}

/**
 * Is the specific cell occupied by any dimension?
 *
 * @param iCll
 * @return
 */
bool cMap::occupied(int iCell) {
	assert(iCell > -1);
	assert(iCell < MAX_CELLS);

    if (occupiedInDimension(iCell, MAPID_UNITS)) return true;
    if (occupiedInDimension(iCell, MAPID_AIR)) return true;
    if (occupiedInDimension(iCell, MAPID_STRUCTURES)) return true;
    if (occupiedByType(iCell)) return true;

	return false;
}

bool cMap::occupied(int iCll, int iUnitID)
{
    bool bResult=false;

    if (iCll < 0 || iUnitID < 0)
        return true;

    if (map.cell[iCll].id[MAPID_UNITS] > -1 &&
		map.cell[iCll].id[MAPID_UNITS] != iUnitID)
        bResult=true;

    // TODO: when unit wants to enter a structure...

    if (map.cell[iCll].id[MAPID_STRUCTURES] > -1 )
	{
		// we are on top of a structure we do NOT want to enter...
		if (unit[iUnitID].iStructureID > -1)
		{
			if (map.cell[iCll].id[MAPID_STRUCTURES] != unit[iUnitID].iStructureID)
			bResult=true;
		}
		else
			bResult=true;
	}

    // walls block as do mountains
    if (map.cell[iCll].type == TERRAIN_WALL)
        bResult=true;

    // mountains only block infantry
    if (units[unit[iUnitID].iType].infantry == false)
        if (map.cell[iCll].type == TERRAIN_MOUNTAIN)
           bResult=true;



    return bResult;
}

// HUH? WHat is this doing here? (TODO: move to structure Drawing thing)
void cMap::draw_structures_health()
{
	cGameControlsContext *context = player[HUMAN].getGameControlsContext();
    // DRAW HEALTH
    if (context->isMouseOverStructure())
    {
        int i = context->getIdOfStructureWhereMouseHovers();

        // Draw structure health
        int draw_x = structure[i]->iDrawX()-1;
        int draw_y = structure[i]->iDrawY()-5;
        int width_x = structures[structure[i]->getType()].bmp_width-1;
        int height_y = 4;

        if (draw_y < 30) draw_y = 30;

        int w = health_structure(i, structures[structure[i]->getType()].bmp_width);

        int step = (255/structures[structure[i]->getType()].bmp_width);
        int r = 255-(w*step);
        int g = w*step;

        if (g > 255)
            g = 255;
        if (r < 0) r= 0;


        // shadow
       // rectfill(bmp_screen, draw_x+2, draw_y+2, draw_x + width_x + 2, draw_y + height_y + 2, makecol(0,0,0));

        // bar itself
        rectfill(bmp_screen, draw_x, draw_y, draw_x + width_x+1, draw_y + height_y+1, makecol(0,0,0));
        rectfill(bmp_screen, draw_x, draw_y, draw_x + (w-1), draw_y + height_y, makecol(r,g,32));

        // bar around it
        rect(bmp_screen, draw_x, draw_y, draw_x + width_x, draw_y + height_y, makecol(255, 255, 255));

    }

}

// do the static info thinking
void cMap::think_minimap() {
	// Draw static info
	cMiniMapDrawer * miniMapDrawer = drawManager->getMiniMapDrawer();
	int iStatus = miniMapDrawer->getStatus();
	int currentStaticFrame = miniMapDrawer->getStaticFrame();
	if (iStatus < 0) {
		miniMapDrawer->drawStaticFrame();

		if (currentStaticFrame < STAT21) {
			currentStaticFrame++;
		}
	} else {
		// transparancy is calculated actulaly
		if (currentStaticFrame > STAT01) {
			currentStaticFrame--;
		}
	}
	miniMapDrawer->setStaticFrame(currentStaticFrame);
}

// TODO: move this to a bulletDrawer (remove here!)
void cMap::draw_bullets()
{

    // Loop through all units, check if they should be drawn, and if so, draw them
  for (int i=0; i < MAX_BULLETS; i++)
  {
    if (bullet[i].bAlive)
    {
      if (bullet[i].draw_x() > -32 &&
          bullet[i].draw_x() < game.screen_x &&
          bullet[i].draw_y() > -32 &&
          bullet[i].draw_y() < game.screen_y)
          bullet[i].draw();
    }
  }
}

void cMap::clear_all()
{
    for (int c=0; c < MAX_CELLS; c++)
        iVisible[c][0] = true;
}

void cMap::clear_spot(int c, int size, int player)
{
  // Get the x and y and make a circle around it of 16xR, then calculate of every step the cell and
  // clear it

  int cx = iCellGiveX(c); /*cell[c].x(c);*/
  int cy = iCellGiveY(c); /*cell[c].y(c);*/

  // fail
  if (cx < 0 || cy < 0)
      return;

  map.iVisible[c][player]=true;

#define TILE_SIZE_PIXELS 32

  // go around 360 fDegrees and calculate new stuff
  for (int dr=1; dr < size; dr++)
  {
    for (double d=0; d < 360; d++)
    {
      int x = cx, y = cy;

      // when scrolling, compensate
      x -= mapCamera->getX();
      y -= mapCamera->getY();

      // convert to pixels (*32)
      x *= TILE_SIZE_PIXELS;
      y *= TILE_SIZE_PIXELS;

      y += 42;

	  // center on unit
      x+= 16;
      y+= 16;


      int dx, dy;
      dx = x;
      dy = y;

       x = (x + (cos(d)*((dr*32))));
       y = (y + (sin(d)*((dr*32))));

     // DEBUG

      //line (bmp_screen, dx, dy, x, y, makecol(255,255,255));
      //if (DEBUGGING)
		//  putpixel(bmp_screen, x, y, makecol(255,255,0));

      //putpixel(bmp_screen, dx, dy, makecol(0,255,0));
     // DEBUG

      // convert back

       int cell_x, cell_y;

       cell_y  = y;
	   cell_y -= 42;
	   cell_y += (mapCamera->getY()*32);
	   cell_y = cell_y/32;

	   cell_x  = x;
	   cell_x += (mapCamera->getX()*32);
       cell_x  = cell_x/32;


	   /*
	   if (DEBUGGING)
	   {

	   int iDrawX = (cell_x-scroll_x)*32;
	   int iDrawY = (((cell_y-scroll_y)*32)+42);

	   rectfill(bmp_screen, iDrawX, iDrawY, iDrawX+32, iDrawY+32, makecol(0,32,0));

	   }*/


       FIX_POS(cell_x, cell_y);



	   //draw the cells



	   /*
	   if (game.state == GAME_PLAYING)
	   {
	   char msg[255];
	   sprintf(msg, "X %d, Y %d -> CX %d, CY = %d", x, y, cell_x, cell_y);
	   logbook(msg);
	   }*/

       int cl = iCellMake(cell_x, cell_y);



       if (iVisible[cl][player] == false)
       {

       iVisible[cl][player] = true;     // make visible
           // human unit detected enemy, now be scared and play some neat music
       if (player == 0)
       {
            if (cell[cl].id[MAPID_UNITS] > -1)
            {
                int id = cell[cl].id[MAPID_UNITS];

                if (unit[id].iPlayer != 0) // NOT friend
                {
                    // when state of music is not attacking, do attacking stuff and say "Warning enemy unit approaching

                    if (game.iMusicType == MUSIC_PEACE)
                    {
                        play_music(MUSIC_ATTACK);

                        // warning... bla bla
                        if (unit[id].iType == SANDWORM)
                            play_voice(SOUND_VOICE_10_ATR);  // omg a sandworm, RUN!
                        else
                            play_voice(SOUND_VOICE_09_ATR);  // enemy unit
                    }

                }
            }
       }

       } // make visible



    }
  }


}

// Each index is a mapdata field holding indexes of the map layout
//
void cMap::remove_id(int iIndex, int iIDType)
{
    // Search through the entire map and remove it
    // Using X,Y stuff, since we do not want to have unnescesary slow-downs
    // when using small maps... and going through a 512x512 map

	/*

	int x, y, cll;
    x = y = cll = 0;



    for (x=0; x < game.map_width; x++)
        for (y=0; y < game.map_height; y++)
        {
            cll = iCellMake(x,y);
            if (cell[cll].id[iIDType] == iIndex)
			{
                cell[cll].id[iIDType] = -1; // remove it
			}
        }*/

	for (int iCell=0; iCell < MAX_CELLS; iCell++)
		if (cell[iCell].id[iIDType] == iIndex)
		{
			// remove
			cell[iCell].id[iIDType] = -1;
		}



}


void cMap::randommap()
{
    // create random map
    init();

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

  // draw

	  while (a_rock > 0)
	  {
		  int iCll=iCellMake(4+rnd(game.map_width-8), 4+rnd(game.map_height-8));

		  bool bOk=true;
		  if (iSpot < 6)
		  {
			  for (int s=0; s < 6; s++)
			  {
				  if (iSpotRock[s] > -1)
				  {
					  if (ABS_length(iCellGiveX(iCll), iCellGiveY(iCll), iCellGiveX(iSpotRock[s]), iCellGiveY(iSpotRock[s])) < iDistance)
						  bOk=false;
					  else
						  iFails++;
				  }
			  }
		  }

		  if (iFails > 10)
		  {
			  iFails=0;
			  bOk=true;
		  }

		  if (bOk)
		  {
			  iProgress+=5;
			  create_field(TERRAIN_ROCK, iCll, 5500+rnd(1500));

			  if (iSpot < game.iSkirmishStartPoints)
			  {
				  PreviewMap[0].iStartCell[iSpot] = iCll;
			  }
			  else
			  {
				  create_field(TERRAIN_MOUNTAIN, iCll, 5+rnd(15));
			  }

			  if (iSpot < 6)
				  iSpotRock[iSpot]=iCll;

			  iSpot++;
			  a_rock--;

			  // blit on screen
			  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
			  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

		  }



		  // take screenshot
		  if (key[KEY_F11])
		  {
			  char filename[25];
			  sprintf(filename, "%dx%d_%d.bmp", game.screen_x, game.screen_y, game.screenshot);
			  save_bmp(filename, bmp_screen, general_palette);
			  game.screenshot++;
		  }

	  }

	  // soft out rock a bit
  soft();

  // blit on screen
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  soft();

  // blit on screen
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  soft();

  // blit on screen
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  while (a_spice > 0)
  {
	  int iCll=iCellMake(rnd(game.map_width), rnd(game.map_height));
      create_field(TERRAIN_SPICE, iCll, 2500);
      a_spice--;
  	  // blit on screen
	  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

  }

  while (a_hill > 0)
  {
      create_field(TERRAIN_HILL, iCellMake(rnd(game.map_width), rnd(game.map_height)), 500+rnd(500));
      a_hill--;
	  iProgress+=10;
	  // blit on screen
	  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
	  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);
  }


  // end of map creation

  smooth();

  // blit on screen
  iProgress+=25;
  rectfill(bmp_screen, 216, 225, 216+iProgress, 257, makecol(255,0,0));
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);

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
		if (cell[cll].type == TERRAIN_ROCK) iColor = makecol(80,80,60);
		if (cell[cll].type == TERRAIN_MOUNTAIN) iColor = makecol(48, 48, 36);
		if (cell[cll].type == TERRAIN_SPICEHILL) iColor = makecol(180,90,25); // bit darker
		if (cell[cll].type == TERRAIN_SPICE) iColor = makecol(186,93,32);
		if (cell[cll].type == TERRAIN_HILL) iColor = makecol(188, 115, 50);

        PreviewMap[0].mapdata[cll] = cell[cll].type;

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
  blit(bmp_screen, screen, 0, 0, 0, 0, game.screen_x, game.screen_y);
}


void cMap::create_field(int type, int cll, int size)
{

  int x = iCellGiveX(cll);
  int y = iCellGiveY(cll);

  if (x < 0)
    x = rnd(game.map_width);

  if (y < 0)
    y = rnd(game.map_height);

  if (type == TERRAIN_ROCK && size < 0)
    size=150;

  if (type == TERRAIN_MOUNTAIN && size < 0)
    size=25;  // very small mountainious spots

  int iDist=0;

  int iOrgX=x;
  int iOrgY=y;

  for (int i = 0; i < size; i++)
  {
    int c = iCellMake(x, y);
	iDist = ABS_length(x, y, iOrgX, iOrgY);

	if (iDist > rnd(4)+4)
		if (rnd(100) < 5)
			create_field(type, c, 100);

    if (c > -1)
    {
      // if NOT a rock tile, then place spice on it (if we are placing spice)
      if (type == TERRAIN_SPICE)
      {
          if ((cell[c].type != TERRAIN_ROCK) && (cell[c].type != TERRAIN_SLAB) && (cell[c].type != TERRAIN_MOUNTAIN) &&
              (cell[c].type != TERRAIN_WALL))
              create_spot(c, TERRAIN_SPICE, 0);
      }
      // create hill
      else if (type == TERRAIN_HILL)
      {
        if (cell[c].type == TERRAIN_SAND)
        {
          create_spot(c, TERRAIN_HILL, 0);
        }
        else if (cell[c].type == TERRAIN_SPICE)
        {
          create_spot(c, TERRAIN_SPICE, 0);
        }

      }
      else
      {
        create_spot(c, type, 0);

        if (type == TERRAIN_MOUNTAIN)
        {
            cell[c].passable=false;
        }
        else
            cell[c].passable=true;

      }
    }

	if (rnd(100) < 25)
	{
		x = iCellGiveX(cll);
		y = iCellGiveY(cll);
	}


    switch(rnd(4))
    {
    case 0: x++; break;
    case 1: y++; break;
    case 2: x--; break;
    case 3: y--; break;
    }

    FIX_POS(x, y);
  }

  smooth();

  /*

  // find any gaps with this type and fill it up
  for (int cx = 0; cx < MAP_W_MAX; cx++)
    for (int cy = 0; cy < MAP_H_MAX; cy++)
    {
      int c = iCellMake(cx,cy);

      if (c > -1)
        if (cell[c].type == TERRAIN_SAND)
        {

         int above = CELL_ABOVE(c);
         int under = CELL_UNDER(c);
         int left  = CELL_LEFT(c);
         int right = CELL_RIGHT(c);

         int a = 0; // amount 'near', 2 means 50/50 (random fill), > 2 means fill < 2 means no fill
         if (above > -1)
             if (cell[above].type == type) a++;

         if (under > -1)
             if (cell[under].type == type) a++;

         if (left > -1)
             if (cell[left].type == type) a++;

         if (right > -1)
             if (cell[right].type == type) a++;

         //
         if (a > 2)
         {
           cell[c].type = type;
           cell[c].tile = 0;
           if (cell[c].type != TERRAIN_MOUNTAIN)
               cell[c].passable=true;
           else
               cell[c].passable=false;
         }
         else if (a == 2)
           if (rnd(100) < 50)
           {
               cell[c].type = type;
               cell[c].tile = 0;
               if (cell[c].type != TERRAIN_MOUNTAIN)
                   cell[c].passable=true;
               else
                   cell[c].passable=false;

           }

        }
    }
  */

}
void cMap::draw_units()
{
    set_trans_blender(0, 0, 0, 160);

    // draw all worms first
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
			// DEBUG MODE: DRAW PATHS
			if (DEBUGGING)
				unit[i].draw_path();

            if (unit[i].iType == SANDWORM)
            {

            int drawx = unit[i].draw_x();
            int drawy = unit[i].draw_y();

            if (((drawx+units[unit[i].iType].bmp_width) > 0 && drawx < (game.screen_x-160)) &&
                ((drawy+units[unit[i].iType].bmp_height) > 42 && drawy < game.screen_y))
            {
                // draw
                unit[i].draw();
            }
            }

        }

    }

    // draw all units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {

            if (unit[i].iType == CARRYALL ||
                unit[i].iType == ORNITHOPTER ||
                unit[i].iType == FRIGATE ||
                unit[i].iType == SANDWORM ||
                units[unit[i].iType].infantry == false)
                continue; // do not draw aircraft

            int drawx = unit[i].draw_x();
            int drawy = unit[i].draw_y();

			//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(255,255,255));

            if (((drawx+units[unit[i].iType].bmp_width) > 0 && drawx < (game.screen_x-160)) &&
                ((drawy+units[unit[i].iType].bmp_height) > 42 && drawy < game.screen_y))
            {

                // draw
                unit[i].draw();

				//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(0,255,255));

				if (key[KEY_D] && key[KEY_TAB])
					alfont_textprintf(bmp_screen, game_font, unit[i].draw_x(),unit[i].draw_y(), makecol(255,255,255), "%d", i);

            }

        }

    }

    // draw all units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {

            if (unit[i].iType == CARRYALL ||
                unit[i].iType == ORNITHOPTER ||
                unit[i].iType == FRIGATE ||
                unit[i].iType == SANDWORM ||
                units[unit[i].iType].infantry)
                continue; // do not draw aircraft

            int drawx = unit[i].draw_x();
            int drawy = unit[i].draw_y();

			//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(255,255,255));

            if (((drawx+units[unit[i].iType].bmp_width) > 0 && drawx < (game.screen_x-160)) &&
                ((drawy+units[unit[i].iType].bmp_height) > 42 && drawy < game.screen_y))
            {

                // draw
                unit[i].draw();

				//line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(0,255,255));

				if (key[KEY_D] && key[KEY_TAB])
					alfont_textprintf(bmp_screen, game_font, unit[i].draw_x(),unit[i].draw_y(), makecol(255,255,255), "%d", i);

            }

        }

    }

    int mc = player[HUMAN].getGameControlsContext()->getMouseCell();
    if (mc > -1)
    {
        if (map.cell[mc].id[MAPID_UNITS] > -1)
        {
            int iUnitId = map.cell[mc].id[MAPID_UNITS];

            if (unit[iUnitId].iTempHitPoints < 0)
                game.hover_unit = iUnitId;
        }
        else if (map.cell[mc].id[MAPID_WORMS] > -1)
        {
            int iUnitId = map.cell[mc].id[MAPID_WORMS];
            game.hover_unit = iUnitId;
        }
    }

}

// draw airborn units
void cMap::draw_units_2nd()
{
    set_trans_blender(0, 0, 0, 160);

    // draw unit power
    if (game.hover_unit > -1)
    {
		if (unit[game.hover_unit].iType == HARVESTER)
			unit[game.hover_unit].draw_spice();

        unit[game.hover_unit].draw_health();
		unit[game.hover_unit].draw_experience();

    }

    // draw health of units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {
            if (unit[i].bSelected)
			{
               unit[i].draw_health();
			   unit[i].draw_experience();
			}
        }

    }

	// draw all units
    for (int i=0; i < MAX_UNITS; i++)
    {
        if (unit[i].isValid())
        {

            if (((unit[i].draw_x()+units[unit[i].iType].bmp_width) > 0 && unit[i].draw_x() < game.screen_x) &&
                ((unit[i].draw_y()+units[unit[i].iType].bmp_height) > 0 && unit[i].draw_y() < game.screen_y))
            {
                // Draw aircraft here
                if (unit[i].iType == CARRYALL ||
                    unit[i].iType == ORNITHOPTER ||
                    unit[i].iType == FRIGATE)
                {
                    unit[i].draw();
                }
            }
        }

    }

    set_trans_blender(0, 0, 0, 128);
}

// Create a TYPE of terrain with TILE (when specified)
// at CELL
void cMap::create_spot(int c, int type, int tile)
{
	assert(c > -1);
	assert(c < MAX_CELLS);
	assert(tile > -1);

    // Set
    cell[c].type = type;
    cell[c].tile = tile;
    cell[c].credits = 0;
    cell[c].passable = true;

    cell[c].smudgetile=-1;
    cell[c].smudgetype=-1;

    // when spice
    if (type == TERRAIN_SPICE || type == TERRAIN_SPICEHILL)
        cell[c].credits = 150 + rnd(250);

    if (type == TERRAIN_MOUNTAIN)
        cell[c].passable = false;

	if (type == TERRAIN_WALL)
	{
		cell[c].health = 100;
		cell[c].passable = false;
	}

}

void cMap::smooth_spot(int c)
{

    int above = CELL_ABOVE(c);
    int under = CELL_UNDER(c);
    int left  = CELL_LEFT(c);
    int right = CELL_RIGHT(c);

    if (above > -1) {
        smooth_cell(above);
    }

    if (under > -1) {
        smooth_cell(under);
    }

    if (left > -1) {
        smooth_cell(left);
    }

    if (right > -1) {
        smooth_cell(right);
    }

    if (c > -1) {
        smooth_cell(c);
    }
}

// smooths out ROCK/SAND/SPICE/HILL/SPICEHILL stuff
void cMap::smooth()
{
    // loop through all cells and smooth
    for (int c=0; c < MAX_CELLS; c++) {
        smooth_cell(c);
    }

}

void cMap::soft()
{
	// soft out rocky spots!
	for (int x=1; x < 63; x++)
		for (int y=1; y< 63; y++)
		{
			int cll=iCellMake(x,y);
			if (cell[cll].type == TERRAIN_ROCK)
			{
				int iC=0;

				if (cell[iCellMake(x-1, y)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x, y+1)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x+1, y)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x, y-1)].type == TERRAIN_ROCK)
					iC++;

				if (cell[iCellMake(x-1, y)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (cell[iCellMake(x, y+1)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (cell[iCellMake(x+1, y)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (cell[iCellMake(x, y-1)].type == TERRAIN_MOUNTAIN)
					iC++;

				if (iC < 2)
				{
					cell[cll].type = TERRAIN_SAND;
					cell[cll].tile = 0;
				}
			}
		}
}

//
void cMap::smooth_cell(int c)
{
    // smooth cell

    int above = CELL_ABOVE(c);
    int under = CELL_UNDER(c);
    int left  = CELL_LEFT(c);
    int right = CELL_RIGHT(c);

    int above_type = -1;
    int under_type = -1;
    int left_type  = -1;
    int right_type = -1;

    // checks
    if (above > -1)
      above_type = cell[above].type;
    if (under > -1)
      under_type = cell[under].type;
    if (left > -1)
      left_type  = cell[left].type;
    if (right > -1)
      right_type = cell[right].type;

    // Rock smoothing (is this a rocky tile?)
    if (cell[c].type == TERRAIN_ROCK)
    {

      bool a, u, l, r;

      // rock above?
      if ((above_type == TERRAIN_ROCK) || (above_type == TERRAIN_SLAB))
        a = true;
      else
        a = false;

      // rock under?
      if ((under_type == TERRAIN_ROCK) || (under_type == TERRAIN_SLAB))
        u = true;
      else
        u = false;

      // rock left?
      if ((left_type == TERRAIN_ROCK) ||  (left_type == TERRAIN_SLAB))
        l = true;
      else
        l = false;

      // rock right?
      if ((right_type == TERRAIN_ROCK) || (right_type == TERRAIN_SLAB))
        r = true;
      else
        r = false;

      // extra for mountains
      // rock above?
      if (above_type == TERRAIN_MOUNTAIN)
        a = true;

      // rock under?
      if (under_type == TERRAIN_MOUNTAIN)
        u = true;

      // rock left?
      if (left_type == TERRAIN_MOUNTAIN)
        l = true;

      // rock right?
      if (right_type == TERRAIN_MOUNTAIN)
        r = true;


      // extra for walls
      // rock above?
      if (above_type == TERRAIN_WALL)
        a = true;

      // rock under?
      if (under_type == TERRAIN_WALL)
        u = true;

      // rock left?
      if (left_type == TERRAIN_WALL)
        l = true;

      // rock right?
      if (right_type == TERRAIN_WALL)
        r = true;

/*
      // Extra for debris! (broken stuff)
      if ((above_type >= STR_B_BOTTOM && above_type <= STR_TURRET))
        a = true;

      // rock under?
      if ((under_type >= STR_B_BOTTOM && under_type <= STR_TURRET))
        u = true;

      // rock left?
      if ((left_type >= STR_B_BOTTOM && left_type <= STR_TURRET))
        l = true;

      // rock right?
      if ((right_type >= STR_B_BOTTOM && right_type <= STR_TURRET))
        r = true; */


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile

      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only

      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)

      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)

      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner

      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner

      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner

      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner

      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT

      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER

      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE


      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile -= 1; // because 0 is actually 1, etc
    }


    // Mountain smoothing
    if (cell[c].type == TERRAIN_MOUNTAIN)
    {

      bool a, u, l, r;

      // rock above?
      if ((above_type == TERRAIN_MOUNTAIN))
        a = true;
      else
        a = false;

      // rock under?
      if ((under_type == TERRAIN_MOUNTAIN))
        u = true;
      else
        u = false;

      // rock left?
      if ((left_type == TERRAIN_MOUNTAIN) )
        l = true;
      else
        l = false;

      // rock right?
      if ((right_type == TERRAIN_MOUNTAIN))
        r = true;
      else
        r = false;

      /*
      if ((above_type >= ROCK1 && above_type <= ROCK9))
        a = true;

      // rock under?
      if ((under_type >= ROCK1 && under_type <= ROCK9))
        u = true;

      // rock left?
      if ((left_type >= ROCK1 && left_type <= ROCK9))
        l = true;

      // rock right?
      if ((right_type >= ROCK1 && right_type <= ROCK9))
        r = true;
        */

      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile

      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only

      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)

      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)

      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile =7; // corner

      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner

      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner

      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner

      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT

      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER

      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE


      // accompanied MOUNTAIN (all sides MOUNTAIN too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely MOUNTAIN

      cell[c].tile -= 1;
    }


    // Spice smoothing
    if (cell[c].type == TERRAIN_SPICE)
    {

      bool a, u, l, r;

      // spice above?
      if (above_type == TERRAIN_SPICE)
        a = true;
      else
        a = false;

      // spice under?
      if (under_type == TERRAIN_SPICE)
        u = true;
      else
        u = false;

      // spice left?
      if (left_type == TERRAIN_SPICE)
        l = true;
      else
        l = false;

      // spice right?
      if (right_type == TERRAIN_SPICE)
        r = true;
      else
        r = false;


      // For spice smoothing (normal spice) we simply check if anything 'spice hill' is near
      // and then we count that as spice aswell.

      // spice above?
      if (above_type == TERRAIN_SPICEHILL)
        a = true;

      // spice under?
      if (under_type == TERRAIN_SPICEHILL)
        u = true;

      // spice left?
      if (left_type == TERRAIN_SPICEHILL)
        l = true;

      // spice right?
      if (right_type == TERRAIN_SPICEHILL)
        r = true;


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile

      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only

      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)

      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)

      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner

      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner

      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner

      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner

      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT

      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER

      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE


      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile--;

    }


    // Spice HILL smoothing
    if (cell[c].type == TERRAIN_SPICEHILL)
    {

      bool a, u, l, r;

      // Now we only check the hills

      // spice above?
      if (above_type == TERRAIN_SPICEHILL)
        a = true;
      else
        a = false;

      // spice under?
      if (under_type == TERRAIN_SPICEHILL)
        u = true;
      else
        u = false;

      // spice left?
      if (left_type == TERRAIN_SPICEHILL)
        l = true;
      else
        l = false;

      // spice right?
      if (right_type == TERRAIN_SPICEHILL)
        r = true;
      else
        r = false;


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile

      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only

      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)

      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)

      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner

      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner

      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner

      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner

      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT

      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER

      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE


      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile--;

    }

    // Normal hills smoothing
    if (cell[c].type == TERRAIN_HILL)
    {

      bool a, u, l, r;

      // Now we only check the hills

      // spice above?
      if (above_type == TERRAIN_HILL)
        a = true;
      else
        a = false;

      // spice under?
      if (under_type == TERRAIN_HILL)
        u = true;
      else
        u = false;

      // spice left?
      if (left_type == TERRAIN_HILL)
        l = true;
      else
        l = false;

      // spice right?
      if (right_type == TERRAIN_HILL)
        r = true;
      else
        r = false;


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 10; // lonely tile

      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 12; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 13; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 14; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 15; // at the bottom only

      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 11; // one way connection  (left/right)

      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 16; // one way connection  (up/down)

      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 7; // corner

      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 8; // corner

      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 9; // corner

      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 6; // corner

      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 3; // border LEFT

      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 2; // border RIGHT

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 5; // border UNDER

      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 4; // border ABOVE


      // accompanied rock (all sides rock too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 1; // not so lonely rock

      cell[c].tile--;

    }


    // Walls smoothing
    if (cell[c].type == TERRAIN_WALL)
    {

      bool a, u, l, r;

      // rock above?
      if ((above_type == TERRAIN_WALL))
        a = true;
      else
        a = false;

      // WALL under?
      if ((under_type == TERRAIN_WALL))
        u = true;
      else
        u = false;

      // WALL left?
      if ((left_type == TERRAIN_WALL))
        l = true;
      else
        l = false;

      // WALL right?
      if ((right_type == TERRAIN_WALL))
        r = true;
      else
        r = false;

      /*
      // Extra for debris! (broken stuff)
      if ((above_type >= STR_B_BOTTOM && above_type <= STR_TURRET))
        a = true;

      // rock under?
      if ((under_type >= STR_B_BOTTOM && under_type <= STR_TURRET))
        u = true;

      // rock left?
      if ((left_type >= STR_B_BOTTOM && left_type <= STR_TURRET))
        l = true;

      // rock right?
      if ((right_type >= STR_B_BOTTOM && right_type <= STR_TURRET))
        r = true; */


      // decide the proper tile!
      if (l == false && a == false && r == false && u == false)
        cell[c].tile = 12; // lonely tile

      if (l == true && a == false && r == false && u == false)
        cell[c].tile = 10; // at the left only

      if (l == false && a == false && r == true && u == false)
        cell[c].tile = 10; // at the right only

      if (l == false && a == true && r == false && u == false)
        cell[c].tile = 12; // at the top only

      if (l == false && a == false && r == false && u == true)
        cell[c].tile = 8; // at the bottom only

      if (l == true && a == false && r == true && u == false)
        cell[c].tile = 10; // one way connection  (left/right)

      if (l == false && a == true && r == false && u == true)
        cell[c].tile = 8; // one way connection  (up/down)

      // corners
      if (l == true && a == true && r == false && u == false)
        cell[c].tile = 3; // corner

      if (l == true && a == false && r == false && u == true)
        cell[c].tile = 11; // corner

      if (l == false && a == true && r == true && u == false)
        cell[c].tile = 2; // corner

      if (l == false && a == false && r == true && u == true)
        cell[c].tile = 1; // corner

      // Border fixes
      if (l == true && a == true && r == false && u == true)
        cell[c].tile = 5; // border LEFT

      if (l == false && a == true && r == true && u == true)
        cell[c].tile = 4; // border RIGHT

      if (l == true && a == true && r == true && u == false)
        cell[c].tile = 7; // border UNDER

      if (l == true && a == false && r == true && u == true)
        cell[c].tile = 6; // border ABOVE


      // accompanied WALL (all sides WALL too)
      if (l == true && a == true && r == true && u == true)
        cell[c].tile = 9; // not so lonely WALL

      cell[c].tile--;

    }
}


// TODO: move this somewhere to a mouse related class
void cMap::draw_think() {
	// busy with selecting box, so do not think (about scrolling, etc)
	if (mouse_co_x1 > -1 && mouse_co_y1 > -1) {
		return;
	}

	// determine the width and height in cells
	// this way we know the size of the viewport

	int iEndX = mapCamera->getX() + ((game.screen_x - 160) / 32); // width of sidebar is 160
	int iEndY = mapCamera->getY() + ((game.screen_y - 42) / 32) + 1; // height of upper bar is 42

	// thinking for map (scrolling that is)
	if (mouse_x <= 1 || key[KEY_LEFT]) {
		if (mapCamera->getX() > 1) {
			mouse_tile = MOUSE_LEFT;
		}
	}

	if (mouse_y <= 1 || key[KEY_UP]) {
		if (mapCamera->getY() > 1) {
			mouse_tile = MOUSE_UP;
		}
	}

	if (mouse_x >= (game.screen_x - 2) || key[KEY_RIGHT]) {
		if ((iEndX) < (game.map_width - 1)) {
			mouse_tile = MOUSE_RIGHT;
		}
	}

	if (mouse_y >= (game.screen_y - 2) || key[KEY_DOWN]) {
		if ((iEndY) < (game.map_height - 1)) {
			mouse_tile = MOUSE_DOWN;
		}
	}
}

void cMap::thinkInteraction() {
	mapCamera->thinkInteraction();
}

int cMap::mouse_draw_x()
{
	if (player[HUMAN].getGameControlsContext()->getMouseCell() > -1)
  return ( (( iCellGiveX(player[HUMAN].getGameControlsContext()->getMouseCell()) * 32 ) - (mapCamera->getX()*32)));
	else
		return -1;
}

int cMap::mouse_draw_y()
{
	if (player[HUMAN].getGameControlsContext()->getMouseCell() > -1)
  return (( (( iCellGiveY(player[HUMAN].getGameControlsContext()->getMouseCell()) * 32 ) - (mapCamera->getY()*32)))+42);
	else
		return -1;
}
