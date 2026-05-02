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
    int createPath(int iUnitId, int iPathCountUnits);
    int returnCloseGoal(int iCll, int iMyCell, int iID);
private:
    ASTAR temp_map[16384];
    // static void verifyPathContiguity(const cUnit* pUnit, int firstCell);
};