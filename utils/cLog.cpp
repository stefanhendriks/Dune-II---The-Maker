#include "cLog.h"

#include "definitions.h"

#include <cstdio>
#include <ctime>
#include <system_error>

extern bool	bDoDebug;

namespace {

std::string getLogLevelString(eLogLevel level) {
    // LOG_TRACE, LOG_WARN, LOG_ERROR, LOG_FATAL
    switch (level) {
        case LOG_FATAL:
            return "FATAL";
        case LOG_WARN:
            return "WARN";
        case LOG_ERROR:
            return "ERROR";
        case LOG_TRACE:
            return "TRACE";
        case LOG_INFO:
            return "INFO";
    }

    return "UNIDENTIFIED";
}

std::string getLogComponentString(eLogComponent component) {
    switch(component) {
        case COMP_UNITS:
            return "UNITS";
        case COMP_STRUCTURES:
            return "STRUCTURES";
        case COMP_GAMEINI:
            return "GAMEINI";
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
        case COMP_SIDEBAR:
            return "SIDEBAR";
        case COMP_NONE:
            return "NONE";
        case COMP_SETUP:
            return "SETUP";
        case COMP_INIT:
            return "INIT";
        case COMP_ALLEGRO:
            return "ALLEGRO";
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
    }

    return std::string("UNIDENTIFIED");
}

std::string getLogOutcomeString(eLogOutcome outcome) {
    switch (outcome) {
        case OUTC_SUCCESS:
            return "SUCCESS";
        case OUTC_FAILED:
            return "FAILED";
        case OUTC_NONE:
            return "NONE";
        case OUTC_UNKNOWN:
            return "UNKNOWN";
        case OUTC_IGNOREME:
            return "IGNOREME";
    }

    return "UNIDENTIFIED";
}

std::string getLogHouseString(int houseId) {
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
    return "UNIDENTIFIED";
}

}

cLogger* cLogger::getInstance() {
    // The logger is initialized once as soon as this function is called for the first time.
    static cLogger theLogger;
    return &theLogger;
}

void cLogger::log(eLogLevel level, eLogComponent component, const std::string& event, const std::string& message) {
    log(level, component, event, message.c_str(), OUTC_IGNOREME, -1, -1);
}

void cLogger::log(eLogLevel level, eLogComponent component, const std::string& event, const std::string& message, eLogOutcome outcome) {
    log(level, component, event, message.c_str(), outcome, -1, -1);
}

//	Timestamp | Level | Component | House (if component requires) | ID (if component requires) | Message | Outcome | Event | Event fields...
void cLogger::log(eLogLevel level, eLogComponent component, const std::string& event, const std::string& message, eLogOutcome outcome, int playerId, int houseId) {
    if (level == LOG_TRACE) {
        if (!DEBUGGING) {
            // trace level is only in debug mode
            return;
        }
    }

    auto diffTime = getTimeInMilisDifference();

    // log line starts with time
    std::string logline = std::to_string(diffTime);
    logline += "|";

    logline += getLogLevelString(level);
    logline += "|";

    logline += getLogComponentString(component);
    logline += "|";

    if (playerId >= GENERALHOUSE) {
        logline += getLogHouseString(houseId);
        logline += "|";
    }

    if (playerId >= HUMAN) {
        logline += std::to_string(playerId);
        logline += "|";
    }

    logline += message;
        logline += "|";

    if (outcome != OUTC_IGNOREME) {
        logline += getLogOutcomeString(outcome);
        logline += "|";
    }

    logline += event;

    fprintf(file, "%s\n", logline.c_str()); // print the text into the file
}

void cLogger::logCommentLine(const std::string& txt) {
    fprintf(file, "\\\\%s\n", txt.c_str()); // print the text into the file
}

void cLogger::logHeader(const std::string& txt) {
    auto str = txt.substr(0, 79);
    auto line = std::string(str.length(), '-');

    logCommentLine(line);
    logCommentLine(str);
    logCommentLine(line);
}


cLogger::cLogger() : file(std::fopen("log.txt", "wt")), startTime(std::clock()) {
    if (file == nullptr) {
        // This translates the POSIX error number into a C++ exception
        throw std::system_error(errno, std::generic_category());
    }
}

cLogger::~cLogger() {
    fclose(file);
}

/* From 1970-01-01T00:00:00 */
long cLogger::getTimeInMilisDifference() const {
    long time_taken_millis = (std::clock() - startTime) * 1E3 / CLOCKS_PER_SEC;
    return time_taken_millis;
}
