#pragma once

#include "utils/cLog.h"

#include <format>
#include <cstddef>
#include <fstream>
#include <string_view>

class cLog {
public:
    explicit cLog(std::string_view filename);
    ~cLog();

    cLog(const cLog &) = delete;
    cLog(cLog &&) = delete;
    cLog &operator=(const cLog &) = delete;
    cLog &operator=(cLog &&) = delete;

    void log(eLogLevel level, eLogComponent component, std::string_view event, std::string_view message, int playerId = -1, int houseId = -1);

    template <typename... Args>
    void error(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void warn(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void trace(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void fatal(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void info(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void log(eLogLevel level, eLogComponent component, std::string_view event, int playerId, int houseId, std::format_string<Args...> fmt, Args&&... args);

private:
    void doLog(eLogLevel level, eLogComponent comp, std::string_view event, std::string_view message, int playerId = -1, int houseId = -1);
    void flushPendingLine(bool forceFlush);

    std::ofstream m_file;
    std::string m_pendingPayload;
    long long m_pendingTimestampMs = 0;
    std::size_t m_pendingCount = 0;
    bool m_hasPendingLine = false;
};

#include "utils/Log.tpp"