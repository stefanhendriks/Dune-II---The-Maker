/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2010 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"

#include <math.h>

#define WATCH_PLAYER 0

cMap::cMap() {
	TIMER_scroll=0;
	iScrollSpeed=1;
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

void cMap::init(int width, int height) {
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
	iScrollSpeed=1;

	delete mapUtils;

    this->width = width;
    this->height = height;
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
    if (iCell < 0 || iCell >= MAX_CELLS) return false;

	if (map.cell[iCell].type == TERRAIN_WALL) return true;
	if (map.cell[iCell].type == TERRAIN_MOUNTAIN) return true;

	return false;
}

bool cMap::occupiedInDimension(int iCell, int dimension) {
    if (iCell < 0 || iCell >= MAX_CELLS) return false;
    if (dimension < 0 || dimension >= MAPID_MAX) return false;

	return map.cell[iCell].id[dimension] > -1;
}

/**
 * Is the specific cell occupied by any dimension?
 *
 * @param iCll
 * @return
 */
bool cMap::occupied(int iCell) {
    if (iCell < 0 || iCell >= MAX_CELLS) return false;

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
	cMiniMapDrawer * miniMapDrawer = drawManager->getMiniMapDrawer();
	miniMapDrawer->think();

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

void cMap::clear_spot(int c, int size, int player) {
    // Get the x and y and make a circle around it of 16xR, then calculate of every step the cell and
    // clear it
    int cx = iCellGiveX(c);
    int cy = iCellGiveY(c);

    if (cx < 0 || cy < 0)
        return;

    map.iVisible[c][player] = true;

#define TILE_SIZE_PIXELS 32

    // go around 360 fDegrees and calculate new stuff
    for (int dr = 1; dr < size; dr++) {
        for (double d = 0; d < 360; d++) {
            int x = cx, y = cy;

            // when scrolling, compensate
            x -= mapCamera->getViewportStartX() / 32;
            y -= mapCamera->getViewportStartY() / 32;

            // convert to pixels (*32)
            x *= TILE_SIZE_PIXELS;
            y *= TILE_SIZE_PIXELS;

            y += 42;

            // center on unit
            x += 16;
            y += 16;


            int dx, dy;
            dx = x;
            dy = y;

            x = (x + (cos(d) * ((dr * 32))));
            y = (y + (sin(d) * ((dr * 32))));

            // DEBUG

            //line (bmp_screen, dx, dy, x, y, makecol(255,255,255));
            //if (DEBUGGING)
            //  putpixel(bmp_screen, x, y, makecol(255,255,0));

            //putpixel(bmp_screen, dx, dy, makecol(0,255,0));
            // DEBUG

            // convert back

            int cell_x, cell_y;

            cell_y = y;
            cell_y -= 42;
            cell_y += mapCamera->getViewportStartY();
            cell_y = cell_y / 32;

            cell_x = x;
            cell_x += mapCamera->getViewportStartX();
            cell_x = cell_x / 32;


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

            int cl = iCellMakeWhichCanReturnMinusOne(cell_x, cell_y);
            if (cl < 0) continue;

            if (!iVisible[cl][player]) {

                iVisible[cl][player] = true;     // make visible

                // human unit detected enemy, now be scared and play some neat music
                if (player == HUMAN) {
                    if (cell[cl].id[MAPID_UNITS] > -1) {
                        int id = cell[cl].id[MAPID_UNITS];

                        if (unit[id].iPlayer != HUMAN) // NOT friend
                        {
                            // when state of music is not attacking, do attacking stuff and say "Warning enemy unit approaching
                            if (game.iMusicType == MUSIC_PEACE) {
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

// Each index is a map data field holding indexes of the map layout
//
void cMap::remove_id(int iIndex, int iIDType) {
    // Search through the entire map and remove the id
	for (int iCell=0; iCell < MAX_CELLS; iCell++) {
        tCell &tCell = cell[iCell];
        if (tCell.id[iIDType] == iIndex) {
            tCell.id[iIDType] = -1;
        }
    }
}

void cMap::draw_units() {
    set_trans_blender(0, 0, 0, 160);

    // draw all worms first
    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;

        // DEBUG MODE: DRAW PATHS
        if (DEBUGGING && cUnit.bSelected) {
            cUnit.draw_path();
        }

        if (cUnit.iType != SANDWORM) continue;

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
        }

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            cUnit.draw();
        }
    }

    // then: draw infantry units
    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;

        if (!cUnit.isInfantryUnit())
            continue; // skip non-infantry units

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
        }

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            // draw
            cUnit.draw();

            //line(bmp_screen, mouse_x, mouse_y, unit[i].draw_x(), unit[i].draw_y(), makecol(0,255,255));

            if (key[KEY_D] && key[KEY_TAB])
                alfont_textprintf(bmp_screen, game_font, cUnit.draw_x(), cUnit.draw_y(), makecol(255, 255, 255), "%d", i);

        }
    }

    // then: draw ground units
    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;

        if (cUnit.isAirbornUnit() ||
            cUnit.iType == SANDWORM ||
            cUnit.isInfantryUnit())
            continue; // skip airborn, infantry and sandworm

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
        }

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            // draw
            cUnit.draw();

            if (key[KEY_D] && key[KEY_TAB]) {
                alfont_textprintf(bmp_screen, game_font, cUnit.draw_x(), cUnit.draw_y(), makecol(255, 255, 255), "%d", i);
            }

        }
    }

    int mc = player[HUMAN].getGameControlsContext()->getMouseCell();
    if (mc > -1) {
        tCell &cellOfMouse = map.cell[mc];

        if (cellOfMouse.id[MAPID_UNITS] > -1) {
            int iUnitId = cellOfMouse.id[MAPID_UNITS];

            if (unit[iUnitId].iTempHitPoints < 0)
                game.hover_unit = iUnitId;

        } else if (cellOfMouse.id[MAPID_WORMS] > -1) {
            int iUnitId = cellOfMouse.id[MAPID_WORMS];
            game.hover_unit = iUnitId;
        }
    }
}

// draw 2nd layer for units, this is health/spice bars and eventually airborn units (last)
void cMap::draw_units_2nd() {
    set_trans_blender(0, 0, 0, 160);

    // draw unit power
    if (game.hover_unit > -1) {
        cUnit &hoverUnit = unit[game.hover_unit];

        if (hoverUnit.iType == HARVESTER) {
            hoverUnit.draw_spice();
        }

        hoverUnit.draw_health();
		hoverUnit.draw_experience();
    }

    // draw health of units
    for (int i=0; i < MAX_UNITS; i++) {
        if (unit[i].isValid()) {
            if (unit[i].bSelected) {
               unit[i].draw_health();
			   unit[i].draw_experience();
			}
        }
    }

	// draw airborn units
    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (!cUnit.isAirbornUnit()) continue;

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
        }

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            cUnit.draw();
        }
    }

    set_trans_blender(0, 0, 0, 128);
}

// TODO: move this somewhere to a mouse related class
void cMap::draw_think() {
}

void cMap::thinkInteraction() {
	mapCamera->thinkInteraction();
}

int cMap::mouse_draw_x() {
    if (player[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = player[HUMAN].getGameControlsContext()->getMouseCell();
        int absX = mapCamera->getAbsoluteXPositionFromCell(mouseCell);
        return mapCamera->getWindowXPosition(absX);
    }
    return -1;
}

int cMap::mouse_draw_y() {
    if (player[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = player[HUMAN].getGameControlsContext()->getMouseCell();
        int absY = mapCamera->getAbsoluteYPositionFromCell(mouseCell);
        return mapCamera->getWindowYPosition(absY);
    }
    return -1;
}
