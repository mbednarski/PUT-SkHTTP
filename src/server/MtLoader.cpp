#include <fstream>
#include <map>
#include "MtLoader.h"

using namespace std;

MtLoader::MtLoader(void){};

map <string,string> MtLoader::load(string fileName){
	fileHandle.open(fileName);
    if(fileHandle.is_open())
        log.log("Loading MIME types from " + fileName);
    else
        log.log("Error opening file " + fileName, Severity::ERROR);   
    
	while (fileHandle >> word ){ 
		fileHandle >> transl;
		dataMap[word]=transl;
	}
	fileHandle.close();
	return dataMap;
}
