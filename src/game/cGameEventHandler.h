#pragma once

#include "include/sGameEvent.h"

#include <functional>

class cGameObjectContext;
class cInfoContext;
class cStructureUtils;

class cGameEventHandler {
public:
    using EventDispatcher = std::function<void(const s_GameEvent &)>;
    using SoundPlayer = std::function<void(int)>;

    cGameEventHandler(
        cGameObjectContext *gameObjectsContext,
        cInfoContext *infoContext,
        cStructureUtils *structureUtils,
        SoundPlayer playSound,
        EventDispatcher dispatchEvent
    );

    void handleEvent(const s_GameEvent &event);

private:
    void onEventEntityDestroyed(const CommonEvent &event);
    void onEventCreateUnit(const DeployUnitEvent &event);
    void onEventSpecialLaunch(const LaunchDeathHandEvent &event);

    cGameObjectContext *m_gameObjectsContext;
    cInfoContext *m_infoContext;
    cStructureUtils *m_structureUtils;

    SoundPlayer m_playSound;
    EventDispatcher m_dispatchEvent;
};
