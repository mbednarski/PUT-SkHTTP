#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>
#include "ContentStream.h"
#include "HttpUtil.h"

class HttpRequest
{
public:
	// Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
	const char *HttpVersion = "HTTP/1.1";
	const char *CRLF = "\r\n";
	HttpMethod Method;
	// https://danielmiessler.com/study/url_vs_uri/
	std::string Uri;	
    // naglowki zadania
	std::map<std::string, std::string> Headers;
    //zawartosc zadania (body/payload)
	ContentStream Content;
	
	HttpRequest(HttpMethod method, std::string uri, std::map<std::string,
		std::string> headers, ContentStream content);
		
};

#endif
