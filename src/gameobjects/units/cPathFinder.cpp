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
//#include <set>
//#include <iostream> 

static const int MAX_PATH_LOCAL_SIZE = 4096;

// Path creation definitions / var
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

int cPathFinder::validateCreatePathInput(int iUnitId)
{
    logbook("CREATE_PATH -- START");
    if (iUnitId < 0) {
        logbook("CREATE_PATH -- END (iUnitId < 0)");
        return -99; // Wut!?
    }

    m_activeUnit = m_objects->getUnit(iUnitId);
    if (m_activeUnit == nullptr) {
        logbook("CREATE_PATH -- END (no unit iUnitId found)");
        return -99; // Wut!?
    }
    if (!m_activeUnit->isValid() || m_activeUnit->isDead()) {
        logbook("CREATE_PATH -- END (iUnitId isDead or not Valid)");
        return -99; // for now...
    }

    // do not start calculating anything before we are on 0,0 x,y wise on a cell
    if (m_activeUnit->isMovingBetweenCells()) {
        m_activeUnit->log("CREATE_PATH -- END 2");
        return -4; // no calculation before we are straight on a cell
    }

    // Too many paths where created , so we wait a little.
    // make sure not to create too many paths at once
    if (m_pathsCreated > 40) {
        m_activeUnit->log("CREATE_PATH -- END 3");
        m_activeUnit->movewaitTimer.reset(50 + RNG::rnd(50));
        return -3;
    }

    m_currentCell = m_activeUnit->getCell(); // current cell

    // When the goal == cell, then skip.
    if (m_currentCell == m_activeUnit->movement.iGoalCell) {
        m_activeUnit->log("CREATE_PATH -- END 4");
        m_activeUnit->log("ODD: The goal = cell?");
        return -1;
    }

    // when all around the unit there is no space, dont even bother
    if (m_activeUnit->isUnableToMove()) {
        m_activeUnit->log("CREATE_PATH -- END 5");
        m_activeUnit->movewaitTimer.reset(30 + RNG::rnd(50));
        return -2;
    }

    return 0;
}

void cPathFinder::initializeCreatePathSearch(int iPathCountUnits)
{
    // Clear unit path settings (index & path string)
    memset(m_activeUnit->movement.iPath, -1, sizeof(m_activeUnit->movement.iPath));
    m_activeUnit->movement.iPathIndex = -1;

    m_goalCell = m_activeUnit->movement.iGoalCell;
    m_controller = m_activeUnit->iPlayer;
    m_pathCountUnits = iPathCountUnits;

    m_valid = true;
    m_success = false;

    m_pathsCreated++;
    for (auto &cell : m_pathMap) {
        cell.cost = -1;
        cell.parent = -1;
        cell.state = UNVISITED;
    }

    int cx = m_mapGeometry->getCellX(m_currentCell);
    int cy = m_mapGeometry->getCellY(m_currentCell);

    // set very first... our start cell
    m_pathMap[m_currentCell].cost = ABS_length(cx, cy,
                                               m_mapGeometry->getCellX(m_goalCell),
                                               m_mapGeometry->getCellY(m_goalCell));
    m_pathMap[m_currentCell].parent = -1;
    m_pathMap[m_currentCell].state = OPEN; // this one is opened by default
}

void cPathFinder::executeCreatePathSearch()
{
    // WHILE VALID TO RUN THIS LOOP
    while (m_valid) {
        // iCell reached Goal Cell. We should have bailed out sooner.
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

        // starting position is cx-1 and cy-1
        int sx = cx - 1;
        int sy = cy - 1;

        // check for not going under zero
        int ex = cx + 1;
        int ey = cy + 1;

        // boundaries
        cPoint::split(sx, sy) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(sx, sy);
        cPoint::split(ex, ey) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(ex, ey);

        double cost = 999999999;
        int the_cll = -1;

        // go around the cell we are checking!
        bool bail_out = false;

        // circle around cell X wise
        for (cx = sx; cx <= ex; cx++) {
            // circle around cell Y wise
            for (cy = sy; cy <= ey; cy++) {
                // only check the 'cell' that is NOT the current cell.
                int cll = m_mapGeometry->getCellWithMapBorders(cx, cy);

                // skip invalid cells
                if (cll < 0)
                    continue;

                // DO NOT CHECK SELF
                if (cll == m_currentCell)
                    continue;

                // Determine if it is a good cell to use or not:
                bool good = false; // not good by default

                // not a sandworm
                int cellType = m_map->getCellType(cll);
                if (!m_activeUnit->isSandworm()) {
                    // Step by step determine if its good
                    // 2 fases:
                    // 1 -> Occupation by unit/structures
                    // 2 -> Occupation by terrain (but only when it is visible, since we do not want to have an
                    //      advantage or some super intelligence by units for unknown territories!)
                    int idOfUnitAtCell = m_map->getCellIdUnitLayer(cll);
                    int idOfStructureAtCellNearby = m_map->getCellIdStructuresLayer(cll);

                    if (idOfUnitAtCell == -1 && idOfStructureAtCellNearby == -1) {
                        // there is nothing on this cell, that is good
                        good = true;
                    }

                    if (idOfStructureAtCellNearby > -1) {
                        // when the cell is a structure, and it is the structure we want to attack, it is good

                        if (m_activeUnit->combat.iAttackStructure > -1)
                            if (idOfStructureAtCellNearby == m_activeUnit->combat.iAttackStructure)
                                good = true;

                        if (m_activeUnit->iStructureID > -1)
                            if (idOfStructureAtCellNearby == m_activeUnit->iStructureID)
                                good = true;

                    }

                    // blocked by other than our own unit
                    if (idOfUnitAtCell > -1) {
                        // occupied by a different unit than ourselves
                        if (idOfUnitAtCell != m_activeUnit->iID) {
                            int iUID = idOfUnitAtCell;

                            if (m_pathCountUnits <= 0) {
                                // other units block our path
                                good = false;
                                m_activeUnit->log("iPathCountUnits <= 0 - variable 'good' becomes 'false'");
                            }

                            cUnit *unitAtCell = m_objects->getUnit(iUID);
                            if (!unitAtCell->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer())) {
                                // allow running over enemy infantry/squishable units
                                if (unitAtCell->isInfantryUnit() &&
                                    m_activeUnit->canSquishInfantry()) // and the current unit can squish
                                    good = true; // its infantry we want to run over, so don't be bothered!
                            }
                            // it is not good, other unit blocks
                        }
                        else {
                            good = true;
                        }
                    }

                    // is not visible, always good (since we don't know yet if its blocked!)
                    if (m_map->isVisible(cll, m_controller) == false) {
                        good = true;
                    }
                    else {
                        // walls stop us
                        if (cellType == TERRAIN_WALL) {
                            good = false;
                        }

                        // When we are infantry, we move through mountains. However, normal units do not
                        if (!m_activeUnit->isInfantryUnit()) {
                            if (cellType == TERRAIN_MOUNTAIN) {
                                good = false;
                            }
                        }
                    }
                }
                else {
                    // Sandworm only cares about terrain type for good/bad cells
                    good = m_map->isCellPassableForWorm(cll);
                }

                if (!good) {
                    continue;
                }

//                pUnit->log(std::format("CREATE_PATH: cll [{}] != [{}] && temp_map[cll].state [{}] and good [{}]",
//                                      cll, iCell, temp_map[cll].state, good));

                // it is the goal cell
                if (cll == m_goalCell) {
                    the_cll = cll;
                    cost = 0;
                    bail_out = true;
                    m_activeUnit->log("CREATE_PATH: Found the goal cell, success, bailing out");
                    break;
                }

                bool isUnvisited = m_pathMap[cll].state == UNVISITED;

                // when the cell (the attached one) is NOT the cell we are on and
                // the cell is UNVISITED (not yet explored)
                if (cll != m_currentCell && // not checking on our own
                    isUnvisited) { // and is unvisited (else it's not valid to check)
                    int gcx = m_mapGeometry->getCellX(m_goalCell);
                    int gcy = m_mapGeometry->getCellY(m_goalCell);

                    // calculate the cost
                    // treat unvisited cells (cost == -1) as 0 to avoid corrupting newCost
                    int tempCost = (m_pathMap[cll].cost >= 0) ? m_pathMap[cll].cost : 0;
                    double distanceCost = m_mapGeometry->distance(cx, cy, gcx, gcy);
                    double newCost = distanceCost + tempCost;
//                        pUnit->log(std::format(
//                                "CREATE_PATH: tempCost [{}] + distanceCost [{}] = newCost = [{}] vs current cost [{}]",
//                                tempCost, distanceCost, newCost, cost));

                    // when the cost is lower than we had before
                    if (newCost < cost) {
                        // when the cost is lower than the previous cost, then we set the new cost and we set the cell
                        the_cll = cll;
                        cost = newCost;
                        // found a new cell, now decrease ipathcountunits
                        m_pathCountUnits--;
//                            pUnit->log(std::format(
//                                    "CREATE_PATH: Waypoint found : cell {} - goalcell = {}, iPathCountUnits = {}", cll,
//                                    goal_cell, iPathCountUnits));
                    }
//                    else {
//                            pUnit->log(std::format(
//                                    "CREATE_PATH: Waypoint found : cell {} - goalcell = {}, iPathCountUnits = {}", cll,
//                                    goal_cell, iPathCountUnits));
//                    }
                } // END OF LOOP #2
            } // Y thingy

            // bail out
            if (bail_out) {
                m_activeUnit->log("CREATE_PATH: BAIL");
                break;
            }
        }

        // When found a new c(e)ll;
        if (the_cll > -1) {
            //pUnit->log(std::format("Found cell as best candidate: {}, parent is {}", the_cll, iCell));
            // Open this one, so we do not check it again
            m_pathMap[the_cll].state = OPEN;
            m_pathMap[the_cll].parent = m_currentCell;
            m_pathMap[the_cll].cost = cost;

            // Now set c to the cll
            m_currentCell = the_cll;
            if (m_currentCell == m_goalCell) {
                m_valid = false;
                m_success = true;
            }

        }
        else {
            // No unvisited neighbor found: mark current cell as a dead end and backtrack to parent
            m_pathMap[m_currentCell].state = CLOSED;
            int prevCell = m_pathMap[m_currentCell].parent;

            if (prevCell > -1) {
                //pUnit->log(std::format("Dead end at cell {}, backtracking to parent {}", iCell, prevCell));
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
    std::fill(m_tempPath.begin(), m_tempPath.end(), -1);

    bool cp = true;

    int sc = m_currentCell;
    int pi = 0;
    m_tempPath[pi] = sc;
    pi++;

    m_activeUnit->log(std::format("Starting backtracing. Path index = {}, temp_path[0] = {}", pi, m_tempPath[pi - 1]));

    // while we should create a path
    while (cp) {
        int tmp = m_pathMap[sc].parent;
        if (tmp > -1) {
            // found terminator (PARENT=CURRENT)
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
    // reverse
    int z = backtraceLength - 1; // start from the last valid index in temp_path
    int a = 0;
    int iPrevCell = -1;

    while (z > -1) {
        if (m_tempPath[z] > -1) {
            // check if any other cell of temp_path also borders to the previous given cell, as that will save us time
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

    // take the closest bordering cell as 'far' away to start with
    for (int i = 1; i < MAX_PATH_SIZE; i++) {
        int pathCell = m_activeUnit->movement.iPath[i];
        if (pathCell > -1) {
            if (m_mapGeometry->isCellAdjacentToOtherCell(m_activeUnit->getCell(), pathCell)) {
                m_activeUnit->movement.iPathIndex = i;
            }
        }
    }
}

/*
  Pathfinder

  Last revision: 02/05/2026
  Before revision: 19/02/2006

  The pathfinder will first eliminate any possibility that it will fail.
  It will check if the unit is free to move, if the timer is set correctly
  and so forth.

  Then the actual FDS path finder starts. Which will output a 'reversed' traceable
  path.

  Eventually this path is converted back to a waypoint string for units, and also
  optimized (skipping attaching cells which are reachable and closer to goal or further on
  path string).

  Return possibilities:
  -1 = FAIL (goalcell = cell, or cannot find path)
  -2 = Cannot move, all surrounded (blocked)
  -3 = Too many paths created
  -4 = Offset is not 0 (moving between cells)
  -99= iUnitId is < 0 (invalid input)
  */
int cPathFinder::createPath(int iUnitId, int iPathCountUnits)
{
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

    // debug debug
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

// void cPathFinder::verifyPathContiguity(const cUnit* pUnit, int firstCell)
// {
//     for (int i = 1; i < MAX_PATH_LOCAL_SIZE; ++i) {
//         int prevCell = pUnit->movement.iPath[i - 1];
//         int currCell = pUnit->movement.iPath[i];
//         if (prevCell == -1 || currCell == -1) break; // fin du chemin

//         if (!m_mapGeometry->isCellAdjacentToOtherCell(prevCell, currCell)) {
//             std::string msg = std::format("ERREUR: Chemin non contigu entre {} et {} à l'index {}", prevCell, currCell, i);
//             pUnit->log(msg);
//             std::cerr << msg << std::endl;
//         }
//     }
//     if (firstCell == pUnit->movement.iPath[0])
//         return;

//     if (!m_mapGeometry->isCellAdjacentToOtherCell(firstCell, pUnit->movement.iPath[0])) {
//         std::string msg = std::format("ERREUR: Le premier cell du chemin ({}) n'est pas égal au cell de départ ({})", pUnit->movement.iPath[0], firstCell);
//         pUnit->log(msg);
//         std::cerr << msg << std::endl;
//     }
// }

// find
int cPathFinder::returnCloseGoal(int iCll, int iMyCell, int iID)
{
    //
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

        // Fix boundaries
        cPoint::split(iStartX, iStartY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(iStartX, iStartY);
        cPoint::split(iEndX, iEndY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(iEndX, iEndY);

        // search
        for (int iSX = iStartX; iSX < iEndX; iSX++)
            for (int iSY = iStartY; iSY < iEndY; iSY++) {
                // find an empty cell
                int cll = m_mapGeometry->getCellWithMapDimensions(iSX, iSY);

                float dDistance2 = ABS_length(iSX, iSY, ix, iy);

                int idOfStructureAtCell = m_map->getCellIdStructuresLayer(cll);
                int idOfUnitAtCell = m_map->getCellIdUnitLayer(cll);

                if ((idOfStructureAtCell < 0) && (idOfUnitAtCell < 0)) { // no unit or structure at cell
                    // depending on unit type, do not choose walls (or mountains)
                    int cellType = m_map->getCellType(cll);
                    if (m_infos->getUnitInfo(m_objects->getUnit(iID)->iType).infantry) {
                        if (cellType == TERRAIN_MOUNTAIN)
                            continue; // do not use this one
                    }

                    if (cellType == TERRAIN_WALL)
                        continue; // do not use this one

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
            break; // get out
        }
    }

    // fail
    return iCll;
}

void cPathFinder::resetPathCreatedByUnit()
{
    m_pathsCreated = 0;
}