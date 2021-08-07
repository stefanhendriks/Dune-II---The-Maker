/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2021 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"
#include "cMap.h"


#include <math.h>

cMap::cMap() {
	TIMER_scroll=0;
	iScrollSpeed=1;
	maxCells = 0;
}

cMap::~cMap() {
	// do not trigger getInstance from structure factory
    for (int i=0; i < MAX_STRUCTURES; i++) {
        // clear out all structures
        cAbstractStructure *pStructure = structure[i];
        if (pStructure) {
            delete pStructure;
        }
        // clear pointer
        structure[i] = nullptr;
    }
}

void cMap::init(int width, int height) {
    INIT_REINFORCEMENT();

    maxCells = width * height;
    cell = std::vector<tCell>(maxCells, tCell());

    // clear out all cells
    clearAllCells();

    cStructureFactory::getInstance()->deleteAllExistingStructures();

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

    this->width = width;
    this->height = height;
}

void cMap::smudge_increase(int iType, int iCell) {
    if (!map.isValidCell(iCell)) return;
    tCell *pCell = getCell(iCell);

    if (pCell->smudgetype < 0)
        pCell->smudgetype = iType;

    if (pCell->smudgetype == SMUDGE_WALL)
        pCell->smudgetile = 0;

    if (pCell->smudgetype == SMUDGE_ROCK) {
        if (pCell->smudgetile < 0)
            pCell->smudgetile = rnd(2);
        else if (pCell->smudgetile + 2 < 6)
            pCell->smudgetile += 2;
    }

    if (pCell->smudgetype == SMUDGE_SAND) {
        if (pCell->smudgetile < 0)
            pCell->smudgetile = rnd(2);
        else if (pCell->smudgetile + 2 < 6)
            pCell->smudgetile += 2;
    }
}

/**
 * Is this cell occupied? This returns true when type of terrain is WALL or MOUNTAIN.
 *
 * @param iCell
 * @return
 */
bool cMap::occupiedByType(int iCell) {
    if (iCell < 0 || iCell >= maxCells) return false;

	if (map.getCellType(iCell) == TERRAIN_WALL) return true;
	if (map.getCellType(iCell) == TERRAIN_MOUNTAIN) return true;

	return false;
}

bool cMap::occupiedInDimension(int iCell, int dimension) {
    if (!map.isValidCell(iCell)) return false;
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
    if (iCell < 0 || iCell >= maxCells) return false;

    if (occupiedInDimension(iCell, MAPID_UNITS)) return true;
    if (occupiedInDimension(iCell, MAPID_AIR)) return true;
    if (occupiedInDimension(iCell, MAPID_STRUCTURES)) return true;
    if (occupiedByType(iCell)) return true;

	return false;
}

/**
 * Checks if a cary-all (passed with iUnitID) can deploy a unit at iCll
 * @param iCll (cell to deploy)
 * @param iUnitID (the carry-all)
 * @return
 */
bool cMap::canDeployUnitAtCell(int iCll, int iUnitID) {
    if (iCll < 0 || iUnitID < 0)
        return false;

    cUnit &pUnit = unit[iUnitID];
    if (!pUnit.isAirbornUnit()) return false; // weird unit passed in
    if (pUnit.iNewUnitType < 0) return false; // safe-guard when this unit has no new unit to spawn

    s_UnitP &unitToDeploy = units[pUnit.iNewUnitType];

    int structureIdOnMap = map.getCellIdStructuresLayer(iCll);
    if (structureIdOnMap > -1) {
        // the cell contains a structure that the unit wants to enter (for repairment?)
        if (pUnit.iStructureID > -1) {
            if (structureIdOnMap == pUnit.iStructureID) {
                return true;
            }
        }

        // all other cases are occupied / blocked
        return false;
    }

    if (!unitToDeploy.airborn) {
        int cellIdOnMap = map.getCellIdUnitLayer(iCll);
        if (cellIdOnMap > -1 && cellIdOnMap != iUnitID) {
            return false; // other unit at cell
        }
    }

    // walls block as do mountains
    if (map.getCellType(iCll) == TERRAIN_WALL) {
        return false;
    }

    // mountains only block infantry
    if (map.getCellType(iCll) == TERRAIN_MOUNTAIN) {
        // we can deploy infantry types on mountains, airborn units can fly over
        if (!unitToDeploy.infantry && !unitToDeploy.airborn) {
            return false;
        }
    }

    return true;
}

/**
 * Returns true if cell is occupied, for a given unit. The unitId is given so it can ignore its own ID if that
 * is recorded on map data. (ie when moving, a unit can occupy 2 cells at once).
 * @param iCll
 * @param iUnitID
 * @return
 */
bool cMap::occupied(int iCll, int iUnitID) {
    if (iCll < 0 || iUnitID < 0)
        return true;

    cUnit &pUnit = unit[iUnitID];

    // TODO: when unit wants to enter a structure...

    int structureIdOnMap = map.getCellIdStructuresLayer(iCll);
    if (structureIdOnMap > -1) {
        // the cell contains a structure that the unit wants to enter
        if (pUnit.iStructureID > -1) {
            if (structureIdOnMap == pUnit.iStructureID) {
                return false;
            }
        }

        // all other cases are occupied / blocked
        return true;
    }

    // non airborn units can block each other
    if (!pUnit.isAirbornUnit() && !pUnit.isSandworm()) {
        int cellIdOnMap = map.getCellIdUnitLayer(iCll);
        if (cellIdOnMap > -1 && cellIdOnMap != iUnitID) {
            return true; // other unit at cell
        }
    }

    // walls block as do mountains
    if (map.getCellType(iCll) == TERRAIN_WALL) {
        return true;
    }

    // mountains only block infantry
    if (map.getCellType(iCll) == TERRAIN_MOUNTAIN) {
        if (!pUnit.isInfantryUnit() && !pUnit.isAirbornUnit()) {
            return true;
        }
    }

    return false;
}

// do the static info thinking
void cMap::think_minimap() {
	// Draw static info
	cMiniMapDrawer * miniMapDrawer = drawManager->getMiniMapDrawer();
	miniMapDrawer->think();

}

void cMap::draw_bullets() {
    // Loop through all units, check if they should be drawn, and if so, draw them
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullet[i].bAlive) {
            bullet[i].draw();
        }
    }
}

void cMap::clear_all(int playerId) {
    for (int c=0; c < maxCells; c++) {
        cell[c].iVisible[playerId] = true;
    }
}

void cMap::clearShroudForAllPlayers(int c, int size) {
    for (int p = 0; p < MAX_PLAYERS; p++) {
        clearShroud(c, size, p);
    }
}

void cMap::clearShroud(int c, int size, int playerId) {
    if (!map.isWithinBoundaries(c)) return;

    map.setVisibleFor(c, playerId);

    // go around 360 fDegrees and calculate new stuff.
    for (float dr = 1; dr < size; dr++) {
        for (float d = 0; d < 360; d++) { // if we reduce the amount of degrees, we don't get full coverage.
            // need a smarter way to do this (less CPU intensive).

            int x = map.getAbsoluteXPositionFromCellCentered(c);
            int y = map.getAbsoluteYPositionFromCellCentered(c);

            float dr1 = cos(d) * (dr * TILESIZE_WIDTH_PIXELS);
            float dr2 = sin(d) * (dr * TILESIZE_HEIGHT_PIXELS);

            x = (x + dr1);
            y = (y + dr2);

            // convert back
            int cl = mapCamera->getCellFromAbsolutePosition(x, y);

            if (cl < 0) continue;

//            if (DEBUGGING) {
//                int cellDrawX = mapCamera->getWindowXPositionFromCell(cl);
//                int cellDrawY = mapCamera->getWindowYPositionFromCell(cl);
//                rectfill(bmp_screen, cellDrawX, cellDrawY, cellDrawX+32, cellDrawY+32, makecol(0,32,0));
//            }

            if (!map.isVisible(cl, playerId)) {
                map.setVisibleFor(cl, playerId);

                int structureId = map.getCellIdStructuresLayer(cl);
                if (structureId > -1) {
                    cAbstractStructure *pStructure = structure[structureId];
                    s_GameEvent event {
                            .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                            .entityType = eBuildType::STRUCTURE,
                            .entityID = structureId,
                            .player = &players[playerId],
                            .entitySpecificType = pStructure->getType(),
                            .atCell = cl
                    };

                    game.onNotify(event);
                }

                int unitId = map.getCellIdUnitLayer(cl);
                if (unitId > -1) {
                    cUnit &cUnit = unit[unitId];
                    if (cUnit.isValid()) {
                        s_GameEvent event{
                                .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                                .entityType = eBuildType::UNIT,
                                .entityID = unitId,
                                .player = &players[playerId],
                                .entitySpecificType = cUnit.getType(),
                                .atCell = cl
                        };

                        game.onNotify(event);
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
	for (int iCell=0; iCell < maxCells; iCell++) {
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

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            cUnit.draw();
        }

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
            putpixel(bmp_screen, cUnit.center_draw_x(), cUnit.center_draw_y(), makecol(255, 0, 255));
            if (key[KEY_TAB]) { // render unit id
                alfont_textprintf(bmp_screen, game_font, cUnit.draw_x(), cUnit.draw_y(), makecol(255, 255, 255), "%d", i);
            }
        }

    }

    // then: draw infantry units
    for (int i=0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;

        if (!cUnit.isInfantryUnit())
            continue; // skip non-infantry units

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            // draw
            cUnit.draw();

        }

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
            putpixel(bmp_screen, cUnit.center_draw_x(), cUnit.center_draw_y(), makecol(255, 0, 255));
            if (key[KEY_TAB]) { // render unit id
                alfont_textprintf(bmp_screen, game_font, cUnit.draw_x(), cUnit.draw_y(), makecol(255, 255, 255), "%d", i);
            }
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

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            // draw
            cUnit.draw();
        }

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
            putpixel(bmp_screen, cUnit.center_draw_x(), cUnit.center_draw_y(), makecol(255, 0, 255));
            if (key[KEY_TAB]) { // render unit id
                alfont_textprintf(bmp_screen, game_font, cUnit.draw_x(), cUnit.draw_y(), makecol(255, 255, 255), "%d", i);
            }
        }

    }

    // TODO: move somewhere else than drawing function
    int mc = players[HUMAN].getGameControlsContext()->getMouseCell();
    if (mc > -1) {
        tCell &cellOfMouse = map.cell[mc];

        if (cellOfMouse.id[MAPID_UNITS] > -1) {
            int iUnitId = cellOfMouse.id[MAPID_UNITS];

            if (unit[iUnitId].iTempHitPoints < 0) {
                game.hover_unit = iUnitId;
            }

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

        if (cUnit.dimensions->isOverlapping(game.mapViewport)) {
            cUnit.draw();
        }

        if (key[KEY_D]) {
            allegroDrawer->drawRectangle(bmp_screen, cUnit.dimensions, makecol(255, 0, 255));
            putpixel(bmp_screen, cUnit.center_draw_x(), cUnit.center_draw_y(), makecol(255, 0, 255));
            if (key[KEY_TAB]) { // render unit id
                alfont_textprintf(bmp_screen, game_font, cUnit.draw_x(), cUnit.draw_y(), makecol(255, 255, 255), "%d", i);
            }
        }
    }

    set_trans_blender(0, 0, 0, 128);
}

void cMap::thinkInteraction() {
	mapCamera->thinkInteraction();
}

int cMap::mouse_draw_x() {
    if (players[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = players[HUMAN].getGameControlsContext()->getMouseCell();
        int absX = getAbsoluteXPositionFromCell(mouseCell);
        return mapCamera->getWindowXPosition(absX);
    }
    return -1;
}

int cMap::mouse_draw_y() {
    if (players[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = players[HUMAN].getGameControlsContext()->getMouseCell();
        int absY = getAbsoluteYPositionFromCell(mouseCell);
        return mapCamera->getWindowYPosition(absY);
    }
    return -1;
}

int cMap::getTotalCountCellType(int cellType) {
    int count = 0;
    for (int c = 0; c < maxCells; c++) {
        if (getCellType(c) == cellType) {
            count++;
        }
    }
    return count;
}

int cMap::getCellSlowDown(int iCell) {
    int cellType = map.getCellType(iCell);

    if (cellType == TERRAIN_SAND) return 2;
    if (cellType == TERRAIN_MOUNTAIN) return 5;
    if (cellType == TERRAIN_HILL) return 3;
    if (cellType == TERRAIN_SPICEHILL) return 3;
    if (cellType == TERRAIN_ROCK) return 1;
    if (cellType == TERRAIN_SLAB) return 0;

    return 1;
}

int cMap::findCloseMapBorderCellRelativelyToDestinationCel(int destinationCell) {
    assert(destinationCell > -1);
    // Cell x and y coordinates
    int iCllX = getCellX(destinationCell);
    int iCllY = getCellY(destinationCell);

    // STEP 1: determine starting
    int iStartCell=-1;
    int lDistance=9999;

    int tDistance=9999;
    int cll=-1;

    // HORIZONTAL cells
    for (int iX=0; iX < width; iX++) {
        // check when Y = 0 (top)
        tDistance = distance(iX, 0, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = makeCell(iX, 0);

            if (map.occupied(cll) == false) {
                iStartCell = cll;
            }
        }

        // check when Y = map_height (bottom)
        tDistance = distance(iX, height-1, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = makeCell(iX, height - 1);

            if (map.occupied(cll) == false) {
                iStartCell = cll;
            }
        }
    }

    // VERTICAL cells
    for (int iY=0; iY < height; iY++)
    {
        // check when X = 0 (left)
        tDistance = distance(0, iY, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = makeCell(0, iY);

            if (map.occupied(cll) == false) {
                iStartCell = cll;
            }
        }

        // check when XY = map_width (bottom)
        tDistance = distance(width-1, iY, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;
            cll = makeCell(width - 1, iY);

            if (map.occupied(cll) == false) {
                iStartCell = cll;
            }
        }
    }

    return iStartCell;
}

double cMap::distance(int x1, int y1, int x2, int y2) {
    if (x1 == x2 && y1 == y2) return 1; // when all the same, distance is 1 ...

    int A = abs(x2-x1) * abs(x2-x1);
    int B = abs(y2-y1) * abs(y2-y1);
    return sqrt((double)(A+B)); // get C from A and B
}

int cMap::getCellY(int c) {
    if (c < 0 || c >= maxCells) {
        return -1;
    }

    return (c / width);
}

int cMap::getCellX(int c) {
    if (c < 0 || c >= maxCells) {
        return -1;
    }

    int cellX = c - ((c / width) * width);
    return cellX;
}

bool cMap::isCellAdjacentToOtherCell(int thisCell, int otherCell) {
    if (getCellAbove(thisCell) == otherCell) return true;
    if (getCellBelow(thisCell) == otherCell) return true;
    if (getCellLeft(thisCell) == otherCell) return true;
    if (getCellRight(thisCell) == otherCell) return true;

    //
    if (getCellUpperLeft(thisCell) == otherCell) return true;
    if (getCellUpperRight(thisCell) == otherCell) return true;
    if (getCellLowerLeft(thisCell) == otherCell) return true;
    if (getCellLowerRight(thisCell) == otherCell) return true;

    return false;
}

int cMap::getCellLowerRight(int c)  {
    int lowerRightCell = getCellBelow(c) + 1;
    if (lowerRightCell >= maxCells) return -1;
    if (lowerRightCell < 0) return -1;

    return lowerRightCell;
}

int cMap::getCellLowerLeft(int c) {
    int lowerLeftCell = getCellBelow(c) - 1;
    if (lowerLeftCell < 0) return -1;
    if (lowerLeftCell >= maxCells) return -1;
    return lowerLeftCell;
}

int cMap::getCellUpperRight(int c) {
    int upperRightCell = getCellAbove(c) + 1;
    if (upperRightCell < 0) return -1;

    return upperRightCell;
}

int cMap::getCellUpperLeft(int c) {
    int upperLeftCell = getCellAbove(c) - 1;
    if (upperLeftCell < 0) return -1;

    return upperLeftCell;
}

int cMap::getCellRight(int c) {
    int x = getCellX(c);
    int cellRight = x + 1;
    if (cellRight >= maxCells) return -1;
    if (cellRight >= width) return -1;

    return c + 1;
}

int cMap::getCellLeft(int c) {
    if (c < 0) return -1;
    int x = getCellX(c);
    int cellLeft = x - 1;
    if (cellLeft < 0) return -1;
    return c - 1;
}

int cMap::getCellBelow(int c) {
    if (c < 0) return -1;
    int cellBelow = c + width;
    if (cellBelow >= maxCells)
        return -1;

    return cellBelow;
}

int cMap::getCellAbove(int c) {
    if (c < 0) return -1;
    int cellAbove = c - width;

    if (cellAbove < 0) return -1;

    return cellAbove;
}

int cMap::getAbsoluteYPositionFromCell(int cell) {
    if (cell < 0) return -1;
    return getCellY(cell) * TILESIZE_HEIGHT_PIXELS;
}

int cMap::getAbsoluteXPositionFromCell(int cell) {
    if (cell < 0) return -1;
    return getCellX(cell) * TILESIZE_WIDTH_PIXELS;
}

int cMap::getAbsoluteXPositionFromCellCentered(int cell) {
    return getAbsoluteXPositionFromCell(cell) + (TILESIZE_WIDTH_PIXELS / 2);
}

int cMap::getAbsoluteYPositionFromCellCentered(int cell) {
    return getAbsoluteYPositionFromCell(cell) + (TILESIZE_HEIGHT_PIXELS / 2);
}

int cMap::makeCell(int x, int y) {
    assert(x > -1 && "makeCell x must be > -1");
    assert(x < width && "makeCell x must be < width"); // should never be higher!
    assert(y > -1 && "makeCell y must be > -1");
    assert(y < height && "makeCell y must be < height");

    // create cell
    int cell = getCellWithMapDimensions(x, y);

    assert(cell < maxCells); // may never be => (will since MAX_CELLS-1 is max in array!)
    assert(cell > -1); // may never be < 0

    return cell;
}

double cMap::distance(int cell1, int cell2) {
    int x1 = getCellX(cell1);
    int y1 = getCellY(cell1);

    int x2 = getCellX(cell2);
    int y2 = getCellY(cell2);
    return ABS_length(x1, y1, x2, y2);
}

int cMap::getCellWithMapBorders(int x, int y) {
    // internal vars are 1 based (ie 64x64 means 0-63, which really means 1...62 are valid)
    int maxHeight = (height-2); // hence the -2!
    int maxWidth = (width-2);

    if (x < 1) return -1;
    if (y < 1) return -1;
    if (x > maxWidth) return -1;
    if (y > maxHeight) return -1;

    return getCellWithMapDimensions(x, y);
}

int cMap::getCellWithMapDimensions(int x, int y)  {
    int mapWidth = width;
    int mapHeight = height;
    // (over the) boundaries result in cell -1
    if (x < 0) return -1;
    if (x >= mapWidth) return -1;
    if (y < 0) return -1;
    if (y >= mapHeight) return -1;

    return (y * mapWidth) + x;
}

bool cMap::isValidCell(int c) {
    return !(c < 0 || c >= maxCells);
}

/**
 * Returns a random cell, disregards playable borders
 * @return
 */
int cMap::getRandomCell() {
    return rnd(maxCells);
}

void cMap::createCell(int cell, int terrainType, int tile) {
    if (!isValidCell(cell)) return;
    if (terrainType > TERRAIN_WALL) return;
    if (terrainType < TERRAIN_BLOOM) return;
    if (tile < 0) return;
    if (tile > 16) return;

    assert(terrainType >= TERRAIN_BLOOM);
    assert(terrainType <= TERRAIN_WALL);
    assert(tile > -1);
    assert(tile < 17);

    // Set
    map.cellChangeType(cell, terrainType);
    map.cellChangeTile(cell, tile);
    map.cellChangeCredits(cell, 0);
    if (terrainType == TERRAIN_BLOOM) {
        map.cellChangeCredits(cell, -23);
    }
    map.cellChangePassable(cell, true);
    map.cellChangePassableFoot(cell, true);

    map.cellChangeSmudgeTile(cell, -1);
    map.cellChangeSmudgeType(cell, -1);

    // when spice
    if (terrainType == TERRAIN_SPICE || terrainType == TERRAIN_SPICEHILL) {
        map.cellChangeCredits(cell, 50 + rnd(250));
    } else if (terrainType == TERRAIN_MOUNTAIN) {
        map.cellChangePassable(cell, false);
        map.cellChangePassableFoot(cell, true);
    } else if (terrainType == TERRAIN_WALL) {
        map.cellChangeHealth(cell, 100);
        map.cellChangePassable(cell, false);
        map.cellChangePassableFoot(cell, false);
    }
}

void cMap::clearAllCells() {
    for (int c=0; c < getMaxCells(); c++) {
        cellInit(c);
    }
}

bool cMap::isVisible(int iCell, cPlayer *thePlayer) {
    if (!thePlayer) return false;
    int playerId = thePlayer->getId();
    return isVisible(iCell, playerId);
}

void cMap::resize(int width, int height) {
    maxCells = width * height;
    this->width = width;
    this->height = height;
    cell.resize(maxCells);
}

int cMap::getRandomCellWithinMapWithSafeDistanceFromBorder(int distance) {
    return getCellWithMapBorders(
            distance + rnd(width - (distance * 2)),
            distance + rnd(height - (distance * 2))
            );
}

bool cMap::isWithinBoundaries(int c) {
    return isWithinBoundaries(getCellX(c), getCellY(c));
}

void cMap::setVisibleFor(int iCell, cPlayer *pPlayer) {
    if (!pPlayer) return;
    setVisible(iCell, pPlayer->getId(), true);
}

void cMap::setVisible(int iCell, int iPlayer, bool flag) {
    if (!isValidCell(iCell)) return;
    if (iPlayer < 0 || iPlayer >= MAX_PLAYERS) return;

    cell[iCell].iVisible[iPlayer] = flag;
}

int cMap::findNearestSpiceBloom(int iCell) {
    int quarterOfMap = getWidth() / 4;
    int iDistance = quarterOfMap;
    int halfWidth = getWidth() / 2;
    int halfHeight = getHeight() / 2;

    if (iCell < 0) {
        // use cell at center
        iCell = map.getCellWithMapDimensions(halfWidth, halfHeight);
        iDistance = map.getWidth();
    }

    int cx, cy;
    int closestBloomFoundSoFar=-1;
    int bloomsEvaluated = 0;

    cx = map.getCellX(iCell);
    cy = map.getCellY(iCell);

    for (int i=0; i < map.getMaxCells(); i++) {
        int cellType = map.getCellType(i);
        if (cellType != TERRAIN_BLOOM) continue;
        bloomsEvaluated++;

        int d = ABS_length(cx, cy, map.getCellX(i), map.getCellY(i));

        if (d < iDistance) {
            closestBloomFoundSoFar = i;
            iDistance = d;
        }
    }

    // found a close spice bloom
    if (closestBloomFoundSoFar > 0) {
        return closestBloomFoundSoFar;
    }

    // no spice blooms evaluated, abort
    if (bloomsEvaluated < 0) {
        return -1;
    }

    // randomly pick one
    int iTargets[10];
    memset(iTargets, -1, sizeof(iTargets));
    int iT=0;

    for (int i=0; i < map.getMaxCells(); i++) {
        int cellType = map.getCellType(i);
        if (cellType == TERRAIN_BLOOM) {
            iTargets[iT] = i;
            iT++;
            if (iT >= 10) break;
        }
    }

    // when finished, return bloom
    return iTargets[rnd(iT)];
}

bool cMap::isValidTerrainForStructureAtCell(int cll) {
    if (!isValidCell(cll)) return false;
    int cellType = getCellType(cll);
    // TODO: make this a flag in the cell/terrain type kind? (there is no such thing yet, its all hard-coded)
    bool allowedCellType = (cellType == TERRAIN_SLAB || cellType == TERRAIN_ROCK);
    if (!allowedCellType) {
        return false;
    }

    return true;
}

/**
 * returns a (randomized) cell. Taking cell param as 'start' position and 'distance' the amount of cells you want to
 * move away. This function will not chose a random position between cell and position. Hence, the amount of cells
 * to move is guaranteed to be so many cells away.
 * @param cell
 * @param distance
 */
int cMap::getRandomCellFrom(int cell, int distance) {
    int startX = getCellX(cell);
    int startY = getCellY(cell);
    int xDir = rnd(100) < 50 ? -1 : 1;
    int yDir = rnd(100) < 50 ? -1 : 1;
    int newX = (startX - distance) + (xDir * distance);
    int newY = (startY - distance) + (yDir * distance);
    return getCellWithMapBorders(newX, newY);
}

/**
 * Like getRandomCellFrom, but will randomize x/y coordinate. Using 'distance' to create a square around 'cell' (as center)
 * Hence, this could result in a cell being returned much closer than distance. But never further away than distance.
 * @param cell
 * @param distance
 * @return
 */
int cMap::getRandomCellFromWithRandomDistance(int cell, int distance) {
    int startX = getCellX(cell);
    int startY = getCellY(cell);
    int newX = (startX - distance) + (rnd(distance*2));
    int newY = (startY - distance) + (rnd(distance*2));
    return getCellWithMapBorders(newX, newY);
}
/**
 * Takes structure, evaluates all its cells, and if any of these are visible, this function returns true.
 *
 * @param pStructure
 * @param iPlayer
 * @return
 */
bool cMap::isStructureVisible(cAbstractStructure *pStructure, int iPlayer) {
    if (!pStructure) return false;
    if (!pStructure->isValid()) return false;

    // iterate over all cells of structure
    const std::vector<int> &cells = pStructure->getCellsOfStructure();
    for (auto &cll : cells) {
        if (map.isVisible(cll, iPlayer)) {
            return true;
        }
    }

    return false;
}
