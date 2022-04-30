#include "cPathFinder.h"

#include "player/cPlayer.h"
#include "gameobjects/units/cUnit.h"
#include "d2tmc.h"
#include "data/gfxdata.h"

#include <sys/time.h>
typedef unsigned long long timestamp_t;

static timestamp_t get_timestamp () {
//    struct timeval now;
    timeval now;
    gettimeofday (&now, nullptr);
    return now.tv_usec + (timestamp_t)now.tv_sec * 1000;
}

//double secs = (t1 - t0) / 1000000.0L;

cPathNode::cPathNode(int _x, int _y) {
    x = _x;
    y = _y;
    parent = nullptr;
    hCost = 0;
    gCost = 0;
}

cPathFinder::cPathFinder(cMap *map) :
    m_map(map)
{
}

/**
 * Returns a path between start and target cell. When no path can be found, the returned vector will be empty.
 *
 * @param startCell
 * @param targetCell
 * @return
 */
cPath cPathFinder::findPath(int startCell, int targetCell, cUnit & pUnit) {
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
    timestamp_t t0 = get_timestamp();
    timestamp_t t_findingBestFCostNode = 0;
    timestamp_t t_totalFindingNeighborNodes = 0;

    node_ptr closestPathNode = startNode;

    while (!openSet.empty()) {
        bailoutCounter++;
        node_ptr bestNextNode = openSet[0]; // by default the current is the 1st in the list

//        if (bailoutCounter > 5000) {
//            break; // bail to safe ourselves from hanging (need to find out why it happens though)
//        }

        timestamp_t t0 = get_timestamp();
        // unless we have a better candidate in our open set
        for (unsigned int i = 1; i < openSet.size(); i++) {
            std::shared_ptr<cPathNode> other = openSet[i];
            if (other->fCost() < bestNextNode->fCost() || (other->fCost() == bestNextNode->fCost() && other->hCost < bestNextNode->hCost)) {
                bestNextNode = other;
            }
        }
        timestamp_t t1 = get_timestamp();
        t_findingBestFCostNode += (t1-t0);

        logbook(fmt::format("Performance 'unless we have a better candidate in our open set' {} - {} = {}", t0, t1, (t1-t0)));


        // put it into our closedSet first
        closedSet.insert(bestNextNode);

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

        const std::vector<int> &neighbours = m_map->getNeighbours(bestNextNode->x, bestNextNode->y);

        t0 = get_timestamp();

        timestamp_t t00;
        timestamp_t tFindingNeighborNodeInClosedList = 0;
        for (const auto & neighbourCell : neighbours) {
            std::shared_ptr<cPathNode> neighbourNode = getPathNodeFromMapCell(neighbourCell);
            t00 = get_timestamp();
            const std::set<std::shared_ptr<cPathNode>>::iterator &iterClosed = std::find_if(closedSet.begin(), closedSet.end(),
                                                                                         [&](const std::shared_ptr<cPathNode> &pathNode) {
                                                                                             return pathNode->isAt(
                                                                                                     neighbourNode.get());
                                                                                         });

            timestamp_t t11 = get_timestamp();
            tFindingNeighborNodeInClosedList += (t11 - t00);
            t_totalFindingNeighborNodes += (t11 - t00);

            auto nodeInClosedSet = iterClosed != closedSet.end();
//            if (closedSet.contains(neighbourNode)) {
            if (nodeInClosedSet) {
                // skip nodes we have already 'closed'
                continue;
            }

            // else, check if it is blocked/walkable
            if (pUnit.isSandworm()) {

            } else {
                int idOfStructureAtCell = m_map->cellGetIdFromLayer(neighbourCell, MAPID_STRUCTURES);
                int idOfUnitAtCell = m_map->getCellIdUnitLayer(neighbourCell);

                if (idOfStructureAtCell > -1) {
                    // when the cell is a structure, and it is the structure we want to attack, it is good
                    if (pUnit.iStructureID > -1) {
                        if (idOfStructureAtCell != pUnit.iStructureID) {
                            // not allowed
                            continue;
                        }
                    } else if (pUnit.iAttackStructure > -1) {
                        if (idOfStructureAtCell != pUnit.iAttackStructure) {
                            continue;
                        }
                    } else {
                        continue; // by default blocked by structures
                    }
                }

                // blocked by other than our own unit
                if (idOfUnitAtCell > -1) {
                    // occupied by a different unit than ourselves
                    if (idOfUnitAtCell != pUnit.iID) {
                        int iUID = idOfUnitAtCell;

                        cUnit &unitAtCell = unit[iUID];
                        if (!unitAtCell.getPlayer()->isSameTeamAs(pUnit.getPlayer())) {
                            // allow running over enemy infantry/squishable units
                            if (unitAtCell.isInfantryUnit() &&
                                !pUnit.canSquishInfantry()) // and the current unit cannot squish
                            {
                                continue; // not allowed
                            }
                        } else {
                            continue; // not allowed (blocked by own units)
                        }
                        // it is not good, other unit blocks
                    }
                }

                // is not visible, always good (since we don't know yet if its blocked!)
                if (map.isVisible(neighbourCell, pUnit.iPlayer)) {
                    // walls stop us
                    int cellType = map.getCellType(neighbourCell);
                    if (cellType == TERRAIN_WALL) {
                        continue;
                    }

                    // When we are infantry, we move through mountains. However, normal units do not
                    if (!pUnit.isInfantryUnit()) {
                        if (cellType == TERRAIN_MOUNTAIN) {
                            continue;
                        }
                    }
                }
            }

            int newMovementCost = bestNextNode->gCost + getDistance(bestNextNode.get(), neighbourNode.get());
            const std::vector<std::shared_ptr<cPathNode>>::iterator &iter = std::find_if(openSet.begin(), openSet.end(),
                                                                                         [&](const std::shared_ptr<cPathNode> &pathNode) {
                                                                                             return pathNode->isAt(
                                                                                                     neighbourNode.get());
                                                                                         });
            auto nodeInOpenSet = iter != openSet.end();
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
        t1 = get_timestamp();
        logbook(fmt::format("Performance 'evaluating neighbours' {} - {} = {}, finding stuff = {}", t0, t1, (t1-t0), tFindingNeighborNodeInClosedList));
    }
    timestamp_t t1 = get_timestamp();
    logbook(fmt::format("Performance 'create path' = {}, spent {} on sorting and {} find neighboursInClosedLists", (t1-t0), t_findingBestFCostNode, t_totalFindingNeighborNodes));

    t0 = get_timestamp();
    auto path = std::vector<int>();

    std::shared_ptr<cPathNode> currentNode = closestPathNode;
    while (currentNode != startNode) {
        path.push_back(m_map->makeCell(currentNode->x, currentNode->y));
        currentNode = currentNode->parent;
    }
    std::reverse(path.begin(), path.end());
    t1 = get_timestamp();

    logbook(fmt::format("Performance 'backtrack path' {} - {} = {}", t0, t1, (t1-t0)));


    cPath result;
    result.waypoints = path;
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
    int distanceX = std::abs(from->x - to->x);
    int distanceY = std::abs(from->y - to->y);

    if (distanceX > distanceY) {
        return 14 * distanceY + (10 * (distanceX - distanceY));
    }

    return 14 * distanceX + (10 * (distanceY - distanceX));
}

node_ptr cPathFinder::getPathNodeFromMapCell(int cell) {
    int x = m_map->getCellX(cell);
    int y = m_map->getCellY(cell);

    return std::make_shared<cPathNode>(x, y);
}

bool cPath::success() const {
    return !waypoints.empty();
}
