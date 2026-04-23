#include "utils/Log.h"

#include "definitions.h"

#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>

namespace {

template <typename TEnum>
std::string_view findEnumString(const std::unordered_map<TEnum, std::string_view> &values, TEnum value)
{
    const auto it = values.find(value);
    return it != values.end() ? it->second : "UNIDENTIFIED";
}

}

std::string_view getLogLevelString(eLogLevel level)
{
    static const std::unordered_map<eLogLevel, std::string_view> levelNames = {
        { LOG_FATAL, "FATAL" },
        { LOG_WARN, "WARN" },
        { LOG_ERROR, "ERROR" },
        { LOG_DEBUG, "DEBUG" },
        { LOG_TRACE, "TRACE" },
        { LOG_INFO, "INFO" },
    };

    return findEnumString(levelNames, level);
}

std::string_view getLogComponentString(eLogComponent component)
{
    static const std::unordered_map<eLogComponent, std::string_view> componentNames = {
        { COMP_UNITS, "UNITS" },
        { COMP_STRUCTURES, "STRUCTURES" },
        { COMP_GAMERULES, "GAMERULES" },
        { COMP_SCENARIOINI, "SCENARIOINI" },
        { COMP_PARTICLE, "PARTICLE" },
        { COMP_BULLET, "BULLET" },
        { COMP_AI, "AI" },
        { COMP_UPGRADE_LIST, "UPGRADE_LIST" },
        { COMP_BUILDING_LIST_UPDATER, "BUILDING_LIST_UPDATER" },
        { COMP_MAP, "MAP" },
        { COMP_GAMEOBJECTS, "GAMEOBJECTS" },
        { COMP_GAME, "GAME" },
        { COMP_SIDEBAR, "SIDEBAR" },
        { COMP_NONE, "NONE" },
        { COMP_SETUP, "SETUP" },
        { COMP_INIT, "INIT" },
        { COMP_SDL2, "SDL2" },
        { COMP_VERSION, "VERSION" },
        { COMP_SKIRMISHSETUP, "SKIRMISHSETUP" },
        { COMP_ALFONT, "ALFONT" },
        { COMP_SOUND, "SOUND" },
        { COMP_REGIONINI, "REGIONINI" },
        { COMP_PLAYER, "PLAYER" },
        { COMP_GAMESTATE, "GAMESTATE" },
        { COMP_CHEATS, "CHEATS" },
    };

    return findEnumString(componentNames, component);
}

std::string_view getLogHouseString(int houseId)
{
    static const std::unordered_map<int, std::string_view> houseNames = {
        { ATREIDES, "ATREIDES" },
        { HARKONNEN, "HARKONNEN" },
        { ORDOS, "ORDOS" },
        { FREMEN, "FREMEN" },
        { SARDAUKAR, "SARDAUKAR" },
        { MERCENARY, "MERCENARY" },
    };

    const auto it = houseNames.find(houseId);
    return it != houseNames.end() ? it->second : "UNKNOWN HOUSE";
}


cLog::cLog(std::string_view filename)
{
    m_file.open(std::string(filename), std::ofstream::out | std::ofstream::app);
    if (!m_file.is_open()) {
        throw std::system_error(errno, std::generic_category());
    }
}

cLog::~cLog()
{
    flushPendingLine(true);
}

void cLog::log(eLogLevel level, eLogComponent component, std::string_view event, std::string_view message, int playerId, int houseId)
{
    doLog(level, component, event, message, playerId, houseId);
}

void cLog::flushPendingLine(bool forceFlush)
{
    if (!m_hasPendingLine) {
        return;
    }

    if (m_pendingCount > 1) {
        m_file << std::format("{}|{}|{}", m_pendingTimestampMs, m_pendingPayload, m_pendingCount) << '\n';
    } else {
        m_file << std::format("{}|{}", m_pendingTimestampMs, m_pendingPayload) << '\n';
    }

    if (forceFlush) {
        m_file.flush();
    }

    m_pendingPayload.clear();
    m_pendingTimestampMs = 0;
    m_pendingCount = 0;
    m_hasPendingLine = false;
}

void cLog::doLog(eLogLevel level, eLogComponent comp, std::string_view event, std::string_view message, int playerId, int houseId)
{
    if (!m_debugMode && (level == LOG_TRACE || level == LOG_DEBUG)) {
        // trace and debug levels are only available in debug mode
        return;
    }

    const auto now = std::chrono::system_clock::now();
    const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::string payload = std::format("{}|{}|{}|{}|{}|{}",
        getLogLevelString(level),
        getLogComponentString(comp),
        event,
        message,
        (playerId >= 0 ? std::to_string(playerId) : ""),
        (houseId >= 0 ? getLogHouseString(houseId) : "")
    );

    if (m_debugMode && (level == LOG_ERROR || level == LOG_FATAL) ) {
        std::cout << payload << std::endl;
    }

    if (m_hasPendingLine && payload == m_pendingPayload) {
        ++m_pendingCount;
    } else {
        flushPendingLine(false);
        m_pendingPayload = std::move(payload);
        m_pendingTimestampMs = nowMs;
        m_pendingCount = 1;
        m_hasPendingLine = true;
    }

    if (level == LOG_ERROR || level == LOG_FATAL) {
        flushPendingLine(true);
    }
}
