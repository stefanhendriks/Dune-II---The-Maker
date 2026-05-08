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
static constexpr int WAYPOINT_STEP_CELLS = 32;

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
                            if (blockingUnit == nullptr) {
                                candidateIsPassable = false;
                            }
                            else if (!blockingUnit->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer())) {
                                if (blockingUnit->isInfantryUnit() && m_activeUnit->canSquishInfantry())
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

            // Spend one budget unit only when a move candidate is effectively selected.
            m_pathCountUnits--;

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

    // Local anti-zig-zag smoothing: for A-B-C, try replacing B with D where
    // D is one step from A towards C and remains a valid walkable bridge.
    auto isCellWalkableForActiveUnit = [this](int cell) -> bool {
        if (cell < 0) {
            return false;
        }

        if (m_activeUnit->isSandworm()) {
            return m_map->isCellPassableForWorm(cell);
        }

        int structureIdAtCell = m_map->getCellIdStructuresLayer(cell);
        int unitIdAtCell = m_map->getCellIdUnitLayer(cell);

        bool isPassable = (structureIdAtCell < 0 && unitIdAtCell < 0);

        if (structureIdAtCell > -1) {
            if (m_activeUnit->combat.iAttackStructure > -1 &&
                structureIdAtCell == m_activeUnit->combat.iAttackStructure) {
                isPassable = true;
            }

            if (m_activeUnit->iStructureID > -1 &&
                structureIdAtCell == m_activeUnit->iStructureID) {
                isPassable = true;
            }
        }

        if (unitIdAtCell > -1 && unitIdAtCell != m_activeUnit->iID) {
            cUnit *blockingUnit = m_objects->getUnit(unitIdAtCell);
            if (blockingUnit != nullptr &&
                !blockingUnit->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer()) &&
                blockingUnit->isInfantryUnit() &&
                m_activeUnit->canSquishInfantry()) {
                isPassable = true;
            }
        }

        if (!m_map->isVisible(cell, m_controller)) {
            return isPassable;
        }

        int terrainType = m_map->getCellType(cell);
        if (terrainType == TERRAIN_WALL) {
            return false;
        }

        if (!m_activeUnit->isInfantryUnit() && terrainType == TERRAIN_MOUNTAIN) {
            return false;
        }

        return isPassable;
    };

    for (int pathIndex = 0; pathIndex + 2 < unitPathWriteIndex; pathIndex++) {
        int cellA = m_activeUnit->movement.iPath[pathIndex];
        int cellB = m_activeUnit->movement.iPath[pathIndex + 1];
        int cellC = m_activeUnit->movement.iPath[pathIndex + 2];

        if (cellA < 0 || cellB < 0 || cellC < 0) {
            break;
        }

        int ax = m_mapGeometry->getCellX(cellA);
        int ay = m_mapGeometry->getCellY(cellA);
        int cx = m_mapGeometry->getCellX(cellC);
        int cy = m_mapGeometry->getCellY(cellC);

        int stepX = std::clamp(cx - ax, -1, 1);
        int stepY = std::clamp(cy - ay, -1, 1);
        if (stepX == 0 && stepY == 0) {
            continue;
        }

        int candidateCell = m_mapGeometry->getCellWithMapBorders(ax + stepX, ay + stepY);
        if (candidateCell < 0 || candidateCell == cellA || candidateCell == cellC || candidateCell == cellB) {
            continue;
        }

        if (!m_mapGeometry->isCellAdjacentToOtherCell(cellA, candidateCell)) {
            continue;
        }

        if (!m_mapGeometry->isCellAdjacentToOtherCell(candidateCell, cellC)) {
            continue;
        }

        if (!isCellWalkableForActiveUnit(candidateCell)) {
            continue;
        }

        m_activeUnit->movement.iPath[pathIndex + 1] = candidateCell;
    }

    // Build waypoint memory: keep one waypoint every WAYPOINT_STEP_CELLS path cells.
    memset(m_activeUnit->movement.waypointCells, -1, sizeof(m_activeUnit->movement.waypointCells));
    int waypointWriteIndex = 0;
    for (int pathIndex = 0; pathIndex < unitPathWriteIndex; pathIndex += WAYPOINT_STEP_CELLS) {
        if (waypointWriteIndex >= MAX_WAYPOINTS_SIZE) {
            break;
        }

        int pathCell = m_activeUnit->movement.iPath[pathIndex];
        if (pathCell > -1) {
            m_activeUnit->movement.waypointCells[waypointWriteIndex] = pathCell;
            waypointWriteIndex++;
        }
    }

    // Also remember the final valid path cell so destination is represented.
    if (unitPathWriteIndex > 0 && waypointWriteIndex < MAX_WAYPOINTS_SIZE) {
        int lastPathCell = m_activeUnit->movement.iPath[unitPathWriteIndex - 1];
        if (lastPathCell > -1) {
            bool alreadyStored = false;
            for (int i = 0; i < waypointWriteIndex; i++) {
                if (m_activeUnit->movement.waypointCells[i] == lastPathCell) {
                    alreadyStored = true;
                    break;
                }
            }

            if (!alreadyStored) {
                m_activeUnit->movement.waypointCells[waypointWriteIndex] = lastPathCell;
            }
        }
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

    // General rule: if first expected step goes away from the final goal direction,
    // recompute the first segment by re-hooking through waypoint logic.
    if (!m_isApplyingFirstSegmentCorrection) {
        int firstStepCell = -1;
        for (int pathIndex = 1; pathIndex < MAX_PATH_SIZE; pathIndex++) {
            int pathCell = m_activeUnit->movement.iPath[pathIndex];
            if (pathCell < 0) {
                break;
            }
            if (m_mapGeometry->isCellAdjacentToOtherCell(m_activeUnit->getCell(), pathCell)) {
                firstStepCell = pathCell;
                break;
            }
        }

        if (firstStepCell > -1) {
            int unitX = m_mapGeometry->getCellX(m_activeUnit->getCell());
            int unitY = m_mapGeometry->getCellY(m_activeUnit->getCell());
            int goalX = m_mapGeometry->getCellX(m_activeUnit->movement.iGoalCell);
            int goalY = m_mapGeometry->getCellY(m_activeUnit->movement.iGoalCell);
            int stepX = m_mapGeometry->getCellX(firstStepCell);
            int stepY = m_mapGeometry->getCellY(firstStepCell);

            int goalVectorX = goalX - unitX;
            int goalVectorY = goalY - unitY;
            int stepVectorX = stepX - unitX;
            int stepVectorY = stepY - unitY;
            int dotProduct = (goalVectorX * stepVectorX) + (goalVectorY * stepVectorY);

            if (dotProduct < 0) {
                m_activeUnit->log("CREATE_PATH: first step is opposite to goal direction, recomputing first segment");
                m_isApplyingFirstSegmentCorrection = true;
                (void)createPathToFirstReachableWaypointAndAppendExisting(unitId);
                m_isApplyingFirstSegmentCorrection = false;
            }
        }
    }

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

int cPathFinder::createPathToFirstReachableWaypointAndAppendExisting(int unitId)
{
    // STEP 1: Save original movement state and waypoint snapshot for rollback.
    if (unitId < 0) {
        return -1;
    }

    cUnit *unit = m_objects->getUnit(unitId);
    if (unit == nullptr || !unit->isValid() || unit->isDead()) {
        return -1;
    }

    const int originalGoalCell = unit->movement.iGoalCell;
    const int originalPathIndex = unit->movement.iPathIndex;
    const int originalPathFails = unit->movement.iPathFails;
    const bool originalCalculateNewPath = unit->movement.bCalculateNewPath;

    int originalWaypoints[MAX_WAYPOINTS_SIZE];
    memcpy(originalWaypoints, unit->movement.waypointCells, sizeof(originalWaypoints));

    int originalPath[MAX_PATH_SIZE];
    memcpy(originalPath, unit->movement.iPath, sizeof(originalPath));

    int firstSearchPathIndex = originalPathIndex;
    if (firstSearchPathIndex < 0) {
        firstSearchPathIndex = 0;
    }

    // STEP 2: Find the first waypoint that is still ahead of current path progress.
    // This is the first waypoint that we will try to reach with a detour path.
    // note it's not obligatory a multiple of WAYPOINT_STEP_CELLS,
    // as the unit might have already progressed after the last waypoint.
    int firstSearchWaypointIndex = 0;
    while (firstSearchWaypointIndex < MAX_WAYPOINTS_SIZE && originalWaypoints[firstSearchWaypointIndex] > -1) {
        int waypointCell = originalWaypoints[firstSearchWaypointIndex];
        bool isBeforeCurrentPathIndex = true;
        for (int pathIndex = firstSearchPathIndex; pathIndex < MAX_PATH_SIZE; pathIndex++) {
            int pathCell = unit->movement.iPath[pathIndex];
            if (pathCell < 0) {
                break;
            }
            if (pathCell == waypointCell) {
                isBeforeCurrentPathIndex = false;
                break;
            }
        }

        if (!isBeforeCurrentPathIndex) {
            break;
        }
        firstSearchWaypointIndex++;
    }

    int waypointIndex = firstSearchWaypointIndex;
    if (waypointIndex >= MAX_WAYPOINTS_SIZE || originalWaypoints[waypointIndex] < 0) {
        unit->movement.iGoalCell = originalGoalCell;
        unit->movement.iPathIndex = originalPathIndex;
        unit->movement.iPathFails = originalPathFails;
        unit->movement.bCalculateNewPath = originalCalculateNewPath;
        return -1;
    }
    // STEP 3: Locate this waypoint in the current path.
    int waypointCell = originalWaypoints[waypointIndex];
    int waypointPathIndex = -1;
    for (int pathIndex = firstSearchPathIndex; pathIndex < MAX_PATH_SIZE; pathIndex++) {
        int pathCell = originalPath[pathIndex];
        if (pathCell < 0) {
            break;
        }
        if (pathCell == waypointCell) {
            waypointPathIndex = pathIndex;
            break;
        }
    }

    if (waypointPathIndex < 0) {
        unit->movement.iGoalCell = originalGoalCell;
        unit->movement.iPathIndex = originalPathIndex;
        unit->movement.iPathFails = originalPathFails;
        unit->movement.bCalculateNewPath = originalCalculateNewPath;
        return -1;
    }

    // If we are already close in path steps, target the following waypoint for extra freedom.
    const int cellsToWaypoint = waypointPathIndex - firstSearchPathIndex;
    if (cellsToWaypoint < 6) {
        const int candidateNextWaypointIndex = waypointIndex + 1;
        if (candidateNextWaypointIndex < MAX_WAYPOINTS_SIZE && originalWaypoints[candidateNextWaypointIndex] > -1) {
            int candidateNextWaypointPathIndex = -1;
            const int candidateNextWaypointCell = originalWaypoints[candidateNextWaypointIndex];
            for (int pathIndex = waypointPathIndex + 1; pathIndex < MAX_PATH_SIZE; pathIndex++) {
                int pathCell = originalPath[pathIndex];
                if (pathCell < 0) {
                    break;
                }
                if (pathCell == candidateNextWaypointCell) {
                    candidateNextWaypointPathIndex = pathIndex;
                    break;
                }
            }

            if (candidateNextWaypointPathIndex > -1) {
                waypointIndex = candidateNextWaypointIndex;
                waypointCell = candidateNextWaypointCell;
                waypointPathIndex = candidateNextWaypointPathIndex;
            }
        }
    }

    // STEP 4: Snapshot the full remaining suffix after this waypoint from the original path.
    int remainingPathLength = 0;
    int remainingPath[MAX_PATH_SIZE];
    memset(remainingPath, -1, sizeof(remainingPath));
    for (int pathIndex = waypointPathIndex + 1; pathIndex < MAX_PATH_SIZE && remainingPathLength < MAX_PATH_SIZE; pathIndex++) {
        int pathCell = originalPath[pathIndex];
        if (pathCell < 0) {
            break;
        }

        remainingPath[remainingPathLength] = pathCell;
        remainingPathLength++;
    }

    // STEP 5: Try the selected waypoint and compute a detour path.
    unit->movement.iGoalCell = waypointCell;
    int detourResult = createPath(unitId, 0);
    if (detourResult < 0) {
        unit->movement.iGoalCell = originalGoalCell;
        unit->movement.iPathIndex = originalPathIndex;
        unit->movement.iPathFails = originalPathFails;
        unit->movement.bCalculateNewPath = originalCalculateNewPath;
        return -1;
    }

    int detourPath[MAX_PATH_SIZE];
    memcpy(detourPath, unit->movement.iPath, sizeof(detourPath));

    int detourPathLength = 0;
    for (int i = 0; i < MAX_PATH_SIZE; i++) {
        if (detourPath[i] < 0) {
            break;
        }
        detourPathLength++;
    }

    if (detourPathLength <= 0) {
        unit->movement.iGoalCell = originalGoalCell;
        unit->movement.iPathIndex = originalPathIndex;
        unit->movement.iPathFails = originalPathFails;
        unit->movement.bCalculateNewPath = originalCalculateNewPath;
        return -1;
    }

    // STEP 6: Build a combined path = detour + full remaining original suffix.
    int combinedPath[MAX_PATH_SIZE];
    memset(combinedPath, -1, sizeof(combinedPath));

    int writeIndex = 0;
    for (int i = 0; i < detourPathLength && writeIndex < MAX_PATH_SIZE; i++) {
        combinedPath[writeIndex] = detourPath[i];
        writeIndex++;
    }

    for (int i = 0; i < remainingPathLength && writeIndex < MAX_PATH_SIZE; i++) {
        int remainingCell = remainingPath[i];
        if (writeIndex > 0 && combinedPath[writeIndex - 1] == remainingCell) {
            continue;
        }

        combinedPath[writeIndex] = remainingCell;
        writeIndex++;
    }

    memcpy(unit->movement.iPath, combinedPath, sizeof(combinedPath));

    // STEP 7: Apply combined path and rebuild movement metadata (waypoints, index, flags).
    memset(unit->movement.waypointCells, -1, sizeof(unit->movement.waypointCells));
    int waypointWriteIndex = 0;
    for (int pathIndex = 0; pathIndex < writeIndex; pathIndex += WAYPOINT_STEP_CELLS) {
        if (waypointWriteIndex >= MAX_WAYPOINTS_SIZE) {
            break;
        }

        int pathCell = unit->movement.iPath[pathIndex];
        if (pathCell > -1) {
            unit->movement.waypointCells[waypointWriteIndex] = pathCell;
            waypointWriteIndex++;
        }
    }

    if (writeIndex > 0 && waypointWriteIndex < MAX_WAYPOINTS_SIZE) {
        int lastPathCell = unit->movement.iPath[writeIndex - 1];
        if (lastPathCell > -1) {
            bool alreadyStored = false;
            for (int i = 0; i < waypointWriteIndex; i++) {
                if (unit->movement.waypointCells[i] == lastPathCell) {
                    alreadyStored = true;
                    break;
                }
            }

            if (!alreadyStored) {
                unit->movement.waypointCells[waypointWriteIndex] = lastPathCell;
            }
        }
    }

    unit->movement.iGoalCell = originalGoalCell;
    unit->movement.iPathIndex = 1;
    for (int pathIndex = 1; pathIndex < MAX_PATH_SIZE; pathIndex++) {
        int pathCell = unit->movement.iPath[pathIndex];
        if (pathCell > -1 && m_mapGeometry->isCellAdjacentToOtherCell(unit->getCell(), pathCell)) {
            unit->movement.iPathIndex = pathIndex;
        }
    }

    unit->movement.iPathFails = 0;
    unit->movement.bCalculateNewPath = false;
    unit->log(std::format("CREATE_PATH: Hooked to waypoint {} and appended remaining original path", waypointCell));
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

    cUnit *unit = m_objects->getUnit(unitId);
    bool unitIsInfantry = false;
    if (unit != nullptr) {
        unitIsInfantry = m_infos->getUnitInfo(unit->iType).infantry;
    }

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
                    if (unitIsInfantry) {
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