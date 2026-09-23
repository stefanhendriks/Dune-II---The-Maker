#include "game/cMissionStatsCollector.h"

#include "include/sGameEvent.h"
#include "gameobjects/players/cPlayer.h"
#include "utils/Log.h"

void cMissionStatsCollector::onNotifyGameEvent(const s_GameEvent &event)
{
    if (event.eventType == eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN) {
        // Reset here, not at mission setup: the scenario's starting units/structures are placed
        // between mission setup and this event, and must not be counted as player-built.
        m_playerStats = {};
        return;
    }

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

MissionStats cMissionStatsCollector::snapshot(uint64_t elapsedSeconds) const
{
    MissionStats stats;
    stats.players = m_playerStats;
    stats.elapsedSeconds = elapsedSeconds;
    return stats;
}
