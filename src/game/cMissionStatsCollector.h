/**
 * @file cMissionStatsCollector.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "game/MissionStats.h"

struct s_GameEvent;

class cMissionStatsCollector {
public:
    void missionInit();
    void onNotifyGameEvent(const s_GameEvent &event);
    [[nodiscard]] MissionStats snapshot() const;

private:
    std::array<PlayerMissionStats, MAX_PLAYERS> m_playerStats;
    uint64_t m_missionStartTicks = 0;
};
