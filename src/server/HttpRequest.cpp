#include "HttpRequest.h"
#include <cassert>

HttpRequest::HttpRequest(HttpMethod method, const std::string uri, std::map<std::string,
		std::string> headers, ContentStream content)
{
	Method = method;
	Uri = uri;
    // sanitize Uri to avoid access to /...
    if(Uri.size() > 0 && Uri[0] == '/')
        Uri = Uri.substr(1);
	Headers = headers;
	Content = content;	
}
