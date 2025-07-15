/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  */

#include "cMap.h"

#include "cMapCamera.h"
#include "cMapEditor.h"
#include "d2tmc.h"
#include "data/gfxdata.h"
#include "gameobjects/particles/cParticle.h"
#include "gameobjects/projectiles/bullet.h"
#include "gameobjects/structures/cStructureFactory.h"
#include "player/cPlayer.h"
#include "gameobjects/units/cReinforcements.h"

#include <fmt/core.h>

#include <algorithm>
#include <cmath>

cMap::cMap()
{
    TIMER_scroll = 0;
    iScrollSpeed = 1;
    maxCells = 0;
    m_reinforcements = nullptr;
    m_iDesiredAmountOfWorms = 0;
    m_iTIMER_respawnSandworms = -1;
    init(64, 64);
}

cMap::~cMap()
{
    // do not trigger getInstance from structure factory
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        // clear out all structures
        cAbstractStructure *pStructure = structure[i];
        if (pStructure) {
            delete pStructure;
        }
        // clear pointer
        structure[i] = nullptr;
    }
}

void cMap::setReinforcements(std::shared_ptr<cReinforcements> reinforcements)
{
    m_reinforcements = reinforcements;
}

void cMap::init(int width, int height)
{
    if (m_reinforcements) {
        m_reinforcements->init();
    }

    m_bAutoDetonateSpiceBlooms = false;
    m_bAutoSpawnSpiceBlooms = true;

    m_iTIMER_blooms = 200;
    m_mBloomTimers = std::map<int, int>();
    m_mBloomTimers.clear();

    m_iDesiredAmountOfWorms = 0;
    m_iTIMER_respawnSandworms = -1;

    maxCells = width * height;
    cell = std::vector<tCell>(maxCells, tCell());

    // clear out all cells
    clearAllCells();

    cStructureFactory::getInstance()->deleteAllExistingStructures();

    for (int i = 0; i < MAX_BULLETS; i++) {
        bullet[i].init();
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        particle[i].init();
    }

    for (int i = 0; i < MAX_UNITS; i++) {
        unit[i].init(i);
    }

    TIMER_scroll = 0;
    iScrollSpeed = 1;

    this->width = width;
    this->height = height;
}

void cMap::smudge_increase(int iType, int iCell)
{
    if (!isValidCell(iCell)) return;
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
bool cMap::occupiedByWallOrMountain(int iCell)
{
    if (iCell < 0 || iCell >= maxCells) return false;

    if (map.getCellType(iCell) == TERRAIN_WALL) return true;
    if (map.getCellType(iCell) == TERRAIN_MOUNTAIN) return true;

    return false;
}

bool cMap::occupiedInDimension(int iCell, int dimension)
{
    if (!map.isValidCell(iCell)) return false;
    if (dimension < 0 || dimension >= MAPID_MAX) return false;

    return map.cell[iCell].id[dimension] > -1;
}

bool cMap::occupiedByUnit(int iCell)
{
    return occupiedInDimension(iCell, MAPID_UNITS);
}

/**
 * Is the specific cell occupied by any dimension?
 *
 * @param iCll
 * @return
 */
bool cMap::occupied(int iCell)
{
    if (iCell < 0 || iCell >= maxCells) return false;

    if (occupiedInDimension(iCell, MAPID_UNITS)) return true;
    if (occupiedInDimension(iCell, MAPID_AIR)) return true;
    if (occupiedInDimension(iCell, MAPID_STRUCTURES)) return true;
    if (occupiedByWallOrMountain(iCell)) return true;

    return false;
}

bool cMap::canDeployUnitTypeAtCell(int iCell, int iUnitType)
{
    if (iCell < 0 || iUnitType < 0)
        return false;

    s_UnitInfo &unitToDeploy = sUnitInfo[iUnitType];

    bool isAirbornUnit = unitToDeploy.airborn;
    bool isInfantryUnit = unitToDeploy.infantry;
    bool isWorm = (iUnitType == SANDWORM);

    if (isAirbornUnit) return true;

    if (isWorm) {
        int wormId = getCellIdWormsLayer(iCell);
        return map.isCellPassableForWorm(iCell) && wormId < 0;
    }

    if (isInfantryUnit && map.isCellPassableForFootUnits(iCell)) return true;

    if (!map.isCellPassable(iCell)) return false;

    int strucId = getCellIdStructuresLayer(iCell);
    int unitId = getCellIdUnitLayer(iCell);

    return unitId < 0 && strucId < 0;
}

/**
 * Checks if a cary-all (passed with iUnitIDWithinStructure) can deploy a unit at iCll.
 *
 * @param iCll (cell to deploy)
 * @param iUnitID (the carry-all)
 * @return
 */
bool cMap::canDeployUnitAtCell(int iCell, int iUnitID)
{
    if (iCell < 0 || iUnitID < 0)
        return false;

    cUnit &pUnit = unit[iUnitID];
    if (!pUnit.isAirbornUnit()) return false; // weird unit passed in
    if (pUnit.iNewUnitType < 0) return false; // safe-guard when this unit has no new unit to spawn

    int structureIdOnMap = map.getCellIdStructuresLayer(iCell);
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

    s_UnitInfo &unitToDeploy = sUnitInfo[pUnit.iNewUnitType];

    bool isAirbornUnit = unitToDeploy.airborn;
    bool isInfantryUnit = unitToDeploy.infantry;

    if (!isAirbornUnit) {
        int cellIdOnMap = map.getCellIdUnitLayer(iCell);
        if (cellIdOnMap > -1 && cellIdOnMap != iUnitID) {
            return false; // other unit at cell
        }
    }

    // walls block as do mountains
    if (map.getCellType(iCell) == TERRAIN_WALL) {
        return false;
    }

    // mountains only block infantry
    if (map.getCellType(iCell) == TERRAIN_MOUNTAIN) {
        // we can deploy infantry types on mountains, airborn units can fly over
        if (!isInfantryUnit && !isAirbornUnit) {
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
bool cMap::occupied(int iCll, int iUnitID)
{
    if (iCll < 0 || iUnitID < 0)
        return true;

    cUnit &pUnit = unit[iUnitID];

    int structureIdOnMap = map.getCellIdStructuresLayer(iCll);
    if (structureIdOnMap > -1) {
        // the cell contains a structure that the unit wants to enter
        if (pUnit.iStructureID > -1) {
            if (structureIdOnMap == pUnit.iStructureID) {
                // the unit wants to enter this structure, so it does not block the unit
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

/**
 * Think function, called every 5 ms
 */
void cMap::thinkFast()
{
    thinkAutoDetonateSpiceBlooms();
    thinkAboutSpawningNewSpiceBlooms();
    thinkAboutRespawningWorms();
}

void cMap::thinkAboutRespawningWorms()
{
    if (m_iTIMER_respawnSandworms < 0) return;

    if (m_iTIMER_respawnSandworms > 1) {
        m_iTIMER_respawnSandworms--;
        return;
    }

    // timer hit exactly '1'
    m_iTIMER_respawnSandworms--;

    int currentAmountOfWorms = players[AI_WORM].getAmountOfUnitsForType(SANDWORM);
    if (currentAmountOfWorms < m_iDesiredAmountOfWorms) {
        // spawn one worm, set timer again
        int failures = 0;
        while (failures < 10) {
            int cell = map.getRandomCell();
            if (!map.isCellPassableForWorm(cell)) {
                failures++;
                continue;
            }
            logbook(fmt::format("cMap::thinkAboutRespawningWorms : Spawning sandworm at {}", cell));
            UNIT_CREATE(cell, SANDWORM, AI_WORM, true);
            break;
        }
    }

    // we have the desired amount of worms already so don't do anything
}

void cMap::thinkAboutSpawningNewSpiceBlooms()
{
    if (!m_bAutoSpawnSpiceBlooms) {
        return;
    }

    if (m_iTIMER_blooms > 0) {
        m_iTIMER_blooms--;
        return;
    }

    // Evaluate every 30 seconds orso
    m_iTIMER_blooms = (1000 / 5) * 30;

    const std::vector<int> &blooms = getAllCellsOfType(TERRAIN_BLOOM);
    int totalSpiceBloomsCount = blooms.size();

    // When no blooms are detected, we must 'spawn' one
    int desiredAmountOfSpiceBloomsInMap = isBigMap() ? 6 : 3;

    if (totalSpiceBloomsCount < desiredAmountOfSpiceBloomsInMap) {
        // randomly create a new spice bloom somewhere on the map
        int iCll = -1;
        for (int i = 0; i < 10; i++) {
            int cell = getRandomCell();
            // find place to spawn bloom
            if (getCellType(cell) == TERRAIN_SAND) {
                iCll = cell;
                break;
            }
        }

        if (iCll > -1) {
            // create bloom (can deal with < -1 cell)
            cMapEditor(*this).createCell(iCll, TERRAIN_BLOOM, 0);
        }
        else {
            m_iTIMER_blooms = (1000 / 5) * 5; // try again sooner than 30 secs
        }
    }
}

void cMap::thinkAutoDetonateSpiceBlooms()  // let spice bloom detonate after X amount of time
{
    if (!m_bAutoDetonateSpiceBlooms) {
        return;
    }

    std::vector<int> keys;
    for (auto const &pair: m_mBloomTimers) {
        keys.push_back(pair.first);
    }

    // key == cell
    for (auto const &key: keys) {
        m_mBloomTimers[key] -= 1; // decrease timer
        if (m_mBloomTimers[key] < 1) {
            // detonate spice bloom
            map.detonateSpiceBloom(key);
        }
    }
}

void cMap::draw_bullets()
{
    // Loop through all units, check if they should be drawn, and if so, draw them
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullet[i].bAlive) {
            bullet[i].draw();
        }
    }
}

void cMap::clear_all(int playerId)
{
    for (int c = 0; c < maxCells; c++) {
        cell[c].iVisible[playerId] = true;
    }
}

void cMap::clearShroudForAllPlayers(int c, int size)
{
    for (int p = 0; p < MAX_PLAYERS; p++) {
        clearShroud(c, size, p);
    }
}

void cMap::clearShroud(int c, int size, int playerId)
{
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

            if (!map.isVisible(cl, playerId)) {
                map.setVisibleFor(cl, playerId);

                int structureId = map.getCellIdStructuresLayer(cl);
                if (structureId > -1) {
                    cAbstractStructure *pStructure = structure[structureId];
                    s_GameEvent event{
                        .eventType = eGameEventType::GAME_EVENT_DISCOVERED,
                        .entityType = eBuildType::STRUCTURE,
                        .entityID = structureId,
                        .player = &players[playerId],
                        .entitySpecificType = pStructure->getType(),
                        .atCell = cl
                    };

                    game.onNotifyGameEvent(event);
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

                        game.onNotifyGameEvent(event);
                    }
                }
            } // make visible
        }
    }
}

// Each index is a map data field holding indexes of the map layout
//
void cMap::remove_id(int iIndex, int iIDType)
{
    // Search through the entire map and remove the id
    for (int iCell = 0; iCell < maxCells; iCell++) {
        tCell &tCell = cell[iCell];
        if (tCell.id[iIDType] == iIndex) {
            tCell.id[iIDType] = -1;
        }
    }
}

void cMap::draw_units()
{
    //@Mira SDL2 blender
    //// @Mira fix trasnparency set_trans_blender(0, 0, 0, 160);

    // draw all worms first
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;

        // DEBUG MODE: DRAW PATHS
        if (game.m_drawUnitDebug) {
            pUnit.draw_path();
        }

        if (pUnit.iType != SANDWORM) continue;

        if (pUnit.isWithinViewport(game.m_mapViewport)) {
            pUnit.draw();
        }

        drawUnitDebug(pUnit);

    }

    // then: draw infantry units
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;

        if (!pUnit.isInfantryUnit())
            continue; // skip non-infantry units

        if (pUnit.isWithinViewport(game.m_mapViewport)) {
            // draw
            pUnit.draw();
        }

        drawUnitDebug(pUnit);
    }

    // then: draw ground units
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;

        if (pUnit.isAirbornUnit() ||
                pUnit.isSandworm() ||
                pUnit.isInfantryUnit())
            continue; // skip airborn, infantry and sandworm

        if (pUnit.isWithinViewport(game.m_mapViewport)) {
            // draw
            pUnit.draw();
        }

        drawUnitDebug(pUnit);
    }
}

void cMap::drawUnitDebug(cUnit &pUnit) const
{
    if (!game.m_drawUnitDebug) return;

    pUnit.draw_debug();
}

// draw 2nd layer for units, this is health/spice bars and eventually airborn units (last)
void cMap::draw_units_2nd()
{
    // Mira SDL2 blender
    //// @Mira fix trasnparency set_trans_blender(0, 0, 0, 160);

    // draw health of units
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;
        if (!pUnit.bHovered && !pUnit.bSelected) continue;
        if (!pUnit.isWithinViewport(game.m_mapViewport)) continue;
        if (pUnit.isHidden()) continue;

        pUnit.draw_health();
        pUnit.draw_experience();
        if (pUnit.iType == HARVESTER) {
            pUnit.draw_spice();
        }
    }

    // draw airborn units
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;
        if (!pUnit.isAirbornUnit()) continue;

        if (pUnit.isWithinViewport(game.m_mapViewport)) {
            pUnit.draw();
            // TODO: Only human players?
            pUnit.draw_health();
            if (game.isDebugMode()) {
                drawUnitDebug(pUnit);
            }
        }

    }
    // Mira SDL2 blender
    //// @Mira fix trasnparency set_trans_blender(0, 0, 0, 128);
}

int cMap::mouse_draw_x()
{
    if (players[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = players[HUMAN].getGameControlsContext()->getMouseCell();
        int absX = getAbsoluteXPositionFromCell(mouseCell);
        return mapCamera->getWindowXPosition(absX);
    }
    return -1;
}

int cMap::mouse_draw_y()
{
    if (players[HUMAN].getGameControlsContext()->getMouseCell() > -1) {
        int mouseCell = players[HUMAN].getGameControlsContext()->getMouseCell();
        int absY = getAbsoluteYPositionFromCell(mouseCell);
        return mapCamera->getWindowYPosition(absY);
    }
    return -1;
}

/**
 * Return amount of cells of a specific type. (SLOW!)
 *
 * TODO: This can be sped up quite a lot, by letting the mapEditor remember what it has placed (keep counter, or list, etc)
 *
 * @param cellType
 * @return
 */
int cMap::getTotalCountCellType(int cellType)
{
    return getAllCellsOfType(cellType).size();
}

/**
 * Returns cell nr's which are of a specific cellType
 *
 * NOTE: This is a slow method, as it iterates though all possible cells
 * @return
 */
std::vector<int> cMap::getAllCellsOfType(int cellType)
{
    std::vector<int> cellsOfType = std::vector<int>();
    for (int c = 0; c < maxCells; c++) {
        if (getCellType(c) == cellType) {
            cellsOfType.push_back(c);
        }
    }
    return cellsOfType;
}

int cMap::getCellSlowDown(int iCell)
{
    int cellType = map.getCellType(iCell);

    if (cellType == TERRAIN_SAND) return 2;
    if (cellType == TERRAIN_MOUNTAIN) return 5;
    if (cellType == TERRAIN_HILL) return 3;
    if (cellType == TERRAIN_SPICEHILL) return 3;
    if (cellType == TERRAIN_ROCK) return 1;
    if (cellType == TERRAIN_SLAB) return 0;

    return 1;
}

int cMap::findCloseMapBorderCellRelativelyToDestinationCel(int destinationCell)
{
    assert(destinationCell > -1);
    // Cell x and y coordinates
    int iCllX = getCellX(destinationCell);
    int iCllY = getCellY(destinationCell);

    // STEP 1: determine starting
    int iStartCell = -1;
    int lDistance = 9999;

    int tDistance = 9999;
    int cll = -1;

    // HORIZONTAL cells
    for (int iX = 0; iX < width; iX++) {
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
        tDistance = distance(iX, height - 1, iCllX, iCllY);

        if (tDistance < lDistance) {
            lDistance = tDistance;

            cll = makeCell(iX, height - 1);

            if (map.occupied(cll) == false) {
                iStartCell = cll;
            }
        }
    }

    // VERTICAL cells
    for (int iY = 0; iY < height; iY++) {
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
        tDistance = distance(width - 1, iY, iCllX, iCllY);

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

double cMap::distance(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2) return 1; // when all the same, distance is 1 ...

    int A = abs(x2 - x1) * abs(x2 - x1);
    int B = abs(y2 - y1) * abs(y2 - y1);
    return sqrt((double) (A + B)); // get C from A and B
}

int cMap::getCellY(int c)
{
    if (c < 0 || c >= maxCells) {
        return -1;
    }

    return (c / width);
}

int cMap::getCellX(int c)
{
    if (c < 0 || c >= maxCells) {
        return -1;
    }

    int cellX = c - ((c / width) * width);
    return cellX;
}

bool cMap::isCellAdjacentToOtherCell(int thisCell, int otherCell)
{
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

int cMap::getCellLowerRight(int c)
{
    int lowerRightCell = getCellBelow(c) + 1;
    if (lowerRightCell >= maxCells) return -1;
    if (lowerRightCell < 0) return -1;

    return lowerRightCell;
}

int cMap::getCellLowerLeft(int c)
{
    int lowerLeftCell = getCellBelow(c) - 1;
    if (lowerLeftCell < 0) return -1;
    if (lowerLeftCell >= maxCells) return -1;
    return lowerLeftCell;
}

int cMap::getCellUpperRight(int c)
{
    int upperRightCell = getCellAbove(c) + 1;
    if (upperRightCell < 0) return -1;

    return upperRightCell;
}

int cMap::getCellUpperLeft(int c)
{
    int upperLeftCell = getCellAbove(c) - 1;
    if (upperLeftCell < 0) return -1;

    return upperLeftCell;
}

int cMap::getCellRight(int c)
{
    int x = getCellX(c);
    int cellRight = x + 1;
    if (cellRight >= maxCells) return -1;
    if (cellRight >= width) return -1;

    return c + 1;
}

int cMap::getCellLeft(int c)
{
    if (c < 0) return -1;
    int x = getCellX(c);
    int cellLeft = x - 1;
    if (cellLeft < 0) return -1;
    return c - 1;
}

int cMap::getCellBelow(int c)
{
    if (c < 0) return -1;
    int cellBelow = c + width;
    if (cellBelow >= maxCells)
        return -1;

    return cellBelow;
}

int cMap::getCellAbove(int c)
{
    if (c < 0) return -1;
    int cellAbove = c - width;

    if (cellAbove < 0) return -1;

    return cellAbove;
}

int cMap::getAbsoluteYPositionFromCell(int cell)
{
    if (cell < 0) return -1;
    return getCellY(cell) * TILESIZE_HEIGHT_PIXELS;
}

int cMap::getAbsoluteXPositionFromCell(int cell)
{
    if (cell < 0) return -1;
    return getCellX(cell) * TILESIZE_WIDTH_PIXELS;
}

int cMap::getAbsoluteXPositionFromCellCentered(int cell)
{
    return getAbsoluteXPositionFromCell(cell) + (TILESIZE_WIDTH_PIXELS / 2);
}

int cMap::getAbsoluteYPositionFromCellCentered(int cell)
{
    return getAbsoluteYPositionFromCell(cell) + (TILESIZE_HEIGHT_PIXELS / 2);
}

int cMap::makeCell(int x, int y)
{
    assert(x > -1 && "makeCell x must be > -1");
    assert(x < width && "makeCell x must be < width"); // should never be higher!
    assert(y > -1 && "makeCell y must be > -1");
    assert(y < height && "makeCell y must be < height");

    // create cell
    int result = getCellWithMapDimensions(x, y);

    assert(result < maxCells); // may never be => (will since MAX_CELLS-1 is max in array!)
    assert(result > -1); // may never be < 0

    return result;
}

double cMap::distance(int cell1, int cell2)
{
    int x1 = getCellX(cell1);
    int y1 = getCellY(cell1);

    int x2 = getCellX(cell2);
    int y2 = getCellY(cell2);
    return ABS_length(x1, y1, x2, y2);
}

int cMap::getCellWithMapBorders(int x, int y)
{
    // internal vars are 1 based (ie 64x64 means 0-63, which really means 1...62 are valid)
    int maxHeight = (height - 2); // hence the -2!
    int maxWidth = (width - 2);

    if (x < 1) return -1;
    if (y < 1) return -1;
    if (x > maxWidth) return -1;
    if (y > maxHeight) return -1;

    return getCellWithMapDimensions(x, y);
}

int cMap::getCellWithMapDimensions(int x, int y)
{
    int mapWidth = width;
    int mapHeight = height;
    // (over the) boundaries result in cell -1
    if (x < 0) return -1;
    if (x >= mapWidth) return -1;
    if (y < 0) return -1;
    if (y >= mapHeight) return -1;

    return (y * mapWidth) + x;
}

bool cMap::isValidCell(int c) const
{
    return !(c < 0 || c >= maxCells);
}

/**
 * Returns a random cell, disregards playable borders
 * @return
 */
int cMap::getRandomCell()
{
    return rnd(maxCells);
}

void cMap::createCell(int cell, int terrainType, int tile)
{
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
    map.cellChangeHealth(cell, 0);

    map.cellChangePassable(cell, true);
    map.cellChangePassableFoot(cell, true);

    map.cellChangeSmudgeTile(cell, -1);
    map.cellChangeSmudgeType(cell, -1);

    if (terrainType == TERRAIN_SPICE) {
        map.cellChangeCredits(cell, 50 + rnd(125));
    }
    else if (terrainType == TERRAIN_SPICEHILL) {
        map.cellChangeCredits(cell, 75 + rnd(150));
    }
    else if (terrainType == TERRAIN_MOUNTAIN) {
        map.cellChangePassable(cell, false);
        map.cellChangePassableFoot(cell, true);
    }
    else if (terrainType == TERRAIN_WALL) {
        map.cellChangeHealth(cell, 100);
        map.cellChangePassable(cell, false);
        map.cellChangePassableFoot(cell, false);
    }
    else if (terrainType == TERRAIN_BLOOM) {
        map.cellChangeCredits(cell, -23);

        s_GameEvent event{
            .eventType = eGameEventType::GAME_EVENT_SPICE_BLOOM_SPAWNED,
            .entityType = eBuildType::SPECIAL,
            .entityID = -1,
            .player = nullptr,
            .entitySpecificType = -1,
            .atCell = cell,
            .isReinforce = false,
            .buildingListItem = nullptr,
            .buildingList = nullptr
        };

        game.onNotifyGameEvent(event);
    }
}

void cMap::clearAllCells()
{
    for (int c = 0; c < getMaxCells(); c++) {
        cellInit(c);
    }
}

bool cMap::isVisible(int iCell, cPlayer *thePlayer)
{
    if (!thePlayer) return false;
    int playerId = thePlayer->getId();
    return isVisible(iCell, playerId);
}

void cMap::resize(int width, int height)
{
    maxCells = width * height;
    this->width = width;
    this->height = height;
    cell.resize(maxCells);
}

int cMap::getRandomCellWithinMapWithSafeDistanceFromBorder(int distance)
{
    return getCellWithMapBorders(
               distance + rnd(width - (distance * 2)),
               distance + rnd(height - (distance * 2))
           );
}

bool cMap::isWithinBoundaries(int c)
{
    return isWithinBoundaries(getCellX(c), getCellY(c));
}

void cMap::setVisibleFor(int iCell, cPlayer *pPlayer)
{
    if (!pPlayer) return;
    setVisible(iCell, pPlayer->getId(), true);
}

void cMap::setVisible(int iCell, int iPlayer, bool flag)
{
    if (!isValidCell(iCell)) return;
    if (iPlayer < 0 || iPlayer >= MAX_PLAYERS) return;

    cell[iCell].iVisible[iPlayer] = flag;
}

int cMap::findNearestSpiceBloom(int iCell)
{
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
    int closestBloomFoundSoFar = -1;
    int bloomsEvaluated = 0;

    cx = map.getCellX(iCell);
    cy = map.getCellY(iCell);

    for (int i = 0; i < map.getMaxCells(); i++) {
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
    int iT = 0;

    for (int i = 0; i < map.getMaxCells(); i++) {
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

bool cMap::isValidTerrainForStructureAtCell(int cll)
{
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
int cMap::getRandomCellFrom(int cell, int distance)
{
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
int cMap::getRandomCellFromWithRandomDistance(int cell, int distance)
{
    int startX = getCellX(cell);
    int startY = getCellY(cell);
    int newX = (startX - distance) + (rnd(distance * 2));
    int newY = (startY - distance) + (rnd(distance * 2));
    return getCellWithMapBorders(newX, newY);
}

/**
 * Takes structure, evaluates all its cells, and if any of these are visible, this function returns true.
 *
 * @param pStructure
 * @param thePlayer (for who it should be visible)
 * @return
 */
bool cMap::isStructureVisible(cAbstractStructure *pStructure, cPlayer *thePlayer)
{
    if (!thePlayer) return false;
    return isStructureVisible(pStructure, thePlayer->getId());
}

/**
 * Takes structure, evaluates all its cells, and if any of these are visible, this function returns true.
 *
 * @param pStructure
 * @param iPlayer
 * @return
 */
bool cMap::isStructureVisible(cAbstractStructure *pStructure, int iPlayer)
{
    if (!pStructure) return false;
    if (!pStructure->isValid()) return false;

    // iterate over all cells of structure
    const std::vector<int> &cells = pStructure->getCellsOfStructure();
    for (auto &cll: cells) {
        if (map.isVisible(cll, iPlayer)) {
            return true;
        }
    }

    return false;
}

bool cMap::isAtMapBoundaries(int cell)
{
    bool validCell = isValidCell(cell);
    if (!validCell) return false;

    int maxHeight = (height - 2); // hence the -2!
    int maxWidth = (width - 2);

    int x = getCellX(cell);
    int y = getCellY(cell);

    if (x == 1 || x == maxWidth) return true;
    if (y == 1 || y == maxHeight) return true;

    return false;
}

cPoint cMap::fixCoordinatesToBeWithinPlayableMap(int x, int y) const
{
    return {std::clamp(x, 1, getWidth() - 2), std::clamp(y, 1, getHeight() - 2)};
}

cPoint cMap::fixCoordinatesToBeWithinMap(int x, int y) const
{
    return {std::clamp(x, 0, getWidth() - 1), std::clamp(y, 0, getHeight() - 1)};
}

int cMap::findNearByValidDropLocation(int cell, int minRange, int range, int unitTypeToDrop)
{

    if (minRange < 1) {
        minRange = 1;
    }

    // go around 360 fDegrees and calculate new stuff.
    for (float dr = minRange; dr < range; dr++) { // go outwards
        for (float d = 0; d < 360; d++) { // if we reduce the amount of degrees, we don't get full coverage.
            // need a smarter way to do this (less CPU intensive).

            int x = map.getAbsoluteXPositionFromCellCentered(cell);
            int y = map.getAbsoluteYPositionFromCellCentered(cell);

            float dr1 = cos(d) * (dr * TILESIZE_WIDTH_PIXELS);
            float dr2 = sin(d) * (dr * TILESIZE_HEIGHT_PIXELS);

            x = (x + dr1);
            y = (y + dr2);

            // convert back
            int cl = mapCamera->getCellFromAbsolutePosition(x, y);

            if (cl < 0) continue;
            if (!map.isWithinBoundaries(cl)) continue;

            if (map.canDeployUnitTypeAtCell(cl, unitTypeToDrop)) {
                return cl;
            }
        }
    }
    return -1;
}

int cMap::findNearByValidDropLocation(int cell, int range, int unitTypeToDrop)
{
    return findNearByValidDropLocation(cell, 1, range, unitTypeToDrop);
}

int cMap::findNearByValidDropLocationForUnit(int cell, int range, int unitIDToDrop)
{
    // go around 360 fDegrees and calculate new stuff.
    for (float dr = 1; dr < range; dr++) { // go outwards
        for (float d = 0; d < 360; d++) { // if we reduce the amount of degrees, we don't get full coverage.
            // need a smarter way to do this (less CPU intensive).

            int x = map.getAbsoluteXPositionFromCellCentered(cell);
            int y = map.getAbsoluteYPositionFromCellCentered(cell);

            float dr1 = cos(d) * (dr * TILESIZE_WIDTH_PIXELS);
            float dr2 = sin(d) * (dr * TILESIZE_HEIGHT_PIXELS);

            x = (x + dr1);
            y = (y + dr2);

            // convert back
            int cl = mapCamera->getCellFromAbsolutePosition(x, y);

            if (cl < 0) continue;

            if (map.canDeployUnitAtCell(cell, unitIDToDrop)) {
                return cell;
            }
        }
    }
    return -1;
}

/**
 * Scans structures, belonging to player. If it is the same type it will be returned.
 * @param player
 * @param structureType
 * @return
 */
cAbstractStructure *cMap::findClosestStructureType(int cell, int structureType, cPlayer *player)
{
    assert(player);
    assert(structureType > -1);
    assert(isValidCell(cell));

    int foundStructureId = -1;    // found structure id
    long shortestDistance = 9999; // max distance to search in

    const std::vector<int> &myStructuresAsId = player->getAllMyStructuresAsId();
    for (auto &i: myStructuresAsId) {
        cAbstractStructure *pStructure = structure[i];
        if (pStructure == nullptr) continue;
        if (pStructure->getType() != structureType) continue;

        long distance = map.distance(cell, pStructure->getCell());

        // if distance is lower than last found distance, it is the closest for now.
        if (distance < shortestDistance) {
            foundStructureId = i;
            shortestDistance = distance;
        }
    }

    if (foundStructureId > -1) {
        return structure[foundStructureId];
    }

    return nullptr;
}

void cMap::cellTakeDamage(int cellNr, int damage)
{
    tCell *pCell = getCell(cellNr);
    if (pCell) {
        pCell->health -= damage;

        if (pCell->health < -25) {
            if (pCell->type == TERRAIN_ROCK) {
                smudge_increase(SMUDGE_ROCK, cellNr);
            }

            if (pCell->type == TERRAIN_SAND ||
                    pCell->type == TERRAIN_HILL ||
                    pCell->type == TERRAIN_SPICE ||
                    pCell->type == TERRAIN_SPICEHILL) {
                smudge_increase(SMUDGE_SAND, cellNr);
            }

            pCell->health += rnd(35);
        }
    }
}

void cMap::cellChangeType(int cellNr, int type)
{
    tCell *pCell = getCell(cellNr);
    if (pCell) {
        if (type > TERRAIN_WALL) {
            pCell->type = type;
        }
        else if (type < TERRAIN_BLOOM) {
            pCell->type = type;
        }
        else {
            pCell->type = type;
        }
    }
}

void cMap::cellInit(int cellNr)
{
    tCell *pCell = getCell(cellNr);
    if (!pCell) return; // bail

    pCell->credits = 0;
    pCell->health = 0;
    pCell->passable = true;
    pCell->passableFoot = true;
    pCell->tile = 0;
    pCell->type = TERRAIN_SAND;    // refers to gfxdata!

    pCell->smudgetile = -1;
    pCell->smudgetype = -1;

    // clear out the ID stuff
    memset(pCell->id, -1, sizeof(pCell->id));
    memset(pCell->iVisible, 0, sizeof(pCell->iVisible));

    // for (int i = 0; i < MAX_PLAYERS; i++) {
    //     setVisible(cellNr, i, false);
    // }
}

cAbstractStructure *cMap::findClosestAvailableStructureType(int cell, int structureType, cPlayer *pPlayer)
{
    assert(pPlayer);
    assert(structureType > -1);
    assert(isValidCell(cell));

    int foundStructureId = -1;    // found structure id
    long shortestDistance = 9999; // max distance to search in

    const std::vector<int> &myStructuresAsId = pPlayer->getAllMyStructuresAsId();
    for (auto &i: myStructuresAsId) {
        cAbstractStructure *pStructure = structure[i];
        if (pStructure == nullptr) continue;
        if (pStructure->getType() != structureType) continue;
        if (pStructure->hasUnitWithin()) continue; // already occupied

        long distance = map.distance(cell, pStructure->getCell());

        // if distance is lower than last found distance, it is the closest for now.
        if (distance < shortestDistance) {
            foundStructureId = i;
            shortestDistance = distance;
        }
    }

    if (foundStructureId > -1) {
        return structure[foundStructureId];
    }

    return nullptr;
}

cAbstractStructure *
cMap::findClosestAvailableStructureTypeWhereNoUnitIsHeadingTo(int cell, int structureType, cPlayer *pPlayer)
{
    assert(pPlayer);
    assert(structureType > -1);
    assert(isValidCell(cell));

    int foundStructureId = -1;    // found structure id
    long shortestDistance = 9999; // max distance to search in

    int playerId = pPlayer->getId();

    const std::vector<int> &myStructuresAsId = pPlayer->getAllMyStructuresAsId();
    for (auto &i: myStructuresAsId) {
        cAbstractStructure *pStructure = structure[i];
        if (pStructure == nullptr) continue;
        if (pStructure->getOwner() != playerId) continue;
        if (pStructure->getType() != structureType) continue;
        if (pStructure->hasUnitWithin()) continue; // already occupied

        if (!pStructure->hasUnitHeadingTowards()) {    // no other unit is heading to this structure
            long distance = map.distance(cell, pStructure->getCell());

            // if distance is lower than last found distance, it is the closest for now.
            if (distance < shortestDistance) {
                foundStructureId = i;
                shortestDistance = distance;
            }
        }
    }

    if (foundStructureId > -1) {
        return structure[foundStructureId];
    }

    return nullptr;
}

int cMap::getRandomCellFromWithRandomDistanceValidForUnitType(int cell, int minRange, int maxRange, int unitType)
{
    return findNearByValidDropLocation(cell, minRange, maxRange, unitType);
}

bool cMap::isCellPassableForWorm(int cell)
{
    int cellType = getCellType(cell);
    return cellType == TERRAIN_SAND ||
           cellType == TERRAIN_HILL ||
           cellType == TERRAIN_SPICE ||
           cellType == TERRAIN_SPICEHILL;
}

bool cMap::isValidTerrainForConcreteAtCell(int cell)
{
    if (!isValidCell(cell)) return false;
    int cellType = getCellType(cell);
    // TODO: make this a flag in the cell/terrain type kind? (there is no such thing yet, its all hard-coded)
    bool allowedCellType = (cellType == TERRAIN_SLAB || cellType == TERRAIN_ROCK);
    if (!allowedCellType) {
        return false;
    }

    return true;
}

/**
 * Returns true if map is wider or higher than 64 cells
 * @return
 */
bool cMap::isBigMap()
{
    return getWidth() > 64 || getHeight() > 64;
}

void cMap::detonateSpiceBloom(int cell)
{
    if (!isValidCell(cell)) return;
    int cellTypeAtCell = getCellType(cell);
    if (cellTypeAtCell != TERRAIN_BLOOM) return;

    // change type of terrain to sand
    auto mapEditor = cMapEditor(*this);
    mapEditor.createCell(cell, TERRAIN_SAND, 0);
    int size = 75 + (rnd(100));
    mapEditor.createRandomField(cell, TERRAIN_SPICE, size);
    game.shakeScreen(20);

    s_GameEvent event{
        .eventType = eGameEventType::GAME_EVENT_SPICE_BLOOM_BLEW,
        .entityType = eBuildType::SPECIAL,
        .entityID = -1,
        .player = nullptr,
        .entitySpecificType = -1,
        .atCell = cell,
        .isReinforce = false,
        .buildingListItem = nullptr,
        .buildingList = nullptr
    };
    game.onNotifyGameEvent(event);

}

void cMap::onNotifyGameEvent(const s_GameEvent &event)
{
    switch (event.eventType) {
        case eGameEventType::GAME_EVENT_SPICE_BLOOM_SPAWNED:
            if (m_bAutoDetonateSpiceBlooms) {
                // 1000/5 = taking care of thinkFast 5ms loop. The second part
                // is the actual amount of seconds we want to delay before blowing up the spice bloom
                m_mBloomTimers[event.atCell] = (1000 / 5) * (45 + rnd(120));
            }
            break;
        case eGameEventType::GAME_EVENT_SPICE_BLOOM_BLEW:
            if (m_bAutoDetonateSpiceBlooms) {
                m_mBloomTimers.erase(event.atCell);
            }
            break;
        case eGameEventType::GAME_EVENT_DESTROYED:
            onEntityDestroyed(event);
            break;
        case eGameEventType::GAME_EVENT_CREATED:
            onEntityCreated(event);
            break;
        default:
            break;
    }
}

void cMap::onEntityCreated(const s_GameEvent &event)
{
    if (event.entityType != eBuildType::UNIT) return;

    // only care about units
    if (event.entitySpecificType == SANDWORM) {
        evaluateIfWeShouldSetTimerToRespawnWorm();
    }
}

void cMap::evaluateIfWeShouldSetTimerToRespawnWorm()
{
    int currentAmountOfWorms = players[AI_WORM].getAmountOfUnitsForType(SANDWORM);

    // as long as we don't have the desired amount, set respawn timer
    if (currentAmountOfWorms < m_iDesiredAmountOfWorms) {
        setSandwormRespawnTimer();
    }
    else {
        // we spawned a worm and got to the total amount, so set timer to -1
        // until a worm has been destroyed (either by enemy units or by withdrawal of worm)
        m_iTIMER_respawnSandworms = -1;
        if (game.isDebugMode()) {
            logbook(fmt::format("cMap::evaluateIfWeShouldSetTimerToRespawnWorm set m_iTIMER_respawnSandworms to -1, because current amount sandworms ({}) == desired amount ({})",
                                currentAmountOfWorms, m_iDesiredAmountOfWorms));
        }
    }
}

void cMap::setSandwormRespawnTimer()
{
    //only set timer when it has been < 0 (not set already)
    if (m_iTIMER_respawnSandworms < 0) {
        // 1000/5 = taking care of thinkFast 5ms loop. The second part
        // is the actual amount of seconds we want to delay

        // give at least a minute (max 3) without that sandworm
        m_iTIMER_respawnSandworms = (1000 / 5) * (60 + rnd(180));

        if (game.isDebugMode()) {
            logbook(fmt::format("cMap::setSandwormRespawnTimer set timer to {}", this->m_iTIMER_respawnSandworms));
        }
    }
    else {
        if (game.isDebugMode()) {
            logbook(fmt::format("cMap::setSandwormRespawnTimer did not change value because timer was already set (value = {})",
                                this->m_iTIMER_respawnSandworms));
        }
    }
}

void cMap::setDesiredAmountOfWorms(int value)
{
    if (game.isDebugMode()) {
        logbook(fmt::format("cMap::setDesiredAmountOfWorms changed value from {} to {}", this->m_iDesiredAmountOfWorms, value));
    }
    m_iDesiredAmountOfWorms = value;
    evaluateIfWeShouldSetTimerToRespawnWorm();
}

void cMap::onEntityDestroyed(const s_GameEvent &event)
{
    if (event.entityType != eBuildType::UNIT) return;

    // only care about units
    if (event.entitySpecificType == SANDWORM) {
        // a sandworm got destroyed, set timer to re-spawn it
        setSandwormRespawnTimer();
    }
}

cPoint cMap::getAbsolutePositionFromCell(int cell)
{
    return cPoint(getAbsoluteXPositionFromCell(cell), getAbsoluteYPositionFromCell(cell));
}
