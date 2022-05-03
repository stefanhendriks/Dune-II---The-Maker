#pragma once

#include "cMap.h"

#include <set>
#include <vector>
#include <memory>

class cPathNode;

using node_ptr = std::shared_ptr<cPathNode>;

class cPath {
public:
    std::vector<int> waypoints;

    bool success() const;
};

struct cPathNode {
    int x, y;
    int cell;

    int gCost;      // cost from start node to this node + any other heuristic
    int hCost;      // hCost
    int fCost;      // gCost + hCost;

    bool visited = false;

    std::vector<cPathNode*> neighbours;
    cPathNode *parent;

    bool isAt(cPathNode * other) const {
        return x == other->x && y == other->y;
    }
};

class cPathFinder {

    public:
        explicit cPathFinder(cMap *map);
        ~cPathFinder();

        cPath findPath(int startCell, int targetCell, cUnit & pUnit);

        bool isStart(int cell);
        bool isEnd(int cell);
        bool isVisited(int cell);
        bool isPathCell(int cell);

    private:
        cMap *m_map;
        cPathNode *grid = nullptr;
        cPathNode *start;
        cPathNode *end;
        cPath path;

        int getDistance(const cPathNode * from, const cPathNode * to) const;

        bool isBlocked(const cUnit &pUnit, const int cell) const;
};

