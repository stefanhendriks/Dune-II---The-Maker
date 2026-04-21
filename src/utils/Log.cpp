#include "utils/Log.h"

#include "definitions.h"

#include <array>
#include <chrono>
#include <string>
#include <string_view>
#include <system_error>
#include <iostream>


std::string getLogLevelString(eLogLevel level)
{
    // LOG_TRACE, LOG_WARN, LOG_ERROR, LOG_FATAL
    switch (level) {
        case LOG_FATAL:
            return "FATAL";
        case LOG_WARN:
            return "WARN";
        case LOG_ERROR:
            return "ERROR";
        case LOG_DEBUG:
            return "DEBUG";
        case LOG_TRACE:
            return "TRACE";
        case LOG_INFO:
            return "INFO";
    }
    return "UNIDENTIFIED";
}

std::string getLogComponentString(eLogComponent component)
{
    switch(component) {
        case COMP_UNITS:
            return "UNITS";
        case COMP_STRUCTURES:
            return "STRUCTURES";
        case COMP_GAMERULES:
            return "GAMERULES";
        case COMP_SCENARIOINI:
            return "SCENARIOINI";
        case COMP_PARTICLE:
            return "PARTICLE";
        case COMP_BULLET:
            return "BULLET";
        case COMP_AI:
            return "AI";
        case COMP_UPGRADE_LIST:
            return "UPGRADE_LIST";
        case COMP_BUILDING_LIST_UPDATER:
            return "BUILDING_LIST_UPDATER";
        case COMP_MAP:
            return "MAP";
        case COMP_GAMEOBJECTS:
            return "GAMEOBJECTS";
        case COMP_SIDEBAR:
            return "SIDEBAR";
        case COMP_NONE:
            return "NONE";
        case COMP_SETUP:
            return "SETUP";
        case COMP_INIT:
            return "INIT";
        // case COMP_ALLEGRO:
        //     return "ALLEGRO";
        case COMP_SDL2:
            return "SDL2";
        case COMP_VERSION:
            return "VERSION";
        case COMP_SKIRMISHSETUP:
            return "SKIRMISHSETUP";
        case COMP_ALFONT:
            return "ALFONT";
        case COMP_SOUND:
            return "SOUND";
        case COMP_REGIONINI:
            return "REGIONINI";
        case COMP_PLAYER:
            return "PLAYER";
        case COMP_GAMESTATE:
            return "GAMESTATE";
        case COMP_CHEATS:
            return "CHEATS";
    }
    return "UNIDENTIFIED";
}

std::string_view getLogHouseString(int houseId)
{
    switch (houseId) {
        case ATREIDES:
            return "ATREIDES";
        case HARKONNEN:
            return "HARKONNEN";
        case ORDOS:
            return "ORDOS";
        case FREMEN:
            return "FREMEN";
        case SARDAUKAR:
            return "SARDAUKAR";
        case MERCENARY:
            return "MERCENARY";
        default:
            return "UNKNOWN HOUSE";
    }
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
