#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>

class Settings
{	
public:
	unsigned short Port;
	size_t PoolSize;
	Settings(void);
	void Parse(int argc, char**argv);
	void Print(void);	
};

extern Settings g_Settings;

#endif
