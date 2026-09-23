#pragma once

#include "game/MissionStats.h"

struct s_GameEvent;

class cMissionStatsCollector {
public:
    void onNotifyGameEvent(const s_GameEvent &event);
    [[nodiscard]] MissionStats snapshot(uint64_t elapsedSeconds) const;

private:
    std::array<PlayerMissionStats, MAX_PLAYERS> m_playerStats;
};
