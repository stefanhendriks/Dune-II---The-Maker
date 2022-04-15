#ifndef D2TM_CPATHFINDER_H
#define D2TM_CPATHFINDER_H

#include "cMap.h"

#include <set>
#include <vector>
#include <memory>

class cPathNode {

public:
    cPathNode(int _x, int _y);

    int x, y;

    int fCost() const {
        return gCost + hCost;
    }

    std::shared_ptr<cPathNode> parent;

    int hCost, gCost;

    bool isAt(cPathNode * other) const {
        return x == other->x && y == other->y;
    }

};

class cPathFinder {

    public:
        explicit cPathFinder(cMap *map);

        std::vector<int> findPath(int startCell, int targetCell, cUnit & pUnit);

    private:
        cMap *m_map;

        std::shared_ptr<cPathNode> getPathNodeFromMapCell(int cell);

        int getDistance(const cPathNode * from, const cPathNode * to) const;

};


#endif //D2TM_CPATHFINDER_H
