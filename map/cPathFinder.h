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

class cPathNode {

public:
    cPathNode(int _x, int _y);

    int x, y;

    int fCost() const {
        return gCost + hCost;
    }

    node_ptr parent;

    int hCost, gCost;

    bool isAt(cPathNode * other) const {
        return x == other->x && y == other->y;
    }

};

class cPathFinder {

    public:
        explicit cPathFinder(cMap *map);

        cPath findPath(int startCell, int targetCell, cUnit & pUnit);

    private:
        cMap *m_map;

        node_ptr getPathNodeFromMapCell(int cell);

        int getDistance(const cPathNode * from, const cPathNode * to) const;

        bool isBlocked(const cUnit &pUnit, const int cell) const;
};

