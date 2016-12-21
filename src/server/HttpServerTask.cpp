#include "HttpServerTask.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "util.h"
#include "HttpException.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

// wzorzec konca naglowka - CRLFCRLF
const char *END_OF_HTTP_HEADER = "\r\n\r\n";

// maksymalny rozmiar naglowka zadania
const size_t MAX_HEADER_SIZE = 4096;
// wielkosc chunka do odczytywania zadania
const size_t CONTENT_CHUNK_SIZE = 1024*1024;

void PrintHeaders(map<string, string> headers)
{
	for(const auto &header : headers)
	{
		cout << header.first << ": " << header.second << endl;
	}
}

HttpServerTask::HttpServerTask(int conn)
{
	Connection = conn;
}

void HttpServerTask::Run(void)
{
    try
    {
        // odbierz i parsuj zapytanie
        auto request = ParseRequest();
        HttpResponse response;
        // odpowiednio obsluz
        switch(request.Method)
        {
            case HttpMethod::GET:
                response = HttpResponse::CreateGet(request);
                break;
            case HttpMethod::HEAD:
                response = HttpResponse::CreateHead(request);
                break;
            case HttpMethod::PUT:
                response = HttpResponse::CreatePut(request);
                break;
            case HttpMethod::DELETE:
                response = HttpResponse::CreateDelete(request);
                break;
            default:
                log.log("Unknown HttpMethod", Severity::ERROR);
                throw HttpException(405);
                return;
        }
        // odeslij odpowiedz
        SendResponse(response);
    }
    catch(RecvException &ex)
    {
        log.log("Exception" + string(ex.what()));
        log.log("Aborted client due to broken connection");
    }
    catch(HttpException &ex)
    {
        log.log("Exception" + string(ex.what()));
        HttpResponse errorResponse = HttpResponse::CreateError(ex.ErrorCode, true, true);
        SendResponse(errorResponse);
    }
    catch(FileNotFoundException &ex)
    {        
        log.log("FileNotFoundException " + string(ex.what()), Severity::ERROR);
        // to sie nie powinno w ogole wydarzyc: wyjatek nie powien tu doleciec, jesli
        // tak sie stalo to cos nie zadzialalo
    }
}

HttpServerTask::~HttpServerTask()
{
	close(Connection);
}

HttpMethod HttpServerTask::ParseMethodLine(const std::string &methodLine, std::string &resourceUri)
{
    // parsuj pierwsza linie, odczytaj zasob, wersje i metode
    //cout <<methodLine << endl;
	HttpMethod method;
	stringstream ss;
	ss << methodLine;
	string methodString, resourceString, protocolVersionString;
	getline(ss, methodString, ' ');
	getline(ss, resourceString, ' ');
	getline(ss, protocolVersionString, ' ');

	if(trim(protocolVersionString) != "HTTP/1.1")
	{
        // nie obslugujemy innych wersji 
		throw HttpException(505);
	}

	method = ParseMethod(methodString);
	resourceUri = trim(resourceString);
	return method;
}
void HttpServerTask::SendResponse(const HttpResponse &response)
{
	ContentStream cs = response.GetResponse();
	size_t len;
	const unsigned char *data = cs.GetData(len);
    log.log("Sending back response " + ToString(len));
    size_t sent = 0;
    while(sent < len)
    {
	    auto s = send(Connection, data + sent, len, 0);
        log.log("Sent bytes: " + ToString(s));
        if(s == -1)
        {
            log.log("send failed, lost connection", Severity::ERROR);
            return;
        }
        sent += s;
    }
    log.log("Response sent");
}
HttpRequest HttpServerTask::ParseRequest(void)
{
	//cout << "Parsing request...\n";
	map<string, string> headers;
	char *buffer = new char[MAX_HEADER_SIZE];
	memset(buffer, MAX_HEADER_SIZE, 0);
	int received = recv(Connection, buffer, MAX_HEADER_SIZE, 0);
    
    if(received == 0)
    {
        throw HttpException(408); // klient nawiazal polaczenie ale nic nie wyslal.
        // np chrome tak robi
    }

	stringstream requestStream;
	requestStream << buffer;

    // parsuj pierwsza linie

	string line;
	getline(requestStream, line);
	string uri;
	HttpMethod method = ParseMethodLine(line, uri);

    // lec linia po linii aby wyciagnac naglowki
	while(getline(requestStream, line))
	{
        // przytni spacje z obu stron
	    line = trim(line);
		// pusta linia oznacza ze skonczyly sie naglowki
		if(line.empty())
			break;

		// parsuj naglowek
		string name, value;
		size_t split = line.find(": ");
		name = line.substr(0, split);
		value = line.substr(split + 2);
		// cout << "Header: '" << name << "' value: '" << value <<  "'" << endl;
		headers.insert(pair<string, string>(name, value));
	}
	// cerr << "Finished parsing headers.";
	//PrintHeaders(headers);

	// Oblicz ile jeszcze trzeba odczytac (content len minus to co juz zostalo odczytane)

	auto contentLength = headers.find("Content-Length");
	if(contentLength == headers.end() && method == HttpMethod::PUT)
	{
		// nie ma naglowka content-length
		throw HttpException(411);
	}

	size_t headerSize = 0;

	char *headerEndPos = strstr(buffer, END_OF_HTTP_HEADER);
	if(headerEndPos == NULL)
	{
		throw HttpException(431); // za duzo naglowkow
	}
	headerSize = headerEndPos - buffer + 4; // for double CRLF
	size_t receivedBodyBytes = received - headerSize;
	size_t l = atoi(headers["Content-Length"].c_str());
	size_t bytesStillToBeReceived = l - receivedBodyBytes;

    /*
	cout << "HeaderSize: " << headerSize << endl;
	cout << "Initial Received: " << received << endl;
	cout << "bodu length: " << l << endl;
	cout << "Received body bytes: " << receivedBodyBytes << endl;
	cout << "Bytes body bytes to be received: " << bytesStillToBeReceived << endl;
    */

	ContentStream content;
	content.Append(headerEndPos + 4, receivedBodyBytes);

    // odczytaj pozostala zawartosc zadania

	char body[CONTENT_CHUNK_SIZE] = {0};
	while(bytesStillToBeReceived > 0)
	{
		log.log("receiving...");
		ssize_t r = recv(Connection, body, CONTENT_CHUNK_SIZE, 0);
        if(r == -1)
        {
            log.log("Error when recv", Severity::ERROR);
            delete buffer;
            throw RecvException();
        }
        
		log.log("Received next bytes: " + ToString(r));
		content.Append(body, r);
		bytesStillToBeReceived -= r;
		log.log("Still to receive: " + ToString(bytesStillToBeReceived));
	}
	log.log("Finished receiving");
	log.log("Still to receive: " + ToString(bytesStillToBeReceived));
    
	delete buffer;

	// auto sss = content->AsString();
	// cout << "Content: '" << sss << "'\n";
	return HttpRequest(method, uri, headers, content);
}
