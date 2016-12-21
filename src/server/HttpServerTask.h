#ifndef _HTTP_SERVER_TASK_H_
#define _HTTP_SERVER_TASK_H_

#include <string>
#include "Task.h"
#include "HttpUtil.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Logger.h"

class HttpServerTask : public Task
{
public:
	virtual void Run(void);
	HttpServerTask(int conn);
	~HttpServerTask();
private:
	int Connection;
    // parsuj pierwsza linie
	HttpMethod ParseMethodLine(const std::string &methodLine, std::string &resourceUri);
    // odeslij odpowiedz
	void SendResponse(const HttpResponse &response);
    // parsuj zapytani
	HttpRequest ParseRequest(void);
    Logger log;
};

#endif
