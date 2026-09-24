#pragma once

#include "game/MissionStats.h"

struct s_GameEvent;
class cPlayers;

class cMissionStatsCollector {
public:
    explicit cMissionStatsCollector(cPlayers *players);
    void onNotifyGameEvent(const s_GameEvent &event);
    [[nodiscard]] MissionStats snapshot(uint64_t elapsedSeconds) const;

private:
    cPlayers *m_players;
    std::array<PlayerMissionStats, MAX_PLAYERS> m_playerStats;
};
