#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <map>
#include "ContentStream.h"
#include "HttpRequest.h"
#include "HttpUtil.h"
#include "Logger.h"

class HttpResponse
{
private:
	// Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
	const char *HttpVersion = "HTTP/1.1";
	const char *CRLF = "\r\n";
	std::string ReasonPhase;
	
	std::map<std::string, std::string> Headers;
    ContentStream content;
    // zwraca typ mime na podstawie rozszerzenia
    static std::string GetContentType(const std::string &path);
    	
public:
	int StatusCode;
	HttpResponse(void);
    ~HttpResponse(void);
	void WriteResponseHeader(void);
    ContentStream GetResponse(void) const;
	
    static HttpResponse CreateGet(const HttpRequest &request, bool includeContent=true);
    static HttpResponse CreateHead(const HttpRequest &request);
    static HttpResponse CreatePut(const HttpRequest &request);
    static HttpResponse CreateDelete(const HttpRequest &request);
    
	static HttpResponse CreateError(int code, bool includeContent, bool includeContentHeaders);
};

#endif
