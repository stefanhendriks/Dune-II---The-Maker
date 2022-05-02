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

    int fGlobalGoal; // cost from start node to this node + any other heuristic
    int fLocalGoal; // cost from this node to target node (fcost)

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

    private:
        cMap *m_map;
        cPathNode *grid = nullptr;

        int getDistance(const cPathNode * from, const cPathNode * to) const;

        bool isBlocked(const cUnit &pUnit, const int cell) const;
};

