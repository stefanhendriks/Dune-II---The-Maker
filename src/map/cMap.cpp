/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2011 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"

#include <math.h>

#define WATCH_PLAYER 0

cMap::cMap() {
	TIMER_scroll=0;
	iScrollSpeed=10;
	cellCalculator = NULL;
}

cMap::~cMap() {
	if (cellCalculator) delete cellCalculator;
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

// do the static info thinking
void cMap::think_minimap() {
	// Draw static info
	cMiniMapDrawer * miniMapDrawer = gameDrawer->getMiniMapDrawer();
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
                        playMusicByType(MUSIC_ATTACK);

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
