#pragma once

#include <vector>

class cPathFinder {
public:
    //std::vector<int> createPath(int startCell, int goalCell);
    static int createPath(int iUnitId, int iPathCountUnits);
    static int returnCloseGoal(int iCll, int iMyCell, int iID);
private: 

};