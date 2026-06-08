#ifndef D2TM_UTILS_LOG_TPP
#define D2TM_UTILS_LOG_TPP

#include "utils/Log.h"

#include <utility>

template <typename... Args>
void cLog::error(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args)
{
    doLog(LOG_ERROR, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::warn(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args)
{
    doLog(LOG_WARN, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::trace(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args)
{
    doLog(LOG_TRACE, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::fatal(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args)
{
    doLog(LOG_FATAL, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::info(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args)
{
    doLog(LOG_INFO, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::log(eLogLevel level, eLogComponent component, std::string_view event, int playerId, int houseId, std::format_string<Args...> fmt, Args&&... args)
{
    doLog(level, component, event, std::format(fmt, std::forward<Args>(args)...), playerId, houseId);
}

#endif
