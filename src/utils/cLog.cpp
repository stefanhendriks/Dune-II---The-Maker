#include "../include/d2tmh.h"

#include <sstream>

cLogger *cLogger::instance = NULL;

cLogger::cLogger() {
	startTime = clock();
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
	switch (component) {
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

std::string cLogger::getLogOutcomeString(eLogOutcome outcome) {
	switch (outcome) {
		case OUTC_SUCCESS:
			return std::string("SUCCESS");
		case OUTC_FAILED:
			return std::string("FAILED");
		case OUTC_NONE:
			return std::string("NONE");
		case OUTC_IGNOREME:
			return std::string("IGNOREME");
		case OUTC_UNKNOWN:
			return std::string("UNKNOWN");
	}
	return std::string("UNIDENTIFIED");
}

std::string cLogger::getLogHouseString(int houseId) {
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
	current_time = time(NULL);
}

void cLogger::logHeader(const char *txt) {
	int length = strlen(txt);
	if (length > 79)
		length = 79;
	std::string line(length, '-');

	char * str = new char[line.length() + 1];
	sprintf(str, "%s", line.c_str());

	logCommentLine(str);
	logCommentLine(txt);
	logCommentLine(str);

	delete [] str;
}

void cLogger::log(eLogLevel level, eLogComponent component, const char *event, const char *message) {
	log(level, component, event, message, OUTC_IGNOREME, -1, -1);
}

void cLogger::log(eLogLevel level, eLogComponent component, const char *event, const char *message, eLogOutcome outcome) {
	log(level, component, event, message, outcome, -1, -1);
}

//	Timestamp | Level | Component | House (if component requires) | ID (if component requires) | Message | Outcome | Event | Event fields...
void cLogger::log(eLogLevel level, eLogComponent component, const char *event, const char *message, eLogOutcome outcome, int playerId, int houseId) {
	updateTime();

	int diffTime = getTimeInMilisDifference();

	// log line starts with time
	std::string logline = getLongAsString(diffTime);
	//= getCurrentFormattedTime();

	logline += "|"; // add first pipe

	std::string sLevel = getLogLevelString(level);
	std::string sComponent = getLogComponentString(component);

	logline += sLevel;
	logline += "|";
	logline += sComponent;

	if (playerId >= GENERALHOUSE) {
		logline += "|";
		logline += getLogHouseString(houseId);
	}

	if (playerId >= HUMAN) {
		logline += "|";
		logline += getIntegerAsString(playerId);
	}

	logline += "|";
	logline += std::string(message);

	if (outcome != OUTC_IGNOREME) {
		std::string sOutcome = getLogOutcomeString(outcome);
		logline += "|";
		logline += sOutcome;
	}

	logline += "|";
	logline += std::string(event);

	// TODO: here could come other fields later

	file = fopen("log.txt", "at");
	if (file) {
		fprintf(file, "%s\n", logline.c_str()); // print the text into the file
		fclose(file);
	}
}

std::string cLogger::getIntegerAsString(int value) {
	std::ostringstream oss;
	oss << std::dec << value;
	return oss.str();
}

std::string cLogger::getLongAsString(long value) {
	std::ostringstream oss;
	oss << std::dec << value;
	return oss.str();
}

void cLogger::logCommentLine(const char *txt) {
	file = fopen("log.txt", "at");
	if (file) {
		fprintf(file, "\\\\%s\n", txt); // print the text into the file
		fclose(file);
	}
}

long cLogger::getTimeInMilisDifference() /* From 1970-01-01T00:00:00 */
{
	long time_taken_millis;
	time_taken_millis = (long) ((clock() - startTime) * 1E3 / CLOCKS_PER_SEC);
	return time_taken_millis;
}
