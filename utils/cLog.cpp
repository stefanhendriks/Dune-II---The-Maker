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
            return std::string("FATAL");
        case LOG_WARN:
            return std::string("WARN");
        case LOG_ERROR:
            return std::string("ERROR");
        case LOG_TRACE:
            return std::string("TRACE");
        case LOG_INFO:
            return std::string("INFO");
    }

    return std::string("UNIDENTIFIED");
}

std::string getLogComponentString(eLogComponent component) {
    switch(component) {
        case COMP_UNITS:
            return std::string("UNITS");
        case COMP_STRUCTURES:
            return std::string("STRUCTURES");
        case COMP_GAMEINI:
            return std::string("GAMEINI");
        case COMP_SCENARIOINI:
            return std::string("SCENARIOINI");
        case COMP_PARTICLE:
            return std::string("PARTICLE");
        case COMP_BULLET:
            return std::string("BULLET");
        case COMP_AI:
            return std::string("AI");
    case COMP_UPGRADE_LIST:
      return std::string("UPGRADE_LIST");
      case COMP_BUILDING_LIST_UPDATER:
      return std::string("BUILDING_LIST_UPDATER");
        case COMP_MAP:
            return std::string("MAP");
        case COMP_SIDEBAR:
            return std::string("SIDEBAR");
        case COMP_NONE:
            return std::string("NONE");
        case COMP_SETUP:
            return std::string("SETUP");
        case COMP_INIT:
            return std::string("INIT");
        case COMP_ALLEGRO:
            return std::string("ALLEGRO");
        case COMP_VERSION:
            return std::string("VERSION");
        case COMP_SKIRMISHSETUP:
            return std::string("SKIRMISHSETUP");
        case COMP_ALFONT:
            return std::string("ALFONT");
        case COMP_SOUND:
            return std::string("SOUND");
        case COMP_REGIONINI:
            return std::string("REGIONINI");
    }

    return std::string("UNIDENTIFIED");
}

std::string getLogOutcomeString(eLogOutcome outcome) {
    switch (outcome) {
        case OUTC_SUCCESS:
            return std::string("SUCCESS");
        case OUTC_FAILED:
            return std::string("FAILED");
        case OUTC_NONE:
            return std::string("NONE");
      case OUTC_UNKNOWN:
      break;
      case OUTC_IGNOREME:
      break;
    }
     return std::string("UNIDENTIFIED");
}

std::string getLogHouseString(int houseId) {
    switch (houseId) {
        case ATREIDES:
            return std::string("ATREIDES");
        case HARKONNEN:
            return std::string("HARKONNEN");
        case ORDOS:
            return std::string("ORDOS");
        case FREMEN:
            return std::string("FREMEN");
        case SARDAUKAR:
            return std::string("SARDAUKAR");
        case MERCENARY:
            return std::string("MERCENARY");
        default:
            return std::string("UNKNOWN HOUSE");
    }
    return std::string("UNIDENTIFIED");
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

    logCommentLine(str);
    logCommentLine(txt);
    logCommentLine(str);
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
