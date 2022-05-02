#include "cPathFinder.h"

#include "utils/cProfiler.h"

#include "player/cPlayer.h"
#include "gameobjects/units/cUnit.h"
#include "d2tmc.h"
#include "data/gfxdata.h"

#include <thread>
#include <sys/time.h>

cPathNode::cPathNode(int _x, int _y, int _cell) {
    x = _x;
    y = _y;
    parent = nullptr;
    hCost = 0;
    gCost = 0;
    cell = _cell;
}

cPathFinder::cPathFinder(cMap *map) :
    m_map(map),
    grid(m_map->getWidth(), m_map->getHeight())
{
    int maxCells = m_map->getWidth() * m_map->getHeight();
    for (int cell = 0; cell < maxCells; cell++) {
        int x = m_map->getCellX(cell);
        int y = m_map->getCellY(cell);

        grid.grid[cell] = std::make_shared<cPathNode>(x, y, cell);
    }
}

cPathNodeGrid::cPathNodeGrid(int width, int height)
{
    int maxCells = width * height;
    grid.clear();
    grid = std::vector<node_ptr>(maxCells);
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
    pathProfiler.reset();
    pathProfiler.start("findPath");

    bool *visited = new bool[m_map->getMaxCells()] {false};

    node_ptr startNode = getPathNodeFromMapCell(startCell);
    node_ptr targetNode = getPathNodeFromMapCell(targetCell);

    startNode->hCost = getDistance(startNode.get(), targetNode.get());

    // used for finding a path
    std::vector<node_ptr> openSet = std::vector<node_ptr>();
    std::set<node_ptr> closedSet = std::set<node_ptr>();
    openSet.push_back(startNode);

    // thoughts:
    // do not use shared_ptr, but cell nrs?
    // have some kind of 'map' where we put in the scores and get neighbours from? (so like a one -dimensional array?)
    // so we can read from there and set the f/h/gcost thing?
    //  - downside: it would be way more memory intensive? although, path finding is done for each unit
    //              - so we could re-initialize that temp map? (like how we did it in the old implementation?)
    // get rid of as much loops as needed
    // build own 'set' or something, to quickly find the lowest fcost? (with the binary search thingy?)

    int bailoutCounter = 0;

    node_ptr closestPathNode = startNode;

    while (!openSet.empty()) {
        bailoutCounter++;
        node_ptr bestNextNode = openSet[0]; // by default the current is the 1st in the list

        if (bailoutCounter > 5000) {
            break; // bail to safe ourselves from hanging (need to find out why it happens though)
        }

        // unless we have a better candidate in our open set
        pathProfiler.start("findingBestFCostNode");
        for (unsigned int i = 1; i < openSet.size(); i++) {
            std::shared_ptr<cPathNode> other = openSet[i];
            if (other->fCost() < bestNextNode->fCost() || (other->fCost() == bestNextNode->fCost() && other->hCost < bestNextNode->hCost)) {
                bestNextNode = other;
            }
        }
        pathProfiler.sample("findingBestFCostNode");

        // put it into our closedSet first
        closedSet.insert(bestNextNode);
        int cell = bestNextNode->cell;
        visited[cell] = true;

        // remove current node from openSet
        openSet.erase(std::remove(openSet.begin(), openSet.end(), bestNextNode), openSet.end());

        if (bestNextNode->isAt(targetNode.get())) {
            // found target; start tracing back our path from closedSet
            closestPathNode = bestNextNode;
            break;
        }

        // closest node for cases when target cannot be reached
        if (bestNextNode->hCost < closestPathNode->hCost) {
            closestPathNode = bestNextNode;
        }

        pathProfiler.start("find neighbours");
        const std::vector<int> &neighbours = m_map->getNeighbours(bestNextNode->x, bestNextNode->y);
        pathProfiler.sample("find neighbours");

        pathProfiler.start("iterate neighbours");
        for (const auto & neighbourCell : neighbours) {
            bool nodeInClosedSet = visited[neighbourCell];
            if (nodeInClosedSet) {
                // skip nodes we have already 'closed'
                continue;
            }

            pathProfiler.start("iterate neighbours - is blocked");
            // else, check if it is blocked/walkable
            if (isBlocked(pUnit, neighbourCell)) {
                continue;
            }
            pathProfiler.sample("iterate neighbours - is blocked");

            pathProfiler.start("iterate neighbours - get path node from map cell");
            std::shared_ptr<cPathNode> neighbourNode = grid.grid[neighbourCell];
            pathProfiler.sample("iterate neighbours - get path node from map cell");

            int newMovementCost = bestNextNode->gCost + getDistance(bestNextNode.get(), neighbourNode.get());
            pathProfiler.start("iterate neighbours - find neighbour in open set");
            const std::vector<std::shared_ptr<cPathNode>>::iterator &iter = std::find_if(openSet.begin(), openSet.end(),
                                                                                         [&](const std::shared_ptr<cPathNode> &pathNode) {
                                                                                             return pathNode->isAt(
                                                                                                     neighbourNode.get());
                                                                                         });
            auto nodeInOpenSet = iter != openSet.end();
            pathProfiler.sample("iterate neighbours - find neighbour in open set");
//            bool nodeInOpenSet = std::find(openSet.begin(), openSet.end(), neighbourNode.isAt()) != openSet.end();
            if (!nodeInOpenSet ||  // not in set , so put it in there
                newMovementCost < neighbourNode->gCost // Or it is there, and it is less costly
                ) {
                int hCost = getDistance(neighbourNode.get(), targetNode.get());

                if (hCost > closestPathNode->hCost * 3) {
                    // don't keep evaluating endlessly the entire map
                    continue;
                }

                if (nodeInOpenSet) {
                    // replace current
                    cPathNode *pNode = iter->get();
                    pNode->gCost = newMovementCost;
                    pNode->hCost = hCost;
                    pNode->parent = bestNextNode;
                } else {
                    // add
                    neighbourNode->gCost = newMovementCost;
                    neighbourNode->hCost = hCost;
                    neighbourNode->parent = bestNextNode;

                    openSet.push_back(neighbourNode);
                }
            }
        }
        pathProfiler.sample("iterate neighbours");
    }

    delete[] visited;

    auto path = std::vector<int>();

    std::shared_ptr<cPathNode> currentNode = closestPathNode;
    while (currentNode != startNode) {
        path.push_back(m_map->makeCell(currentNode->x, currentNode->y));
        currentNode = currentNode->parent;
    }
    std::reverse(path.begin(), path.end());


    cPath result;
    result.waypoints = path;

    pathProfiler.stop("findPath");
    pathProfiler.printResults();
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

node_ptr cPathFinder::getPathNodeFromMapCell(int cell) {
    return grid.grid[cell];
}

bool cPathFinder::isBlocked(const cUnit &pUnit, const int cell) const {
    if (pUnit.isSandworm()) {
        return m_map->isCellPassableForWorm(cell);
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

    // is not visible, always good (since we don't know yet if its blocked!)
    if (map.isVisible(cell, pUnit.iPlayer)) {
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
    }

    // not blocked
    return false;
}

bool cPath::success() const {
    return !waypoints.empty();
}
