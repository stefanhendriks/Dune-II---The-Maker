#pragma once

#include "include/sGameEvent.h"

class cGameObjectContext;
class cInfoContext;
class cStructureUtils;
class cGameInterface;

class cGameEventHandler {
public:
    cGameEventHandler(
        cGameObjectContext *gameObjectsContext,
        cInfoContext *infoContext,
        cStructureUtils *structureUtils,
        cGameInterface *gameInterface
    );

    void handleEvent(const s_GameEvent &event);

private:
    void onEventEntityDestroyed(const CommonEvent &event);
    void onEventCreateUnit(const DeployUnitEvent &event);
    void onEventSpecialLaunch(const LaunchDeathHandEvent &event);

    cGameObjectContext *m_gameObjectsContext;
    cInfoContext *m_infoContext;
    cStructureUtils *m_structureUtils;
    cGameInterface *m_gameInterface;
};
