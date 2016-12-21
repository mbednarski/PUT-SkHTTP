#ifndef _HTTP_UTIL_
#define _HTTP_UTIL_

#include <map>
#include <string>

extern std::map<int, std::string> g_CodeToReasonPhase;
extern std::map<std::string, std::string> g_Mime;

void InitHttpUtil(void);

// obslugiwane metody http
enum class HttpMethod
{
	GET = 0,
	HEAD = 1,
	PUT = 2,
	DELETE = 3
};

HttpMethod ParseMethod(const std::string &methodToParse);
std::string ToString(HttpMethod method);

#endif