#include "Settings.h"
#include <string>
#include <cassert>
#include <sstream>
#include "Logger.h"
#include <string.h>
#include "util.h"

Settings g_Settings;

Logger logger;

Settings::Settings(void)
{
	Port = 8080;
	PoolSize = 10;	
};

void Settings::Parse(int argc, char **argv)
{
	for (int i=1;i<argc;i++){
		try{
			if (strcmp(argv[i],"-p")==0) {
				Port = ToInt(argv[i+1]);
			} else if (strcmp(argv[i],"-ps")==0) {
				PoolSize = ToInt(argv[i+1]);
			}
		} catch (const std::exception& ia){
            std::stringstream ss;
			ss << "Error parsing arguments! " << ia.what() << " aborting...\n";
            logger.log(ss.str(), Severity::ERROR);	
		}
	}
    Print();
}

void Settings::Print(void)
{
    std::stringstream ss;
    ss << "Listening on port: " << Port;
    logger.log(ss.str());
    std::stringstream sss;
    sss << "ThreadPool size: " << PoolSize;
    logger.log(sss.str());    
    
}
