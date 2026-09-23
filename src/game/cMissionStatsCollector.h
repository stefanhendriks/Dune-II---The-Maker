#pragma once

#include "game/MissionStats.h"

struct s_GameEvent;

class cMissionStatsCollector {
public:
    void onNotifyGameEvent(const s_GameEvent &event);
    [[nodiscard]] MissionStats snapshot() const;

private:
    std::array<PlayerMissionStats, MAX_PLAYERS> m_playerStats;
    uint64_t m_missionStartTicks = 0;
};
