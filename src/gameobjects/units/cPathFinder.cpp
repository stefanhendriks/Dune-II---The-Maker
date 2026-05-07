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

void cPathFinder::initializeCreatePathSearch(int pathCountUnitsBudget)
{
    memset(m_activeUnit->movement.iPath, -1, sizeof(m_activeUnit->movement.iPath));
    m_activeUnit->movement.iPathIndex = -1;

    m_goalCell = m_activeUnit->movement.iGoalCell;
    m_controller = m_activeUnit->iPlayer;
    m_pathCountUnits = pathCountUnitsBudget;

    m_valid = true;
    m_success = false;

    m_pathsCreated++;
    // Reset the search map (cost + parent + per-cell state).
    for (auto &pathCell : m_pathMap) {
        pathCell.cost = -1;
        pathCell.parent = -1;
        pathCell.state = UNVISITED;
    }

    int currentCellX = m_mapGeometry->getCellX(m_currentCell);
    int currentCellY = m_mapGeometry->getCellY(m_currentCell);

    m_pathMap[m_currentCell].cost = ABS_length(currentCellX, currentCellY,
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

        int structureIdAtCurrentCell = m_map->cellGetIdFromLayer(m_currentCell, MAPID_STRUCTURES);
        if (m_activeUnit->iStructureID > -1) {
            if (structureIdAtCurrentCell == m_activeUnit->iStructureID) {
                m_valid = false;
                m_success = true;
                m_activeUnit->log("Found structure ID");
                break;
            }
        }

        if (m_activeUnit->combat.iAttackStructure > -1) {
            if (structureIdAtCurrentCell == m_activeUnit->combat.iAttackStructure) {
                m_valid = false;
                m_success = true;
                m_activeUnit->log("Found attack structure ID");
                break;
            }
        }

        int currentX = m_mapGeometry->getCellX(m_currentCell);
        int currentY = m_mapGeometry->getCellY(m_currentCell);

        int minNeighborX = currentX - 1;
        int minNeighborY = currentY - 1;

        int maxNeighborX = currentX + 1;
        int maxNeighborY = currentY + 1;

        cPoint::split(minNeighborX, minNeighborY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(minNeighborX, minNeighborY);
        cPoint::split(maxNeighborX, maxNeighborY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(maxNeighborX, maxNeighborY);

        double bestCandidateCost = 999999999;
        int bestCandidateCell = -1;

        bool reachedGoalNeighbor = false;

        // Explore the 3x3 neighborhood around the current cell.
        for (int neighborX = minNeighborX; neighborX <= maxNeighborX; neighborX++) {
            for (int neighborY = minNeighborY; neighborY <= maxNeighborY; neighborY++) {
                int candidateCell = m_mapGeometry->getCellWithMapBorders(neighborX, neighborY);

                if (candidateCell < 0)
                    continue;

                if (candidateCell == m_currentCell)
                    continue;

                bool candidateIsPassable = false;

                int candidateTerrainType = m_map->getCellType(candidateCell);
                if (!m_activeUnit->isSandworm()) {
                    // For regular units, traversability depends on occupancy
                    // (units/structures), visibility, and terrain.
                    int unitIdAtCandidateCell = m_map->getCellIdUnitLayer(candidateCell);
                    int structureIdAtCandidateCell = m_map->getCellIdStructuresLayer(candidateCell);

                    if (unitIdAtCandidateCell == -1 && structureIdAtCandidateCell == -1) {
                        candidateIsPassable = true;
                    }

                    if (structureIdAtCandidateCell > -1) {
                        if (m_activeUnit->combat.iAttackStructure > -1)
                            if (structureIdAtCandidateCell == m_activeUnit->combat.iAttackStructure)
                                candidateIsPassable = true;

                        if (m_activeUnit->iStructureID > -1)
                            if (structureIdAtCandidateCell == m_activeUnit->iStructureID)
                                candidateIsPassable = true;

                    }

                    if (unitIdAtCandidateCell > -1) {
                        if (unitIdAtCandidateCell != m_activeUnit->iID) {
                            int blockingUnitId = unitIdAtCandidateCell;

                            if (m_pathCountUnits <= 0) {
                                candidateIsPassable = false;
                                m_activeUnit->log("iPathCountUnits <= 0 - variable 'good' becomes 'false'");
                            }

                            cUnit *blockingUnit = m_objects->getUnit(blockingUnitId);
                            if (!blockingUnit->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer())) {
                                if (blockingUnit->isInfantryUnit() &&
                                    m_activeUnit->canSquishInfantry())
                                    candidateIsPassable = true;
                            }
                        }
                        else {
                            candidateIsPassable = true;
                        }
                    }

                    if (m_map->isVisible(candidateCell, m_controller) == false) {
                        candidateIsPassable = true;
                    }
                    else {
                        if (candidateTerrainType == TERRAIN_WALL) {
                            candidateIsPassable = false;
                        }

                        if (!m_activeUnit->isInfantryUnit()) {
                            if (candidateTerrainType == TERRAIN_MOUNTAIN) {
                                candidateIsPassable = false;
                            }
                        }
                    }
                }
                else {
                    candidateIsPassable = m_map->isCellPassableForWorm(candidateCell);
                }

                if (!candidateIsPassable) {
                    continue;
                }

                if (candidateCell == m_goalCell) {
                    bestCandidateCell = candidateCell;
                    bestCandidateCost = 0;
                    reachedGoalNeighbor = true;
                    m_activeUnit->log("CREATE_PATH: Found the goal cell, success, bailing out");
                    break;
                }

                bool isUnvisited = m_pathMap[candidateCell].state == UNVISITED;

                if (candidateCell != m_currentCell &&
                    isUnvisited) {
                    int goalX = m_mapGeometry->getCellX(m_goalCell);
                    int goalY = m_mapGeometry->getCellY(m_goalCell);

                    int candidateAccumulatedCost = (m_pathMap[candidateCell].cost >= 0) ? m_pathMap[candidateCell].cost : 0;
                    double goalDistanceCost = m_mapGeometry->distance(neighborX, neighborY, goalX, goalY);
                    double candidateScore = goalDistanceCost + candidateAccumulatedCost;

                    if (candidateScore < bestCandidateCost) {
                        // Heuristic: keep the neighbor that minimizes distance to the goal.
                        bestCandidateCell = candidateCell;
                        bestCandidateCost = candidateScore;
                        m_pathCountUnits--;
                    }
                }
            }

            if (reachedGoalNeighbor) {
                m_activeUnit->log("CREATE_PATH: BAIL");
                break;
            }
        }

        if (bestCandidateCell > -1) {
            // Move forward: the chosen neighbor becomes the new current cell.
            m_pathMap[bestCandidateCell].state = OPEN;
            m_pathMap[bestCandidateCell].parent = m_currentCell;
            m_pathMap[bestCandidateCell].cost = bestCandidateCost;

            m_currentCell = bestCandidateCell;
            if (m_currentCell == m_goalCell) {
                m_valid = false;
                m_success = true;
            }

        }
        else {
            // Dead end: close the current cell and backtrack to its parent.
            m_pathMap[m_currentCell].state = CLOSED;
            int parentCell = m_pathMap[m_currentCell].parent;

            if (parentCell > -1) {
                m_currentCell = parentCell;
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

    bool continueBacktrace = true;

    int currentBacktraceCell = m_currentCell;
    int pathLength = 0;
    m_tempPath[pathLength] = currentBacktraceCell;
    pathLength++;

    m_activeUnit->log(std::format("Starting backtracing. Path index = {}, temp_path[0] = {}", pathLength, m_tempPath[pathLength - 1]));

    while (continueBacktrace) {
        int parentCell = m_pathMap[currentBacktraceCell].parent;
        if (parentCell > -1) {
            if (parentCell == currentBacktraceCell) {
                m_activeUnit->log("found terminator, stop!");
                continueBacktrace = false;
                continue;
            }
            else {
                m_tempPath[pathLength] = parentCell;
                currentBacktraceCell = m_pathMap[currentBacktraceCell].parent;
                pathLength++;

                if (pathLength >= static_cast<int>(m_tempPath.size())) {
                    std::string warningMessage = std::format("WARNING: backtrace truncated - path exceeds MAX_PATH_LOCAL_SIZE ({})", m_tempPath.size());
                    m_activeUnit->log(warningMessage);
                    continueBacktrace = false;
                    continue;
                }

                m_activeUnit->log(std::format("Backtraced. Path index = {}, temp_path[last] = {}", pathLength, m_tempPath[pathLength - 1]));
            }
        }
        else {
            std::string warningMessage = std::format("WARNING: backtrace stopped at cell {} - parent is -1 (no parent found)", currentBacktraceCell);
            m_activeUnit->log(warningMessage);
            continueBacktrace = false;
        }

        if (currentBacktraceCell == m_activeUnit->getCell()) {
            continueBacktrace = false;
        }
    }

    return pathLength;
}

void cPathFinder::applyTempPathToUnit(int backtracedPathLength)
{
    // Reverse the backtraced path and compact intermediate points when
    // a farther cell is still adjacent to the previous selected cell.
    int tempPathReadIndex = backtracedPathLength - 1;
    int unitPathWriteIndex = 0;
    int previousAssignedCell = -1;

    while (tempPathReadIndex > -1) {
        if (m_tempPath[tempPathReadIndex] > -1) {
            if (previousAssignedCell > -1) {
                int farthestAdjacentIndex = -1;

                for (int scanIndex = tempPathReadIndex; scanIndex > 0; scanIndex--) {
                    if (m_tempPath[scanIndex] > -1) {

                        if (m_mapGeometry->isCellAdjacentToOtherCell(previousAssignedCell, m_tempPath[scanIndex])) {
                            farthestAdjacentIndex = scanIndex;
                        }
                    }
                    else {
                        break;
                    }
                }

                if (farthestAdjacentIndex < tempPathReadIndex && farthestAdjacentIndex > -1) {
                    tempPathReadIndex = farthestAdjacentIndex;
                }
            }

            if (unitPathWriteIndex >= MAX_PATH_SIZE) {
                m_activeUnit->log(std::format("WARNING: path truncated - exceeds MAX_PATH_SIZE ({})", MAX_PATH_SIZE));
                break;
            }
            m_activeUnit->movement.iPath[unitPathWriteIndex] = m_tempPath[tempPathReadIndex];
            previousAssignedCell = m_tempPath[tempPathReadIndex];
            unitPathWriteIndex++;
        }
        tempPathReadIndex--;
    }

    m_activeUnit->movement.iPathIndex = 1;

    for (int pathIndex = 1; pathIndex < MAX_PATH_SIZE; pathIndex++) {
        int pathCell = m_activeUnit->movement.iPath[pathIndex];
        if (pathCell > -1) {
            if (m_mapGeometry->isCellAdjacentToOtherCell(m_activeUnit->getCell(), pathCell)) {
                m_activeUnit->movement.iPathIndex = pathIndex;
            }
        }
    }
}

int cPathFinder::createPath(int unitId, int pathCountUnitsBudget)
{
    // Full pipeline: validate, search, rebuild, then apply to unit movement.
    int validationStatus = validateCreatePathInput(unitId);
    if (validationStatus != 0) {
        return validationStatus;
    }

    initializeCreatePathSearch(pathCountUnitsBudget);
    executeCreatePathSearch();
    if (!m_success) {
        m_activeUnit->log("CREATE_PATH -- not valid");
        m_activeUnit->log("CREATE_PATH: Failed to create path!");
        return -1;
    }
    m_activeUnit->log("CREATE_PATH -- valid loop finished");

    m_activeUnit->log("CREATE_PATH -- pathfinder got to goal-cell. Backtracing ideal path.");
    int backtracedPathLength = backtracePathToTempBuffer();
    applyTempPathToUnit(backtracedPathLength);

    if (m_settings->isDebugMode()) {
        for (int pathIndex = 0; pathIndex < MAX_PATH_SIZE; pathIndex++) {
            int pathCell = m_activeUnit->movement.iPath[pathIndex];
            if (pathCell > -1) {
                m_activeUnit->log(std::format("WAYPOINT {} = {} ", pathIndex, pathCell));
            }
        }
    }

    m_activeUnit->updateCellXAndY();
    m_activeUnit->movement.bCalculateNewPath = false;

    return 0;
}

int cPathFinder::returnCloseGoal(int targetCell, int originCell, int unitId)
{
    // Find a free cell near targetCell by gradually expanding the search radius,
    // then pick the one closest to originCell.
    int searchRadius = 1;
    int startX = m_mapGeometry->getCellX(targetCell) - searchRadius;
    int startY = m_mapGeometry->getCellY(targetCell) - searchRadius;
    int endX = m_mapGeometry->getCellX(targetCell) + searchRadius;
    int endY = m_mapGeometry->getCellY(targetCell) + searchRadius;

    float bestDistance = 9999;

    int originX = m_mapGeometry->getCellX(originCell);
    int originY = m_mapGeometry->getCellY(originCell);

    bool continueSearch = true;

    int closestCell = -1;

    while (continueSearch) {
        startX = m_mapGeometry->getCellX(targetCell) - searchRadius;
        startY = m_mapGeometry->getCellY(targetCell) - searchRadius;
        endX = m_mapGeometry->getCellX(targetCell) + searchRadius;
        endY = m_mapGeometry->getCellY(targetCell) + searchRadius;

        cPoint::split(startX, startY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(startX, startY);
        cPoint::split(endX, endY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(endX, endY);

        for (int searchX = startX; searchX < endX; searchX++)
            for (int searchY = startY; searchY < endY; searchY++) {
                int candidateCell = m_mapGeometry->getCellWithMapDimensions(searchX, searchY);

                float candidateDistance = ABS_length(searchX, searchY, originX, originY);

                int structureIdAtCandidateCell = m_map->getCellIdStructuresLayer(candidateCell);
                int unitIdAtCandidateCell = m_map->getCellIdUnitLayer(candidateCell);

                if ((structureIdAtCandidateCell < 0) && (unitIdAtCandidateCell < 0)) {
                    int terrainType = m_map->getCellType(candidateCell);
                    if (m_infos->getUnitInfo(m_objects->getUnit(unitId)->iType).infantry) {
                        if (terrainType == TERRAIN_MOUNTAIN)
                            continue;
                    }

                    if (terrainType == TERRAIN_WALL)
                        continue;

                    if (candidateDistance < bestDistance) {
                        bestDistance = candidateDistance;
                        closestCell = candidateCell;
                    }
                }
            }

        if (closestCell > -1)
            return closestCell;

        searchRadius++;

        if (searchRadius > 9) {
            continueSearch = false;
            break;
        }
    }

    return targetCell;
}

void cPathFinder::resetPathCreatedByUnit()
{
    m_pathsCreated = 0;
}