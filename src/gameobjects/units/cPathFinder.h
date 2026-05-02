#pragma once

#include <vector>

struct ASTAR {
    int cost;
    int parent;
    int state;
};

// class cUnit;

class cPathFinder {
public:
    cPathFinder() = default;
    ~cPathFinder() = default;
    int createPath(int iUnitId, int iPathCountUnits);
    int returnCloseGoal(int iCll, int iMyCell, int iID);
    void resize(int newSize);
private:
    std::vector<ASTAR> temp_map;
    // static void verifyPathContiguity(const cUnit* pUnit, int firstCell);
};