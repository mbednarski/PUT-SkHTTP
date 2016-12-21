#include "HttpUtil.h"
#include "util.h"
#include "HttpException.h"
#include "MtLoader.h"
#include "Logger.h"

using namespace std;

map<int, std::string> g_CodeToReasonPhase;
map<string, string> g_Mime;
void InitHttpUtil(void)
{
    // inicjuj uzywane kody http
	g_CodeToReasonPhase[100] = "Continue";
	
	g_CodeToReasonPhase[200] = "OK";
    g_CodeToReasonPhase[201] = "Created";
	g_CodeToReasonPhase[204] = "No Content";
	
	g_CodeToReasonPhase[400] = "Bad Request";
	g_CodeToReasonPhase[404] = "Not Found";	
	g_CodeToReasonPhase[405] = "Method Not Allowed";
    g_CodeToReasonPhase[408] = "Request Timeout";
	g_CodeToReasonPhase[411] = "Length Required";
	g_CodeToReasonPhase[431] = "Request Header Fields Too Large";

	g_CodeToReasonPhase[505] = "Version Not Supported";	
    
    // laduj typy mime
    MtLoader mimeLoader;
    g_Mime = mimeLoader.load("../mime.txt");    
}

//self-explainig
HttpMethod ParseMethod(const string &methodToParse)
{
	string methodString = trim(methodToParse);
	HttpMethod method;
	if(methodString == "GET")
		method = HttpMethod::GET;
	else if(methodString == "PUT")
		method = HttpMethod::PUT;
	else if(methodString == "DELETE")
		method = HttpMethod::DELETE;
	else if(methodString == "HEAD")
		method = HttpMethod::HEAD;
	else
		throw HttpException(405);

	return method;
}

//self-explainig
string ToString(HttpMethod method)
{
	switch(method)
	{
		case HttpMethod::GET:
			return "GET";
			break;
		case HttpMethod::PUT:
			return "PUT";
			break;
		case HttpMethod::HEAD:
			return "HEAD";
			break;
		case HttpMethod::DELETE:
			return "DELETE";
			break;
		default:
			return "UNKNOWN HTTP METHOD";
			break;
	}
}