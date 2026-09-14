/**
 * @file cMissionStatsCollector.cpp
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

#include "game/cMissionStatsCollector.h"

#include "include/sGameEvent.h"
#include "gameobjects/players/cPlayer.h"
#include "utils/Log.h"

#include <SDL3/SDL.h>

void cMissionStatsCollector::missionInit()
{
    m_playerStats = {};
    m_missionStartTicks = SDL_GetTicks();
}

void cMissionStatsCollector::onNotifyGameEvent(const s_GameEvent &event)
{
    if (event.eventType != eGameEventType::GAME_EVENT_CREATED &&
        event.eventType != eGameEventType::GAME_EVENT_DESTROYED) {
        return;
    }

    const auto *commonEvent = std::get_if<CommonEvent>(&event.data);
    if (commonEvent == nullptr) {
        Logger::warn(COMP_GAME, "cMissionStatsCollector::onNotifyGameEvent", "GAME_EVENT_CREATED/GAME_EVENT_DESTROYED did not carry a CommonEvent");
        return;
    }

    if (commonEvent->player == nullptr) {
        return;
    }

    const int playerId = commonEvent->player->getId();
    if (playerId < 0 || playerId >= MAX_PLAYERS) {
        return;
    }

    PlayerMissionStats &stats = m_playerStats[playerId];

    if (event.eventType == eGameEventType::GAME_EVENT_CREATED) {
        if (commonEvent->isReinforce) {
            return; // reinforcements were not built by the player this mission
        }
        if (commonEvent->entityType == eBuildType::UNIT) {
            stats.unitsBuilt++;
        }
        else if (commonEvent->entityType == eBuildType::STRUCTURE) {
            stats.structuresBuilt++;
        }
    }
    else { // GAME_EVENT_DESTROYED
        if (commonEvent->entityType == eBuildType::UNIT) {
            stats.unitsLost++;
        }
        else if (commonEvent->entityType == eBuildType::STRUCTURE) {
            stats.structuresLost++;
        }
    }
}

MissionStats cMissionStatsCollector::snapshot() const
{
    MissionStats stats;
    stats.players = m_playerStats;
    stats.elapsedSeconds = (SDL_GetTicks() - m_missionStartTicks) / 1000;
    return stats;
}
