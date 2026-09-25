#include "game/cMissionStatsCollector.h"

#include "context/cGameObjectContext.h"
#include "include/sGameEvent.h"
#include "gameobjects/players/cPlayer.h"
#include "gameobjects/structures/cAbstractStructure.h"
#include "gameobjects/units/cUnit.h"
#include "utils/Log.h"

cMissionStatsCollector::cMissionStatsCollector(cGameObjectContext *objects) : m_objects(objects)
{
}

cPlayer *cMissionStatsCollector::resolveOriginPlayer(int originId, eBuildType originType) const
{
    if (originId < 0) {
        return nullptr;
    }
    if (originType == eBuildType::UNIT) {
        cUnit *pUnit = m_objects->getUnit(originId);
        return (pUnit != nullptr && pUnit->isValid()) ? pUnit->getPlayer() : nullptr;
    }
    if (originType == eBuildType::STRUCTURE) {
        cAbstractStructure *pStructure = m_objects->getStructure(originId);
        return pStructure != nullptr ? pStructure->getPlayer() : nullptr;
    }
    return nullptr;
}

void cMissionStatsCollector::onNotifyGameEvent(const s_GameEvent &event)
{
    if (event.eventType == eGameEventType::GAME_EVENT_ABOUT_TO_BEGIN) {
        // Reset here, not at mission setup: the scenario's starting units/structures are placed
        // between mission setup and this event, and must not be counted as player-built.
        m_playerStats = {};
        // Capture each player's house now, before any of them can be defeated: cGamePlaying
        // resets a defeated player's house to GENERALHOUSE as part of elimination cleanup, so
        // reading it live at scoring time would hide anyone the player actually beat.
        for (int playerId = 0; playerId < MAX_PLAYERS; playerId++) {
            const cPlayer *player = m_objects->getPlayer(playerId);
            if (player != nullptr) {
                m_playerStats[playerId].house = player->getHouse();
                m_playerStats[playerId].minimapColor = player->getMinimapColor();
            }
        }
        return;
    }

    if (event.eventType == eGameEventType::GAME_EVENT_DAMAGED) {
        const auto *damagedEvent = std::get_if<DamagedEvent>(&event.data);
        if (damagedEvent == nullptr) {
            Logger::warn(COMP_GAME, "cMissionStatsCollector::onNotifyGameEvent", "GAME_EVENT_DAMAGED did not carry a DamagedEvent");
            return;
        }
        cPlayer *attacker = resolveOriginPlayer(damagedEvent->originId, damagedEvent->originType);
        if (attacker != nullptr && attacker->getId() >= 0 && attacker->getId() < MAX_PLAYERS) {
            m_playerStats[attacker->getId()].damageDealt += damagedEvent->damage;
        }
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

    if (event.eventType == eGameEventType::GAME_EVENT_DESTROYED) {
        cPlayer *killer = resolveOriginPlayer(commonEvent->originId, commonEvent->originType);
        if (killer != nullptr && killer->getId() >= 0 && killer->getId() < MAX_PLAYERS) {
            PlayerMissionStats &killerStats = m_playerStats[killer->getId()];
            killerStats.damageDealt += commonEvent->damage;
            if (commonEvent->entityType == eBuildType::UNIT) {
                killerStats.unitsDestroyed++;
            }
            else if (commonEvent->entityType == eBuildType::STRUCTURE) {
                killerStats.structuresDestroyed++;
            }
        }
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
