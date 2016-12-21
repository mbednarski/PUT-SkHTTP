#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <ctime>
#include <pthread.h>

enum class Severity
{
	ERROR = 0,
	WARN = 1,
	INFO = 2
};

class Logger {
private:
	char fileName[80];
	char eventTime[80];
	std::ofstream logfile;
	time_t now;
	struct tm ltm;
	static pthread_mutex_t mutex;
public:
	Logger(void);
	void log(std::string text, Severity type=Severity::INFO);
};

#endif
