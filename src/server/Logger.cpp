#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <cassert>
#include "Logger.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util.h"
using namespace std;

Logger::Logger(void)
{
    EnsureDirExists("../log");
	now=time (&now);
	ltm = *localtime(&now);
	strftime(fileName,sizeof(fileName),"../log/%Y-%m-%d",&ltm);
	logfile.open(fileName,ios_base::app);
}

pthread_mutex_t Logger::mutex(PTHREAD_MUTEX_INITIALIZER);

void Logger::log(string text, Severity type){
	now=time (&now);
	ltm = *localtime(&now);
	strftime(eventTime,sizeof(eventTime),"%X",&ltm);
	pthread_mutex_lock(&mutex);
	switch(type){
		case Severity::ERROR:
			logfile << eventTime << ": ERROR - " << text << "\n";
			cout << eventTime << ": ERROR - " << text << endl;
			break;
		case Severity::WARN:
			logfile << eventTime << ": WARN - " << text << "\n";
			cout << eventTime << ": WARN - " << text << endl;
			break;
		case Severity::INFO:
			logfile << eventTime << ": INFO - " << text << "\n";
			cout << eventTime << ": INFO - " << text << endl;
			break;
		default:
            assert(false);
			break;
	}
	logfile.flush();
	pthread_mutex_unlock(&mutex);
}
