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
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "include/sGameServices.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <limits>
#include <queue>

static const int MAX_PATH_LOCAL_SIZE = 4096;
static constexpr int WAYPOINT_STEP_CELLS = 32;
static constexpr int DISTANCE_INF = std::numeric_limits<int>::max();

static int directionToNeighborIndex(int dx, int dy)
{
    if (dx == -1 && dy == -1) return 0;
    if (dx ==  0 && dy == -1) return 1;
    if (dx ==  1 && dy == -1) return 2;
    if (dx == -1 && dy ==  0) return 3;
    if (dx ==  1 && dy ==  0) return 4;
    if (dx == -1 && dy ==  1) return 5;
    if (dx ==  0 && dy ==  1) return 6;
    if (dx ==  1 && dy ==  1) return 7;
    return -1;
}

static int buildPreferredNeighborOrder(int sx, int sy, int orderedNeighborIndices[8])
{
    int orderedCount = 0;

    auto addDirection = [&](int dx, int dy) {
        if (dx == 0 && dy == 0) {
            return;
        }

        const int neighborIndex = directionToNeighborIndex(dx, dy);
        if (neighborIndex < 0) {
            return;
        }

        for (int i = 0; i < orderedCount; i++) {
            if (orderedNeighborIndices[i] == neighborIndex) {
                return;
            }
        }

        if (orderedCount < 8) {
            orderedNeighborIndices[orderedCount] = neighborIndex;
            orderedCount++;
        }
    };

    // Priority: toward target main axes first, then diagonal, then alternatives.
    addDirection(sx, 0);
    addDirection(0, sy);
    addDirection(-sx, 0);
    addDirection(0, -sy);    
    addDirection(sx, sy);
    addDirection(sx, -sy);
    addDirection(-sx, sy);
    addDirection(-sx, -sy);

    // Ensure full coverage only when some directions are still missing.
    if (orderedCount < 8) {
        static constexpr int fallbackDx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
        static constexpr int fallbackDy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
        for (int i = 0; i < 8; i++) {
            addDirection(fallbackDx[i], fallbackDy[i]);
        }
    }

    return orderedCount;
}

cPathFinder::cPathFinder()
{
    m_tempPath.resize(MAX_PATH_LOCAL_SIZE);
}

void cPathFinder::resize(int newSize, int width, int height)
{
    m_pathMap.resize(newSize);
    std::fill(m_tempPath.begin(), m_tempPath.end(), -1);
    m_neighbors.clear();
    m_neighborsWidth = -1;
    m_neighborsHeight = -1;
    ensureNeighborCache(width, height);
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
        logbook("CREATE_PATH -- END (no unit "+ std::to_string(unitId) + " found)");
        return -99;
    }
    if (!m_activeUnit->isValid() || m_activeUnit->isDead()) {
        logbook("CREATE_PATH -- END (unitId "+ std::to_string(unitId) + " isDead or not Valid)");
        return -99;
    }

    if (m_activeUnit->isMovingBetweenCells()) {
        m_activeUnit->log("CREATE_PATH -- END 2 moving Unit "+ std::to_string(unitId));
        return -4;
    }

    const int goalCell = m_activeUnit->movement.iGoalCell;
    if (goalCell > -1) {
        int occupyingUnitId = m_map->getCellIdUnitLayer(goalCell);
        if (occupyingUnitId > -1 && occupyingUnitId != unitId) {
            cUnit *occupyingUnit = m_objects->getUnit(occupyingUnitId);
            if (occupyingUnit != nullptr &&
                occupyingUnit->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer())) {
                m_activeUnit->log(std::format("CREATE_PATH -- END goal {} occupied by allied unit {}", goalCell, occupyingUnitId));
                return -1;
            }
        }
    }

    if (!m_activeUnit->isInfantryUnit()) {
        if (goalCell > -1 && m_map->getCellType(goalCell) == TERRAIN_MOUNTAIN) {
            m_activeUnit->log("CREATE_PATH -- END 5 invalid goal terrain (mountain)");
            return -5;
        }
    }

    if (m_pathsCreated > 40) {
        m_activeUnit->log("CREATE_PATH -- Too many paths END 3");
        m_activeUnit->movewaitTimer.reset(50 + RNG::rnd(50));
        return -3;
    }

    m_currentCell = m_activeUnit->getCell();

    if (m_currentCell == m_activeUnit->movement.iGoalCell) {
        m_activeUnit->log("CREATE_PATH -- END 4 ODD: The goal = cell?");
        return -1;
    }

    if (m_activeUnit->isUnableToMove()) {
        m_activeUnit->log("CREATE_PATH -- END unit "+ std::to_string(unitId) + "cannot move");
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

    m_success = false;

    m_pathsCreated++;
    // Reset the distance field (distance + parent toward goal).
    for (auto &pathCell : m_pathMap) {
        pathCell.cost = DISTANCE_INF;
        pathCell.parent = -1;
        pathCell.state = 0;
    }
}

void cPathFinder::ensureNeighborCache(int mapWidth, int mapHeight)
{
    if (m_neighborsWidth == mapWidth &&
        m_neighborsHeight == mapHeight &&
        static_cast<int>(m_neighbors.size()) == mapWidth * mapHeight) {
        return;
    }

    const int maxCells = mapWidth * mapHeight;
    m_neighbors.assign(maxCells, std::array<int, 8>{-1, -1, -1, -1, -1, -1, -1, -1});

    static constexpr int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    static constexpr int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    for (int cell = 0; cell < maxCells; cell++) {
        const int x = cell % mapWidth;
        const int y = cell / mapWidth;

        for (int i = 0; i < 8; i++) {
            const int nx = x + dx[i];
            const int ny = y + dy[i];

            if (nx < 0 || ny < 0 || nx >= mapWidth || ny >= mapHeight) {
                m_neighbors[cell][i] = -1;
                continue;
            }

            m_neighbors[cell][i] = (ny * mapWidth) + nx;
        }
    }

    m_neighborsWidth = mapWidth;
    m_neighborsHeight = mapHeight;
}

bool cPathFinder::isCellPassableForActiveUnit(int candidateCell) const
{
    if (candidateCell < 0) {
        return false;
    }

    // Never allow traversing the invisible 1-cell map border.
    const int mapWidth = m_map->getWidth();
    const int mapHeight = m_map->getHeight();
    const int cellX = candidateCell % mapWidth;
    const int cellY = candidateCell / mapWidth;
    if (cellX <= 0 || cellY <= 0 || cellX >= (mapWidth - 1) || cellY >= (mapHeight - 1)) {
        return false;
    }

    if (m_activeUnit->isSandworm()) {
        return m_map->isCellPassableForWorm(candidateCell);
    }

    int unitIdAtCandidateCell = m_map->getCellIdUnitLayer(candidateCell);
    int structureIdAtCandidateCell = m_map->getCellIdStructuresLayer(candidateCell);
    int candidateTerrainType = m_map->getCellType(candidateCell);

    if (candidateTerrainType < 0) {
        return false;
    }

    bool candidateIsPassable = false;

    if (unitIdAtCandidateCell == -1 && structureIdAtCandidateCell == -1) {
        candidateIsPassable = true;
    }

    if (structureIdAtCandidateCell > -1) {
        if (m_activeUnit->combat.iAttackStructure > -1 &&
            structureIdAtCandidateCell == m_activeUnit->combat.iAttackStructure) {
            candidateIsPassable = true;
        }

        if (m_activeUnit->iStructureID > -1 &&
            structureIdAtCandidateCell == m_activeUnit->iStructureID) {
            candidateIsPassable = true;
        }
    }

    if (unitIdAtCandidateCell > -1) {
        if (unitIdAtCandidateCell == m_activeUnit->iID) {
            candidateIsPassable = true;
        }
        else {
            candidateIsPassable = false;

            if (m_pathCountUnits > 0) {
                cUnit *blockingUnit = m_objects->getUnit(unitIdAtCandidateCell);
                if (blockingUnit != nullptr &&
                    !blockingUnit->getPlayer()->isSameTeamAs(m_activeUnit->getPlayer()) &&
                    blockingUnit->isInfantryUnit() &&
                    m_activeUnit->canSquishInfantry()) {
                    candidateIsPassable = true;
                }
            }
        }
    }

    if (!m_map->isVisible(candidateCell, m_controller)) {
        return true;
    }

    if (candidateTerrainType == TERRAIN_WALL) {
        return false;
    }

    if (!m_activeUnit->isInfantryUnit() && candidateTerrainType == TERRAIN_MOUNTAIN) {
        return false;
    }

    return candidateIsPassable;
}

void cPathFinder::executeCreatePathSearch()
{
    // Compute a distance field from destination to all reachable cells, then
    // follow decreasing costs from unit position to build the path.
    if (m_goalCell < 0 || m_goalCell >= static_cast<int>(m_pathMap.size())) {
        m_success = false;
        return;
    }

    std::queue<int> frontier;
    const int mapWidth = m_map->getWidth();
    const int targetX = m_currentCell % mapWidth;
    const int targetY = m_currentCell / mapWidth;
    bool reachedCurrentCell = false;

    m_pathMap[m_goalCell].cost = 0;
    m_pathMap[m_goalCell].parent = -1;

    // Fast path: walk directly from goal to start, prioritizing the diagonal direction.
    // If blocked, we fall back to regular BFS expansion.
    {
        auto sign = [](int value) {
            return (value > 0) - (value < 0);
        };

        std::vector<int> directPathCells;
        directPathCells.reserve(64);
        directPathCells.push_back(m_goalCell);

        int currentLineCell = m_goalCell;
        bool directPathBlocked = false;

        while (currentLineCell != m_currentCell) {
            const int lineX = currentLineCell % mapWidth;
            const int lineY = currentLineCell / mapWidth;
            const int stepX = sign(targetX - lineX);
            const int stepY = sign(targetY - lineY);

            const int preferredDx[3] = {stepX, stepX, 0};
            const int preferredDy[3] = {stepY, 0, stepY};

            bool advanced = false;
            for (int i = 0; i < 3; i++) {
                const int dx = preferredDx[i];
                const int dy = preferredDy[i];

                if (dx == 0 && dy == 0) {
                    continue;
                }

                const int directionIndex = directionToNeighborIndex(dx, dy);
                if (directionIndex < 0) {
                    continue;
                }

                const int nextCell = m_neighbors[currentLineCell][directionIndex];
                if (nextCell < 0) {
                    continue;
                }

                // Keep legacy behavior for regular units (goal cell may be occupied/special),
                // but sandworms must stay on worm-passable terrain even for the goal cell.
                const bool skipPassabilityCheck =
                    (nextCell == m_currentCell) ||
                    (nextCell == m_goalCell && !m_activeUnit->isSandworm());

                if (!skipPassabilityCheck && !isCellPassableForActiveUnit(nextCell)) {
                    continue;
                }

                directPathCells.push_back(nextCell);
                currentLineCell = nextCell;
                advanced = true;
                break;
            }

            if (!advanced) {
                directPathBlocked = true;
                break;
            }

            if (directPathCells.size() >= static_cast<size_t>(MAX_PATH_LOCAL_SIZE)) {
                directPathBlocked = true;
                break;
            }
        }

        if (!directPathBlocked && currentLineCell == m_currentCell) {
            for (int i = 1; i < static_cast<int>(directPathCells.size()); i++) {
                const int pathCell = directPathCells[i];
                const int parentCell = directPathCells[i - 1];
                m_pathMap[pathCell].cost = i;
                m_pathMap[pathCell].parent = parentCell;
            }
            reachedCurrentCell = true;
        }
    }

    if (!reachedCurrentCell) {
        frontier.push(m_goalCell);
    }

    while (!frontier.empty() && !reachedCurrentCell) {
        const int currentCell = frontier.front();
        frontier.pop();

        if (currentCell == m_currentCell) {
            reachedCurrentCell = true;
            break;
        }

        const int currentDistance = m_pathMap[currentCell].cost;
        const int currentX = currentCell % mapWidth;
        const int currentY = currentCell / mapWidth;

        const int sx = (targetX > currentX) - (targetX < currentX);
        const int sy = (targetY > currentY) - (targetY < currentY);

        int orderedNeighborIndices[8] = {0};
        const int orderedCount = buildPreferredNeighborOrder(sx, sy, orderedNeighborIndices);

        for (int i = 0; i < orderedCount; i++) {
            const int neighborCell = m_neighbors[currentCell][orderedNeighborIndices[i]];
            if (neighborCell < 0) {
                continue;
            }

            if (m_pathMap[neighborCell].cost != DISTANCE_INF) {
                continue;
            }

            const bool skipNeighborPassabilityCheck =
                (neighborCell == m_goalCell && !m_activeUnit->isSandworm());

            if (!skipNeighborPassabilityCheck && !isCellPassableForActiveUnit(neighborCell)) {
                continue;
            }

            m_pathMap[neighborCell].cost = currentDistance + 1;
            m_pathMap[neighborCell].parent = currentCell;

            if (neighborCell == m_currentCell) {
                reachedCurrentCell = true;
                break;
            }

            frontier.push(neighborCell);
        }
    }

    m_success = m_pathMap[m_currentCell].cost != DISTANCE_INF;

    if (!m_success) {
        const int currentX = m_currentCell % mapWidth;
        const int currentY = m_currentCell / mapWidth;
        const int goalX = m_goalCell % mapWidth;
        const int goalY = m_goalCell / mapWidth;
        const int directDistance = std::abs(goalX - currentX) + std::abs(goalY - currentY);
        m_activeUnit->log(std::format("CREATE_PATH: distance field found no route from {} to {} (direct dist {})", m_currentCell, m_goalCell, directDistance));
    }
}

int cPathFinder::backtracePathToTempBuffer()
{
    // Follow parent pointers from current unit cell to goal (gradient descent).
    std::fill(m_tempPath.begin(), m_tempPath.end(), -1);

    int currentCell = m_activeUnit->getCell();
    int pathLength = 0;

    while (currentCell > -1 && pathLength < static_cast<int>(m_tempPath.size())) {
        m_tempPath[pathLength++] = currentCell;
        if (currentCell == m_goalCell) {
            return pathLength;
        }

        int parentCell = m_pathMap[currentCell].parent;
        if (parentCell == currentCell) {
            m_activeUnit->log("WARNING: path reconstruction aborted due to self-parent");
            return 0;
        }

        if (parentCell < 0) {
            m_activeUnit->log(std::format("WARNING: path reconstruction failed from cell {} (no parent)", currentCell));
            return 0;
        }

        currentCell = parentCell;
    }

    if (pathLength >= static_cast<int>(m_tempPath.size())) {
        m_activeUnit->log(std::format("WARNING: path reconstruction truncated - exceeds MAX_PATH_LOCAL_SIZE ({})", m_tempPath.size()));
    }

    return 0;
}

void cPathFinder::applyTempPathToUnit(int backtracedPathLength)
{
    // Temp path is already ordered from start to goal.
    int unitPathWriteIndex = 0;

    for (int i = 0; i < backtracedPathLength; i++) {
        int pathCell = m_tempPath[i];
        if (pathCell < 0) {
            break;
        }

        if (unitPathWriteIndex >= MAX_PATH_SIZE) {
            m_activeUnit->log(std::format("WARNING: path truncated - exceeds MAX_PATH_SIZE ({})", MAX_PATH_SIZE));
            break;
        }

        m_activeUnit->movement.iPath[unitPathWriteIndex++] = pathCell;
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

    m_activeUnit->movement.iPathIndex = (unitPathWriteIndex > 1) ? 1 : -1;

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

    m_activeUnit->log("CREATE_PATH -- distance field built. Reconstructing path.");
    int backtracedPathLength = backtracePathToTempBuffer();
    if (backtracedPathLength <= 1) {
        m_activeUnit->log("CREATE_PATH: reconstruction failed or empty path");
        return -1;
    }
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
    // Find a free cell near targetCell, scanning only the outer ring at each radius.
    // Returns the free cell closest to originCell, or targetCell if nothing found.
    const int targetX = m_mapGeometry->getCellX(targetCell);
    const int targetY = m_mapGeometry->getCellY(targetCell);
    const int originX = m_mapGeometry->getCellX(originCell);
    const int originY = m_mapGeometry->getCellY(originCell);

    cUnit *unit = m_objects->getUnit(unitId);
    const bool unitIsInfantry = (unit != nullptr) && m_infos->getUnitInfo(unit->iType).infantry;

    for (int searchRadius = 1; searchRadius <= 9; searchRadius++) {
        float bestDistance = 9999;
        int closestCell = -1;

        // Only iterate the outer ring of the square at this radius.
        // Top and bottom rows (full width), then left/right columns (inner height only).
        auto tryCell = [&](int cx, int cy) {
            int candidateCell = m_mapGeometry->getCellWithMapDimensions(cx, cy);
            if (candidateCell < 0) return;
            if (m_map->getCellIdStructuresLayer(candidateCell) >= 0) return;
            if (m_map->getCellIdUnitLayer(candidateCell) >= 0) return;
            int terrainType = m_map->getCellType(candidateCell);
            if (terrainType == TERRAIN_WALL) return;
            if (!unitIsInfantry && terrainType == TERRAIN_MOUNTAIN) return;
            float dist = ABS_length(cx, cy, originX, originY);
            if (dist < bestDistance) {
                bestDistance = dist;
                closestCell = candidateCell;
            }
        };

        const int minX = targetX - searchRadius;
        const int maxX = targetX + searchRadius;
        const int minY = targetY - searchRadius;
        const int maxY = targetY + searchRadius;

        // Clamp to map bounds
        int clampedMinX = minX, clampedMinY = minY, clampedMaxX = maxX, clampedMaxY = maxY;
        cPoint::split(clampedMinX, clampedMinY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(minX, minY);
        cPoint::split(clampedMaxX, clampedMaxY) = m_mapGeometry->fixCoordinatesToBeWithinPlayableMap(maxX, maxY);

        // Top row
        if (minY >= clampedMinY)
            for (int x = clampedMinX; x <= clampedMaxX; x++) tryCell(x, minY);
        // Bottom row
        if (maxY <= clampedMaxY)
            for (int x = clampedMinX; x <= clampedMaxX; x++) tryCell(x, maxY);
        // Left column (excluding corners)
        if (minX >= clampedMinX)
            for (int y = clampedMinY + 1; y <= clampedMaxY - 1; y++) tryCell(minX, y);
        // Right column (excluding corners)
        if (maxX <= clampedMaxX)
            for (int y = clampedMinY + 1; y <= clampedMaxY - 1; y++) tryCell(maxX, y);

        if (closestCell > -1)
            return closestCell;
    }

    return targetCell;
}

void cPathFinder::resetPathCreatedByUnit()
{
    m_pathsCreated = 0;
}