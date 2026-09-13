#pragma once

#include "game/MissionStats.h"

struct s_GameEvent;

class cMissionStatsCollector {
public:
    void missionInit();
    void onNotifyGameEvent(const s_GameEvent &event);
    [[nodiscard]] MissionStats snapshot() const;

private:
    int m_unitsBuilt = 0;
    int m_unitsLost = 0;
    int m_structuresBuilt = 0;
    int m_structuresLost = 0;
    uint64_t m_missionStartTicks = 0;
};
