/**
 * @file cMissionStatsCollector.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
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
