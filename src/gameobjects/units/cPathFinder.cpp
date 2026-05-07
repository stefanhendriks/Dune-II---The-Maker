#include "gameobjects/units/cPathFinder.h"
#include "gameobjects/units/cUnit.h"
#include "gameobjects/players/cPlayers.h"
#include "gameobjects/map/cMap.h"
#include "data/gfxdata.h"
#include "utils/common.h"
#include "utils/RNG.hpp"
#include "gameobjects/map/MapGeometry.hpp"
#include "utils/d2tm_math.h"

#include "game/cGameSettings.h"
#include "controls/cGameControlsContext.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "include/sGameServices.h"

#include <algorithm>
#include <cassert>

static const int MAX_PATH_LOCAL_SIZE = 4096;

#define UNVISITED      -2
#define CLOSED        -1
#define OPEN          0

cPathFinder::cPathFinder()
{
    m_tempPath.resize(MAX_PATH_LOCAL_SIZE);
}

void cPathFinder::resize(int newSize)
{
    m_pathMap.resize(newSize);
    std::fill(m_tempPath.begin(), m_tempPath.end(), -1);
}

void cPathFinder::serviceInit(sGameServices* services)
{
    assert(services != nullptr);
    m_log = services->m_log;
    assert(m_log != nullptr);
    m_infos = services->info;
    assert(m_infos != nullptr);
    m_settings = services->settings;
    assert(m_settings != nullptr);
    m_objects = services->objects;
    assert(m_objects != nullptr);
    m_map = &m_objects->getMap();
    assert(m_map != nullptr);
    m_mapGeometry = m_objects->getMapGeometry();
    assert(m_mapGeometry != nullptr);
}

int cPathFinder::validateCreatePathInput(int unitId)
{
    // Input guardrails: only run path creation for a valid unit,
    // aligned on a cell, and within an acceptable pathfinding load window.
    logbook("CREATE_PATH -- START");
    if (unitId < 0) {
        logbook("CREATE_PATH -- END (unitId < 0)");
        return -99;
    }

    m_activeUnit = m_objects->getUnit(unitId);
    if (m_activeUnit == nullptr) {
        logbook("CREATE_PATH -- END (no unit unitId found)");
        return -99;
    }
    if (!m_activeUnit->isValid() || m_activeUnit->isDead()) {
        logbook("CREATE_PATH -- END (unitId isDead or not Valid)");
        return -99;
    }

    if (m_activeUnit->isMovingBetweenCells()) {
        m_activeUnit->log("CREATE_PATH -- END 2");
        return -4;
    }

    if (m_pathsCreated > 40) {
        m_activeUnit->log("CREATE_PATH -- END 3");
        m_activeUnit->movewaitTimer.reset(50 + RNG::rnd(50));
        return -3;
    }

    m_currentCell = m_activeUnit->getCell();

    if (m_currentCell == m_activeUnit->movement.iGoalCell) {
        m_activeUnit->log("CREATE_PATH -- END 4");
        m_activeUnit->log("ODD: The goal = cell?");
        return -1;
    }

    if (m_activeUnit->isUnableToMove()) {
        m_activeUnit->log("CREATE_PATH -- END 5");
        m_activeUnit->movewaitTimer.reset(30 + RNG::rnd(50));
        return -2;
    }

    return 0;
}

void cPathFinder::initializeCreatePathSearch(int iPathCountUnits)
{
    memset(m_activeUnit->movement.iPath, -1, sizeof(m_activeUnit->movement.iPath));
    m_activeUnit->movement.iPathIndex = -1;

    m_goalCell = m_activeUnit->movement.iGoalCell;
    m_controller = m_activeUnit->iPlayer;
    m_pathCountUnits = iPathCountUnits;

    m_valid = true;
    m_success = false;

    m_pathsCreated++;
    // Reset the search map (cost + parent + per-cell state).
    for (auto &cell : m_pathMap) {
        cell.cost = -1;
        cell.parent = -1;
        cell.state = UNVISITED;
    }

    int cx = m_mapGeometry->getCellX(m_currentCell);
    int cy = m_mapGeometry->getCellY(m_currentCell);

    m_pathMap[m_currentCell].cost = ABS_length(cx, cy,
                                               m_mapGeometry->getCellX(m_goalCell),
                                               m_mapGeometry->getCellY(m_goalCell));
    m_pathMap[m_currentCell].parent = -1;
    m_pathMap[m_currentCell].state = OPEN;
}

void cPathFinder::executeCreatePathSearch()
{
    // Local greedy search with backtracking: pick the best unvisited neighbor,
    // and walk back to the parent as soon as a dead end is reached.
    while (m_valid) {
        if (m_currentCell == m_goalCell) {
            m_activeUnit->log(std::format("WARNING: iCell == goal_cell ({}) at loop start - should have been caught earlier", m_currentCell));
            m_valid = false;
            m_success = true;
            break;
        }

        int idOfStructureAtCell = m_map->cellGetIdFromLayer(m_currentCell, MAPID_STRUCTURES);
        if (m_activeUnit->iStructureID > -1) {
            if (idOfStructureAtCell == m_activeUnit->iStructureID) {
                m_valid = false;
                m_success = true;
                m_activeUnit->log("Found structure ID");
                break;
            }
        }

        if (m_activeUnit->combat.iAttackStructure > -1) {
            if (idOfStructureAtCell == m_activeUnit->combat.iAttackStructure) {
                m_valid = false;
                m_success = true;
                m_activeUnit->log("Found attack structure ID");
                break;
            }
        }

        int cx = m_mapGeometry->getCellX(m_currentCell);
        int cy = m_mapGeometry->getCellY(m_currentCell);

        int sx = cx - 1;
        int sy = cy - 1;

        int ex = cx + 1;
        int ey = cy + 1;

        cPoint::split(sx, sy) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(sx, sy);
        cPoint::split(ex, ey) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(ex, ey);

        double cost = 999999999;
        int the_cll = -1;

        bool bail_out = false;

        // Explore the 3x3 neighborhood around the current cell.
        for (cx = sx; cx <= ex; cx++) {
            for (cy = sy; cy <= ey; cy++) {
                int cll = m_mapGeometry->getCellWithMapBorders(cx, cy);

                if (cll < 0)
                    continue;

                if (cll == m_currentCell)
                    continue;

                bool good = false;

                int cellType = m_map->getCellType(cll);
                if (!m_activeUnit->isSandworm()) {
                    // For regular units, traversability depends on occupancy
                    // (units/structures), visibility, and terrain.
                    int idOfUnitAtCell = m_map->getCellIdUnitLayer(cll);
                    int idOfStructureAtCellNearby = m_map->getCellIdStructuresLayer(cll);

                    if (idOfUnitAtCell == -1 && idOfStructureAtCellNearby == -1) {
                        good = true;
                    }

                    if (idOfStructureAtCellNearby > -1) {
                        if (m_activeUnit->combat.iAttackStructure > -1)
                            if (idOfStructureAtCellNearby == m_activeUnit->combat.iAttackStructure)
                                good = true;

                        if (m_activeUnit->iStructureID > -1)
                            if (idOfStructureAtCellNearby == m_activeUnit->iStructureID)
                                good = true;

                    }

                    if (idOfUnitAtCell > -1) {
                        if (idOfUnitAtCell != m_activeUnit->iID) {
                            int iUID = idOfUnitAtCell;

                            if (m_pathCountUnits <= 0) {
                                good = false;
                                m_activeUnit->log("iPathCountUnits <= 0 - variable 'good' becomes 'false'");
                            }

                            cUnit *unitAtCell = m_objects->getUnit(iUID);
                            if (!unitAtCell->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer())) {
                                if (unitAtCell->isInfantryUnit() &&
                                    m_activeUnit->canSquishInfantry())
                                    good = true;
                            }
                        }
                        else {
                            good = true;
                        }
                    }

                    if (m_map->isVisible(cll, m_controller) == false) {
                        good = true;
                    }
                    else {
                        if (cellType == TERRAIN_WALL) {
                            good = false;
                        }

                        if (!m_activeUnit->isInfantryUnit()) {
                            if (cellType == TERRAIN_MOUNTAIN) {
                                good = false;
                            }
                        }
                    }
                }
                else {
                    good = m_map->isCellPassableForWorm(cll);
                }

                if (!good) {
                    continue;
                }

                if (cll == m_goalCell) {
                    the_cll = cll;
                    cost = 0;
                    bail_out = true;
                    m_activeUnit->log("CREATE_PATH: Found the goal cell, success, bailing out");
                    break;
                }

                bool isUnvisited = m_pathMap[cll].state == UNVISITED;

                if (cll != m_currentCell &&
                    isUnvisited) {
                    int gcx = m_mapGeometry->getCellX(m_goalCell);
                    int gcy = m_mapGeometry->getCellY(m_goalCell);

                    int tempCost = (m_pathMap[cll].cost >= 0) ? m_pathMap[cll].cost : 0;
                    double distanceCost = m_mapGeometry->distance(cx, cy, gcx, gcy);
                    double newCost = distanceCost + tempCost;

                    if (newCost < cost) {
                        // Heuristic: keep the neighbor that minimizes distance to the goal.
                        the_cll = cll;
                        cost = newCost;
                        m_pathCountUnits--;
                    }
                }
            }

            if (bail_out) {
                m_activeUnit->log("CREATE_PATH: BAIL");
                break;
            }
        }

        if (the_cll > -1) {
            // Move forward: the chosen neighbor becomes the new current cell.
            m_pathMap[the_cll].state = OPEN;
            m_pathMap[the_cll].parent = m_currentCell;
            m_pathMap[the_cll].cost = cost;

            m_currentCell = the_cll;
            if (m_currentCell == m_goalCell) {
                m_valid = false;
                m_success = true;
            }

        }
        else {
            // Dead end: close the current cell and backtrack to its parent.
            m_pathMap[m_currentCell].state = CLOSED;
            int prevCell = m_pathMap[m_currentCell].parent;

            if (prevCell > -1) {
                m_currentCell = prevCell;
            } else {
                m_activeUnit->log("Failed to find new cell, backtracking failed - no parent!");
                m_valid = false;
                m_success = false;
                m_activeUnit->log("FAILED TO CREATE PATH - nothing found to continue");
                break;
            }
        }
    }
}

int cPathFinder::backtracePathToTempBuffer()
{
    // Walk the parent chain from the final position back to the start.
    std::fill(m_tempPath.begin(), m_tempPath.end(), -1);

    bool cp = true;

    int sc = m_currentCell;
    int pi = 0;
    m_tempPath[pi] = sc;
    pi++;

    m_activeUnit->log(std::format("Starting backtracing. Path index = {}, temp_path[0] = {}", pi, m_tempPath[pi - 1]));

    while (cp) {
        int tmp = m_pathMap[sc].parent;
        if (tmp > -1) {
            if (tmp == sc) {
                m_activeUnit->log("found terminator, stop!");
                cp = false;
                continue;
            }
            else {
                m_tempPath[pi] = tmp;
                sc = m_pathMap[sc].parent;
                pi++;

                if (pi >= static_cast<int>(m_tempPath.size())) {
                    std::string msg = std::format("WARNING: backtrace truncated - path exceeds MAX_PATH_LOCAL_SIZE ({})", m_tempPath.size());
                    m_activeUnit->log(msg);
                    cp = false;
                    continue;
                }

                m_activeUnit->log(std::format("Backtraced. Path index = {}, temp_path[last] = {}", pi, m_tempPath[pi - 1]));
            }
        }
        else {
            std::string msg = std::format("WARNING: backtrace stopped at cell {} - parent is -1 (no parent found)", sc);
            m_activeUnit->log(msg);
            cp = false;
        }

        if (sc == m_activeUnit->getCell()) {
            cp = false;
        }
    }

    return pi;
}

void cPathFinder::applyTempPathToUnit(int backtraceLength)
{
    // Reverse the backtraced path and compact intermediate points when
    // a farther cell is still adjacent to the previous selected cell.
    int z = backtraceLength - 1;
    int a = 0;
    int iPrevCell = -1;

    while (z > -1) {
        if (m_tempPath[z] > -1) {
            if (iPrevCell > -1) {
                int iGoodZ = -1;

                for (int sz = z; sz > 0; sz--) {
                    if (m_tempPath[sz] > -1) {

                        if (m_mapGeometry->isCellAdjacentToOtherCell(iPrevCell, m_tempPath[sz])) {
                            iGoodZ = sz;
                        }
                    }
                    else {
                        break;
                    }
                }

                if (iGoodZ < z && iGoodZ > -1) {
                    z = iGoodZ;
                }
            }

            if (a >= MAX_PATH_SIZE) {
                m_activeUnit->log(std::format("WARNING: path truncated - exceeds MAX_PATH_SIZE ({})", MAX_PATH_SIZE));
                break;
            }
            m_activeUnit->movement.iPath[a] = m_tempPath[z];
            iPrevCell = m_tempPath[z];
            a++;
        }
        z--;
    }

    m_activeUnit->movement.iPathIndex = 1;

    for (int i = 1; i < MAX_PATH_SIZE; i++) {
        int pathCell = m_activeUnit->movement.iPath[i];
        if (pathCell > -1) {
            if (m_mapGeometry->isCellAdjacentToOtherCell(m_activeUnit->getCell(), pathCell)) {
                m_activeUnit->movement.iPathIndex = i;
            }
        }
    }
}

int cPathFinder::createPath(int iUnitId, int iPathCountUnits)
{
    // Full pipeline: validate, search, rebuild, then apply to unit movement.
    int validationResult = validateCreatePathInput(iUnitId);
    if (validationResult != 0) {
        return validationResult;
    }

    initializeCreatePathSearch(iPathCountUnits);
    executeCreatePathSearch();
    if (!m_success) {
        m_activeUnit->log("CREATE_PATH -- not valid");
        m_activeUnit->log("CREATE_PATH: Failed to create path!");
        return -1;
    }
    m_activeUnit->log("CREATE_PATH -- valid loop finished");

    m_activeUnit->log("CREATE_PATH -- pathfinder got to goal-cell. Backtracing ideal path.");
    int backtraceLength = backtracePathToTempBuffer();
    applyTempPathToUnit(backtraceLength);

    if (m_settings->isDebugMode()) {
        for (int i = 0; i < MAX_PATH_SIZE; i++) {
            int pathCell = m_activeUnit->movement.iPath[i];
            if (pathCell > -1) {
                m_activeUnit->log(std::format("WAYPOINT {} = {} ", i, pathCell));
            }
        }
    }

    m_activeUnit->updateCellXAndY();
    m_activeUnit->movement.bCalculateNewPath = false;

    return 0;
}

int cPathFinder::returnCloseGoal(int iCll, int iMyCell, int iID)
{
    // Find a free cell near iCll by gradually expanding the search radius,
    // then pick the one closest to iMyCell.
    int iSize = 1;
    int iStartX = m_mapGeometry->getCellX(iCll) - iSize;
    int iStartY = m_mapGeometry->getCellY(iCll) - iSize;
    int iEndX = m_mapGeometry->getCellX(iCll) + iSize;
    int iEndY = m_mapGeometry->getCellY(iCll) + iSize;

    float dDistance = 9999;

    int ix = m_mapGeometry->getCellX(iMyCell);
    int iy = m_mapGeometry->getCellY(iMyCell);

    bool bSearch = true;

    int iTheClosest = -1;

    while (bSearch) {
        iStartX = m_mapGeometry->getCellX(iCll) - iSize;
        iStartY = m_mapGeometry->getCellY(iCll) - iSize;
        iEndX = m_mapGeometry->getCellX(iCll) + iSize;
        iEndY = m_mapGeometry->getCellY(iCll) + iSize;

        cPoint::split(iStartX, iStartY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(iStartX, iStartY);
        cPoint::split(iEndX, iEndY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(iEndX, iEndY);

        for (int iSX = iStartX; iSX < iEndX; iSX++)
            for (int iSY = iStartY; iSY < iEndY; iSY++) {
                int cll = m_mapGeometry->getCellWithMapDimensions(iSX, iSY);

                float dDistance2 = ABS_length(iSX, iSY, ix, iy);

                int idOfStructureAtCell = m_map->getCellIdStructuresLayer(cll);
                int idOfUnitAtCell = m_map->getCellIdUnitLayer(cll);

                if ((idOfStructureAtCell < 0) && (idOfUnitAtCell < 0)) {
                    int cellType = m_map->getCellType(cll);
                    if (m_infos->getUnitInfo(m_objects->getUnit(iID)->iType).infantry) {
                        if (cellType == TERRAIN_MOUNTAIN)
                            continue;
                    }

                    if (cellType == TERRAIN_WALL)
                        continue;

                    if (dDistance2 < dDistance) {
                        dDistance = dDistance2;
                        iTheClosest = cll;
                    }
                }
            }

        if (iTheClosest > -1)
            return iTheClosest;

        iSize++;

        if (iSize > 9) {
            bSearch = false;
            break;
        }
    }

    return iCll;
}

void cPathFinder::resetPathCreatedByUnit()
{
    m_pathsCreated = 0;
}