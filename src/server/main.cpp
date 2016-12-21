#include <stdio.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "Task.h"
#include "ThreadPool.h"
#include <cstdlib>
#include "Settings.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <cstring>
#include <sstream>
#include <map>
#include "HttpException.h"
#include <fstream>
#include "ContentStream.h"
#include "HttpResponse.h"
#include "HttpUtil.h"
#include "HttpServerTask.h"
#include "util.h"
#include "Logger.h"

using namespace std;

string banner = "***SKHTTP Server by Mateusz Bednarski & Artur Konieczny***";

int main(int argc, char **argv)
{
    // przejdz do katalogu bazowego
    // inicjuj co trzega
    EnsureDirExists("public_html");
    chdir("public_html");
    Logger log;
    log.log(banner, Severity::INFO);
	InitHttpUtil();
	g_Settings.Parse(argc, argv);	
	ThreadPool tp(g_Settings.PoolSize);
	tp.Start();
	
    // nasluchuj na polaczenia na 8080 na wzsystkich ip
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

	sockaddr_in sa;
	sa.sin_family = PF_INET;
	sa.sin_port = htons(g_Settings.Port);
	sa.sin_addr.s_addr = INADDR_ANY;

	int status = bind(fd, (sockaddr *) &sa, sizeof(sa));
	if(status == -1)
    {
		log.log("Cannot bind socket", Severity::ERROR);
        exit(2);
    }

    log.log("Listening...");
	listen(fd, 10);

    // w nieskonczonosc
	while(true)
	{
        // akceptuj klienta, task sam go zwolni kiedy trzeba
		sockaddr_in client_addr;
		socklen_t none = sizeof(client_addr);

		int conn = accept(fd, (sockaddr *) &client_addr, &none);		
		const char *clientIp = inet_ntoa(client_addr.sin_addr);
        string msg = "Client accepted " + string(clientIp); 
		log.log(msg);
				
		Task *t = new HttpServerTask(conn);
		tp.EnqueueTask(t);		
	}

	close(fd);

	return 0;
}
