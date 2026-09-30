#pragma once

#include "game/MissionStats.h"
#include "include/enums.h"

struct s_GameEvent;
class cPlayer;
class cGameObjectContext;

class cMissionStatsCollector {
public:
    explicit cMissionStatsCollector(cGameObjectContext *objects);
    void onNotifyGameEvent(const s_GameEvent &event);
    [[nodiscard]] MissionStats snapshot(uint64_t elapsedSeconds) const;

private:
    // Resolves who dealt damage/a killing blow, by id + kind. Returns nullptr when there was no
    // attacker (self-destruct, sandworm, debug kill) or it no longer exists.
    cPlayer *resolveOriginPlayer(int originId, eBuildType originType) const;

    cGameObjectContext *m_objects;
    std::array<PlayerMissionStats, MAX_PLAYERS> m_playerStats;
};
