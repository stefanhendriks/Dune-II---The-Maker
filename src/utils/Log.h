#pragma once

//just have eLogLevel, eLogComponent in this file, to avoid circular dependencies with cLog

enum eLogLevel {
    LOG_INFO,
    LOG_TRACE,
    LOG_DEBUG,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

enum eLogComponent {
    COMP_UNITS,
    COMP_STRUCTURES,
    COMP_GAMERULES,
    COMP_SCENARIOINI,
    COMP_PARTICLE,
    COMP_BULLET,
    COMP_AI,
    COMP_UPGRADE_LIST,
    COMP_BUILDING_LIST_UPDATER,
    COMP_SIDEBAR,
    COMP_MAP,
    COMP_GAMEOBJECTS,
    COMP_GAME,
    COMP_NONE,
    COMP_INIT,
    // COMP_ALLEGRO,		/** Use for allegro specific calls **/
    COMP_SDL2,        /** Use for SDL2 specific calls **/
    COMP_SETUP,
    COMP_VERSION,		/** version specific loggin messages **/
    COMP_SKIRMISHSETUP, /** When skirmish game is being set up **/
    COMP_ALFONT, 		/** ALFONT library specific **/
    COMP_SOUND, 		/** Sound related **/
    COMP_REGIONINI,		/** Used for regions **/
    COMP_PLAYER,			/** Player related **/
    COMP_GAMESTATE,			/** Used for game state changes **/
    COMP_CHEATS,            /** Used for cheat related logs **/
    COMP_BUILDER
};

#include <format>
#include <cstddef>
#include <fstream>
#include <string_view>
#include <chrono>

class cLog {
public:
    explicit cLog(std::string_view filename);
    ~cLog();

    cLog(const cLog &) = delete;
    cLog(cLog &&) = delete;
    cLog &operator=(const cLog &) = delete;
    cLog &operator=(cLog &&) = delete;

    void setDebug(bool debug);

    void log(eLogLevel level, eLogComponent component, std::string_view event, std::string_view message, int playerId = -1, int houseId = -1);

    void print(eLogLevel level, eLogComponent component, std::string_view event, std::string_view message);

    template <typename... Args>
    void error(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void warn(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void trace(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void fatal(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void debug(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void info(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);

    template <typename... Args>
    void log(eLogLevel level, eLogComponent component, std::string_view event, int playerId, int houseId, std::format_string<Args...> fmt, Args&&... args);

private:
    void doLog(eLogLevel level, eLogComponent comp, std::string_view event, std::string_view message, int playerId = -1, int houseId = -1);
    void flushPendingLine(bool forceFlush);

    std::ofstream m_file;
    std::chrono::steady_clock::time_point m_startTime;
    std::string m_pendingPayload;
    long long m_pendingTimestampMs = 0;
    std::size_t m_pendingCount = 0;
    bool m_hasPendingLine = false;
    bool m_debugMode = false;
};

// ============================================================================
// NAMESPACE GLOBAL DECLARATION (INTERFACE LOG)
// ============================================================================
namespace Logger {
    // Initialise le système avec l'instance de Log créée dans le main
    void init(cLog* instance);

    // Surcharges explicites si tu veux spécifier le composant et l'événement
    template <typename... Args>
    void print(eLogLevel level, eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
    template <typename... Args>
    void info(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
    template <typename... Args>
    void trace(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
    template <typename... Args>
    void debug(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
    template <typename... Args>
    void warn(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
    template <typename... Args>
    void error(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
    template <typename... Args>
    void fatal(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args);
}


// ============================================================================
// ============================================================================
// TEMPLATES IMPLEMENTATION  (HEADER REQUIREMENT)
// ============================================================================
// ============================================================================
#include <utility>

// --- Methodes from Log class ---
template <typename... Args>
void cLog::error(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
    doLog(LOG_ERROR, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::warn(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
    doLog(LOG_WARN, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::trace(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
    doLog(LOG_TRACE, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::fatal(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
    doLog(LOG_FATAL, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::info(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
    doLog(LOG_INFO, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

template <typename... Args>
void cLog::debug(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
    doLog(LOG_DEBUG, comp, event, std::format(fmt, std::forward<Args>(args)...));
}

// --- Globales fonctions from Namespace Logger ---
namespace Logger {
    // On indique au header que cette variable existe dans le .cpp
    extern cLog* g_loggerInstance;

    // Versions complètes avec Composant et Événement
    template <typename... Args>
    void print(eLogLevel level, eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->print(level, comp, event, std::format(fmt, std::forward<Args>(args)...));
    }
    template <typename... Args>
    void info(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->info(comp, event, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void trace(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->trace(comp, event, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void debug(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->debug(comp, event, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void warn(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->warn(comp, event, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void error(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->error(comp, event, fmt, std::forward<Args>(args)...);
    }
    template <typename... Args>
    void fatal(eLogComponent comp, std::string_view event, std::format_string<Args...> fmt, Args&&... args) {
        if (g_loggerInstance) g_loggerInstance->fatal(comp, event, fmt, std::forward<Args>(args)...);
    }
}
