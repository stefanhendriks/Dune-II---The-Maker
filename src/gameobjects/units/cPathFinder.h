#pragma once

#include <vector>

struct ASTAR {
    int cost;
    int parent;
    int state;
};

class cUnit;
struct sGameServices;
class cGameSettings;
class cGameObjectContext;
class cGameInfoContext;
class cLog;
class cInfoContext;
class cMap;
class MapGeometry;

class cPathFinder {
public:
    cPathFinder();
    ~cPathFinder() = default;
    int createPath(int unitId, int pathCountUnitsBudget);
    int createPathToFirstReachableWaypointAndAppendExisting(int unitId);
    int returnCloseGoal(int targetCell, int originCell, int unitId);
    void resize(int newSize);
    void serviceInit(sGameServices* services);
    void resetPathCreatedByUnit();
private:
    cGameSettings *m_settings = nullptr;
    cGameObjectContext *m_objects = nullptr;
    cInfoContext *m_infos = nullptr;
    cLog *m_log = nullptr;
    cMap *m_map = nullptr;
    MapGeometry* m_mapGeometry = nullptr;
    int validateCreatePathInput(int unitId);
    void initializeCreatePathSearch(int pathCountUnitsBudget);
    void executeCreatePathSearch();
    int backtracePathToTempBuffer();
    void applyTempPathToUnit(int backtracedPathLength);

    cUnit *m_activeUnit = nullptr;
    int m_currentCell = -1;
    int m_goalCell = -1;
    int m_controller = -1;
    int m_pathCountUnits = 0;
    bool m_valid = false;
    bool m_success = false;
    int m_pathsCreated = 0;
    bool m_isApplyingFirstSegmentCorrection = false;

    std::vector<ASTAR> m_pathMap;
    std::vector<int> m_tempPath;
    // static void verifyPathContiguity(const cUnit* pUnit, int firstCell);
};