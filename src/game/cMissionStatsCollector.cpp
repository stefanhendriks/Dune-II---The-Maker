#include "game/cMissionStatsCollector.h"

#include "include/sGameEvent.h"
#include "gameobjects/players/cPlayer.h"

#include <SDL3/SDL.h>

void cMissionStatsCollector::missionInit()
{
    m_unitsBuilt = 0;
    m_unitsLost = 0;
    m_structuresBuilt = 0;
    m_structuresLost = 0;
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
        return;
    }

    if (commonEvent->player == nullptr || !commonEvent->player->isHuman()) {
        return;
    }

    if (event.eventType == eGameEventType::GAME_EVENT_CREATED) {
        if (commonEvent->isReinforce) {
            return; // reinforcements were not built by the player this mission
        }
        if (commonEvent->entityType == eBuildType::UNIT) {
            m_unitsBuilt++;
        }
        else if (commonEvent->entityType == eBuildType::STRUCTURE) {
            m_structuresBuilt++;
        }
    }
    else { // GAME_EVENT_DESTROYED
        if (commonEvent->entityType == eBuildType::UNIT) {
            m_unitsLost++;
        }
        else if (commonEvent->entityType == eBuildType::STRUCTURE) {
            m_structuresLost++;
        }
    }
}

MissionStats cMissionStatsCollector::snapshot() const
{
    MissionStats stats;
    stats.unitsBuilt = m_unitsBuilt;
    stats.unitsLost = m_unitsLost;
    stats.structuresBuilt = m_structuresBuilt;
    stats.structuresLost = m_structuresLost;
    stats.elapsedSeconds = (SDL_GetTicks() - m_missionStartTicks) / 1000;
    return stats;
}
