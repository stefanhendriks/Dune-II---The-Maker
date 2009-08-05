#ifndef D2TM_CLOG_H_
#define D2TM_CLOG_H_

class cLogger {

public:
	void log(eLogLevel level, eLogComponent component, char *event, char *message);
	void log(eLogLevel level, eLogComponent component, char *event, char *message, eLogOutcome outcome, int playerId);
	void log(eLogLevel level, eLogComponent component, char *event, char *message, eLogOutcome outcome);

	void logHeader(char *txt);

	static cLogger *getInstance();

private:
	FILE *file;

	time_t current_time;

	std::string getLogLevelString(eLogLevel level);
	std::string getLogComponentString(eLogComponent component);
	std::string getLogOutcomeString(eLogOutcome outcome);

	std::string getCurrentFormattedTime();

	void updateTime();

	static cLogger *instance;

	std::string getIntegerAsString(int value);

protected:
	cLogger();

};

#endif
