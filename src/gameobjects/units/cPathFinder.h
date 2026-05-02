#pragma once

#include <vector>

struct ASTAR {
    int cost;
    int parent;
    int state;
};

class cUnit;

class cPathFinder {
public:
    cPathFinder() = default;
    ~cPathFinder() = default;
    int createPath(int iUnitId, int iPathCountUnits);
    int returnCloseGoal(int iCll, int iMyCell, int iID);
    void resize(int newSize);
private:
    cUnit *m_activeUnit = nullptr;
    int m_currentCell = -1;
    int m_goalCell = -1;
    int m_controller = -1;
    int m_pathCountUnits = 0;
    bool m_valid = false;
    bool m_success = false;

    std::vector<ASTAR> m_pathMap;
    // static void verifyPathContiguity(const cUnit* pUnit, int firstCell);
};