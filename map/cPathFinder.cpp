#include "cPathFinder.h"

#include "utils/cProfiler.h"

#include "player/cPlayer.h"
#include "gameobjects/units/cUnit.h"
#include "d2tmc.h"
#include "data/gfxdata.h"

#include <thread>
#include <sys/time.h>
#include <cmath>
#include <queue>

cPathFinder::cPathFinder(cMap *map) :
    m_map(map)
{
    int width = map->getWidth();
    int height = map->getHeight();
    int maxCells = width * height;
    start = nullptr;
    end = nullptr;

    grid = new cPathNode[maxCells];

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int cell = (y * width) + x; // duplicated logic (also in cMap::make_cell)
            cPathNode &node = grid[cell];
            node.x = x;
            node.y = y;
            node.cell = cell;
            node.visited = false;
            node.parent = nullptr;
        }
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int cell = (y * width) + x; // duplicated logic (also in cMap::make_cell)
            cPathNode &node = grid[cell];

            if (y > 1)
                node.neighbours.push_back(&grid[(y - 1) * width + (x + 0)]);
            if (y < height - 2)
                node.neighbours.push_back(&grid[(y + 1) * width + (x + 0)]);
            if (x > 1)
                node.neighbours.push_back(&grid[(y + 0) * width + (x - 1)]);
            if (x < width - 2)
                node.neighbours.push_back(&grid[(y + 0) * width + (x + 1)]);

            // We can also connect diagonally
            if (y>1 && x>1)
                node.neighbours.push_back(&grid[(y - 1) * width + (x - 1)]);
            if (y<height-2 && x>1)
                node.neighbours.push_back(&grid[(y + 1) * width + (x - 1)]);
            if (y>1 && x<width-2)
                node.neighbours.push_back(&grid[(y - 1) * width + (x + 1)]);
            if (y<height - 2 && x<width-2)
                node.neighbours.push_back(&grid[(y + 1) * width + (x + 1)]);
        }
    }
}

bool cPathFinder::isStart(int cell) {
    if (!start) return false;
    return start->cell == cell;
}

bool cPathFinder::isEnd(int cell) {
    if (!end) return false;
    return end->cell == cell;
}

bool cPathFinder::isVisited(int cell) {
    return grid[cell].visited;
}

bool cPathFinder::isPathCell(int cell) {
    return std::find(path.waypoints.begin(), path.waypoints.end(), cell) != path.waypoints.end();
}

cPathFinder::~cPathFinder() {
    delete[] grid;
}

cProfiler pathProfiler;

/**
 * Returns a path between start and target cell. When no path can be found, the returned vector will be empty.
 *
 * @param startCell
 * @param targetCell
 * @return
 */
cPath cPathFinder::findPath(int startCell, int targetCell, cUnit & pUnit) {
    for (int c = 0; c < m_map->getMaxCells(); c++) {
        cPathNode &node = grid[c];
        node.visited = false;
        node.gCost = INT32_MAX;
        node.hCost = INT32_MAX;
        node.fCost = INT32_MAX;
        node.parent = nullptr;
    }

    cPathNode *startNode = &grid[startCell];
    cPathNode *targetNode = &grid[targetCell];

    start = startNode;
    end = targetNode;

    startNode->gCost = 0;
    startNode->hCost = getDistance(startNode, targetNode);
    startNode->fCost = startNode->hCost;

    // used for finding a path
    // So that we have a min priority queue based on the fCost
    struct compare {
        bool operator()(cPathNode* l, cPathNode* r) {
            return l->fCost > r->fCost;
        }
    };

    std::priority_queue<cPathNode *, std::vector<cPathNode *>, compare> notTestedNodes;
    notTestedNodes.push(startNode);

    int giveUpThreshold = 4;

    cPathNode *closestPathNode = startNode;
    cPathNode *currentNode = nullptr;

    while (!notTestedNodes.empty()) {

        // remove any visited nodes
        while(!notTestedNodes.empty()) {
            cPathNode *topNode = notTestedNodes.top();
            if (topNode->visited) {
                notTestedNodes.pop();
            } else {
                break;
            }
        }

        if (notTestedNodes.empty())
            break;

        currentNode = notTestedNodes.top();
        currentNode->visited = true; // We only explore a node once

        // closest node for cases when target cannot be reached
        if (currentNode->hCost < closestPathNode->hCost) {
            closestPathNode = currentNode;
        }

        if (currentNode == targetNode) {
            closestPathNode = currentNode;
            break;
        }

        for (auto nodeNeighbour: currentNode->neighbours) {
            // only evaluate non-visited nodes
            if (!nodeNeighbour->visited) {
                int distanceToNeighbor = getDistance(currentNode, nodeNeighbour);
                // remember currentNode at start is the startNode, hence, this will be
                // 0 + distanceToNeighbor, which will be lower than the current neighbor gCost, as
                // that is set to MAX_INT32.
                int nextMoveCost = currentNode->gCost + distanceToNeighbor;

                int hCost = getDistance(nodeNeighbour, targetNode);

                if (hCost > closestPathNode->hCost * giveUpThreshold) {
                    // don't keep evaluating endlessly the entire map
                    continue;
                }

                nodeNeighbour->hCost = hCost;

                if (!isBlocked(pUnit, nodeNeighbour->cell)) {
                    // the neighbor gCost initially is MAX_INT32, but could later be updated. Hence, this
                    // will only update if we have found a lower gCost
                    if (nextMoveCost < nodeNeighbour->gCost) {
                        nodeNeighbour->parent = currentNode;
                        nodeNeighbour->gCost = nextMoveCost;
                        nodeNeighbour->fCost = hCost + nextMoveCost;
                    }
                    notTestedNodes.push(nodeNeighbour);
                }
            }
        }
    }

    cPath result;
    if (closestPathNode) {
        cPathNode *theCurrentNode = closestPathNode;
        while (theCurrentNode != startNode) {
            result.waypoints.push_back(theCurrentNode->cell);
            theCurrentNode = theCurrentNode->parent;
        }
        std::reverse(result.waypoints.begin(), result.waypoints.end());
        this->path = result; // copy path result for rendering
    }
    return result;
}

/**
 * Returns distance by looking at 'nodes' (ie, cells). Each movement up/down/left/right has a score of 10. Diagonal
 * is considered more 'expensive' and counts as a distance of 14.
 *
 * @param from
 * @param to
 * @return
 */
int cPathFinder::getDistance(const cPathNode * from, const cPathNode * to) const {
    pathProfiler.start("getdistance");

    int distanceX = std::abs(from->x - to->x);
    int distanceY = std::abs(from->y - to->y);

    if (distanceX > distanceY) {
        return 14 * distanceY + (10 * (distanceX - distanceY));
    }

    int result = 14 * distanceX + (10 * (distanceY - distanceX));
    pathProfiler.sample("getdistance");
    return result;
}

bool cPathFinder::isBlocked(const cUnit &pUnit, const int cell) const {
    if (pUnit.isSandworm()) {
        return m_map->isCellPassableForWorm(cell);
    }

    if (!map.isVisible(cell, pUnit.iPlayer)) {
        // regard invisible tiles as passable
        return false;
    }

    int idOfStructureAtCell = m_map->getCellIdStructuresLayer(cell);
    int idOfUnitAtCell = m_map->getCellIdUnitLayer(cell);

    if (idOfStructureAtCell > -1) {
        // when the cell is a structure, but it is not our target structure, then we're blocked
        if (pUnit.iStructureID > -1) { // structureId when we want to enter a structure
            if (idOfStructureAtCell != pUnit.iStructureID) { // not same structure
                return true; // so .. blocked
            }
        } else if (pUnit.iAttackStructure > -1) { // structure we want to attack
            if (idOfStructureAtCell != pUnit.iAttackStructure) { // other than our target
                return true; // so .. blocked
            }
        }

        // by default blocked by structures
        // TODO: what about infantry?
        return true;
    }

    // blocked by other than our own pUnit
    if (idOfUnitAtCell > -1) {
        // occupied by a different pUnit than ourselves
        if (idOfUnitAtCell != pUnit.iID) {
            int iUID = idOfUnitAtCell;

            cUnit &unitAtCell = unit[iUID];
            if (!unitAtCell.getPlayer()->isSameTeamAs(pUnit.getPlayer())) {
                // allow running over enemy infantry/squishable units
                if (unitAtCell.isInfantryUnit() &&
                    !pUnit.canSquishInfantry()) // and the current pUnit cannot squish
                {
                    return true; // not allowed
                }
            }

            return true; // not allowed (blocked by own units)
            // it is not good, other pUnit blocks
        }
    }

    // walls stop us
    int cellType = map.getCellType(cell);
    if (cellType == TERRAIN_WALL) {
        return true;
    }

    // When we are infantry, we move through mountains. However, normal units do not
    if (!pUnit.isInfantryUnit()) {
        if (cellType == TERRAIN_MOUNTAIN) {
            return true;
        }
    }

    // not blocked
    return false;
}

bool cPath::success() const {
    return !waypoints.empty();
}
