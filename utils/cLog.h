#pragma once

//#include <cstdio>
#include <iostream>
#include <fstream>

#include <string>

enum eLogLevel {
    LOG_INFO,
    LOG_TRACE,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

enum eLogComponent {
    COMP_UNITS,
    COMP_STRUCTURES,
    COMP_GAMEINI,
    COMP_SCENARIOINI,
    COMP_PARTICLE,
    COMP_BULLET,
    COMP_AI,
    COMP_UPGRADE_LIST,
    COMP_BUILDING_LIST_UPDATER,
    COMP_SIDEBAR,
    COMP_MAP,
    COMP_NONE,
    COMP_INIT,
    COMP_ALLEGRO,		/** Use for allegro specific calls **/
    COMP_SETUP,
    COMP_VERSION,		/** version specific loggin messages **/
    COMP_SKIRMISHSETUP, /** When skirmish game is being set up **/
    COMP_ALFONT, 		/** ALFONT library specific **/
    COMP_SOUND, 		/** Sound related **/
    COMP_REGIONINI		/** Used for regions **/
};

enum eLogOutcome {
    OUTC_SUCCESS,
    OUTC_FAILED,
    OUTC_NONE,
    OUTC_UNKNOWN,
    OUTC_IGNOREME /** will not be printed **/
};

class cLogger {
  public:
      static cLogger* getInstance();

      void setDebugMode(bool value) { m_debugMode = value; }

      void log(eLogLevel level, eLogComponent component, const std::string& event, const std::string& message);
      void log(eLogLevel level, eLogComponent component, const std::string& event, const std::string& message, eLogOutcome outcome, int playerId, int houseId);
      void log(eLogLevel level, eLogComponent component, const std::string& event, const std::string& message, eLogOutcome outcome);

      void logCommentLine(const std::string& txt);

      void logHeader(const std::string& txt);

  private:
      cLogger();
      ~cLogger();

      cLogger& operator=(const cLogger&) = delete;
      cLogger& operator=(cLogger&&) = delete;
      cLogger(const cLogger&) = delete;
      cLogger(cLogger&&) = delete;

      //std::FILE* m_file;
      std::ofstream m_file;

      long m_startTime; // start time of logging in miliseconds

      bool m_debugMode;

      long getTimeInMilisDifference() const;
};
