#include "d2tmh.h"

cLogger *cLogger::instance = NULL;

cLogger::cLogger() {
}

cLogger *cLogger::getInstance() {
	if (instance == NULL) {
		instance = new cLogger();
	}

	return instance;
}

std::string cLogger::getLogLevelString(eLogLevel level) {
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

std::string cLogger::getLogComponentString(eLogComponent component) {
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
	}

	return std::string("UNIDENTIFIED");
}

std::string cLogger::getLogOutcomeString(eLogOutcome outcome) {
	switch (outcome) {
		case OUTC_SUCCESS:
			return std::string("SUCCESS");
		case OUTC_FAILED:
			return std::string("FAILED");
		case OUTC_NONE:
			return std::string("NONE");
	}
	return std::string("UNIDENTIFIED");
}

// courtesy from : http://www.codeguru.com/forum/showthread.php?t=477894
std::string cLogger::getCurrentFormattedTime() {
	struct tm* ts;
	char szBuffer[80];

	ts = localtime(&current_time);

	// Format the time
	strftime(szBuffer, sizeof(szBuffer), "%H:%M:%S", ts);

	return szBuffer;
}

void cLogger::updateTime() {
	current_time = time (NULL);
}

void cLogger::logHeader(char *txt) {
	int length = strlen(txt);
	if (length > 79) length = 79;
	std::string line(length, '-');

	file = fopen("log.txt", "at");
	if (file) {
		fprintf(file, "%s\n", line.c_str()); // print the text into the file
		fprintf(file, "%s\n", txt); // print the text into the file
		fprintf(file, "%s\n", line.c_str()); // print the text into the file
		fclose(file);
	}
}

void cLogger::log(eLogLevel level, eLogComponent component, char *event, char *message) {
	log(level, component, event, message, OUTC_IGNOREME, -1);
}

void cLogger::log(eLogLevel level, eLogComponent component, char *event, char *message, eLogOutcome outcome) {
	log(level, component, event, message, outcome, -1);
}

//	<time> | <level> | <component> | <event> | <message> | <outcome> | <player>
void cLogger::log(eLogLevel level, eLogComponent component, char *event, char *message, eLogOutcome outcome, int playerId) {
	updateTime();

	// log line starts with time
	std::string logline = getCurrentFormattedTime();

	logline += "|"; // add first pipe

	std::string sLevel = getLogLevelString(level);
	std::string sComponent = getLogComponentString(component);

	logline += sLevel;
	logline += "|";
	logline += sComponent;
	logline += "|";
	logline += std::string(event);
	logline += "|";
	logline += std::string(message);

	if (outcome != OUTC_IGNOREME) {
		std::string sOutcome = getLogOutcomeString(outcome);
		logline += "|";
		logline += sOutcome;
	}

	if (playerId >= HUMAN) {
		logline += "|";
		logline += getIntegerAsString(playerId);
	}

	file = fopen("log.txt", "at");
	if (file) {
		fprintf(file, "%s\n", logline.c_str()); // print the text into the file
		fclose(file);
	}
}

std::string cLogger::getIntegerAsString(int value) {
	char numberChar[10];
	sprintf(numberChar, "%d", value);
	return std::string(numberChar);
}
