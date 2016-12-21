#ifndef MTLOADER_H
#define MTLOADER_H

#include <fstream>
#include <map>
#include <string>
#include "Logger.h"

class MtLoader {
private:
	std::ifstream fileHandle;
	std::map <std::string, std::string> dataMap;
	std::string word;
	std::string transl;
    Logger log;
public:
	MtLoader(void);
	std::map <std::string, std::string> load(std::string fileName);
};

#endif
