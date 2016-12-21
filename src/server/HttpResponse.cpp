#include "HttpResponse.h"

#include <stdio.h>
#include <stdlib.h>

#include "HttpUtil.h"
#include <cassert>
#include <sstream>
#include <iostream>
#include "util.h"
#include <unistd.h>

using namespace std;

Logger log;

// domysla nazwa zasobu jesli nie zostala podana
const string INDEX_FILENAME = "index.html";

HttpResponse::HttpResponse(void)
{
	Headers = std::map<std::string, std::string>();
    // te naglowski sa zawsze obecne wiec dodajemy je w ctorze
	Headers["Server"] = "SKHTTP";
    Headers["Connection"] = "close";
    Headers["Cache-control"] = "no-cache";
    content = ContentStream();
}

HttpResponse::~HttpResponse(void)
{
}

ContentStream HttpResponse::GetResponse(void) const
{
    return content;
}

void HttpResponse::WriteResponseHeader()
{
	// linia statusu (wersjka, kod, opis)
	char buffer[1024] = {0};
	sprintf(buffer, "%s %d %s %s", HttpVersion, StatusCode, ReasonPhase.c_str(), CRLF);
	content.Append(buffer);
	
    // wpisanie naglowkow
	for(const auto &header : Headers)
	{
		sprintf(buffer, "%s: %s%s", header.first.c_str(), header.second.c_str(), CRLF);
		content.Append(buffer);		
	}		
	// clrf jako zakonczenie naglowkow i rozpoczecie body
    
	content.Append(CRLF);		
}

HttpResponse HttpResponse::CreateError(int code, bool includeContent, bool includeContentHeaders)
{
    if(includeContent)
        includeContentHeaders = true;
    // utworz odpowiedz ze odpowiednim statusem
	HttpResponse hr;
	hr.StatusCode = code;
	hr.ReasonPhase = g_CodeToReasonPhase[code];
    string errorString = "<html><body><h1>" + ToString(code) + " - " + g_CodeToReasonPhase[code] + "</h1></body></html>";
    // opcjonalnie wpisz powod do wyswietlenia
    if(includeContentHeaders)
    {
        hr.Headers["Content-Type"] = "text/html";
        hr.Headers["Content-Length"] = ToString(errorString.size());
    }
    
    hr.WriteResponseHeader();
    
    if(includeContent)
        hr.content.Append(errorString);
	return hr;	
}

HttpResponse HttpResponse::CreateGet(const HttpRequest &request, bool includePayload)
{
    // try to open file
    
    log.log("Processing GET/HEAD for: " + request.Uri);
    
    HttpResponse response;
    
    //zabezpiecz sie przed szukaniem z / oraz dodaj index.html w razie potrzeby (jesli nie podano)
    
    string path = request.Uri;
    if(path == "")
        path = INDEX_FILENAME;
    
    if(!FileExists(path))
    {
        if(path[path.size()-1] == '/')
            path += INDEX_FILENAME;
        else
            path += "/" + INDEX_FILENAME; 
    }    
    
    if(FileExists(path))
    {
        log.log("Found " + path);
        // jesli plik istnieje zwroc 200 OK z jego zawartoscia
        response.StatusCode = 200;
        response.ReasonPhase = "OK";
        // pobierz rozmiar pliku i wpisz do naglowka
        size_t contentLen = GetFileSize(path);
        log.log("Resource size " + ToString(contentLen));
        stringstream ss;
        ss << contentLen;
        string contentLenString;
        ss >> contentLenString;        
        
        response.Headers["Content-Length"] = contentLenString;
        // ustaw typ mime
        response.Headers["Content-Type"] = GetContentType(path);
        
        response.WriteResponseHeader();
        // doklej zawartosc pliku do odpowiedzi
        if(includePayload)
        {
            log.log("Reading resource...");
            response.content.AppendFile(path);
            log.log("Resource read");
        }
        return response;
    }
    else
    {
        // jesli nie znaleziono - 404 not found
        log.log("Not found " + path);
        return CreateError(404, includePayload, true);
    }
}

// analogicznie jak CreateGet tylko nie przesylaj zawartosci pliku
HttpResponse HttpResponse::CreateHead(const HttpRequest &request)
{
    return HttpResponse::CreateGet(request, false);
}

HttpResponse HttpResponse::CreatePut(const HttpRequest &request)
{    
    log.log("Processing PUT for" + request.Uri);
    // czy zasob juz istnial?
    bool resourceHadExisted = FileExists(request.Uri);
    
    // zapisz dane pod daneym uri
    log.log("Trying to store resource...");
    bool success = request.Content.ToFile(request.Uri);
    if(success)
    {
        if(resourceHadExisted)
        {
            log.log("Replaced");
            return CreateError(204, true, true); // udalo sie, zasob zaaktualizowany 204 no content
            // za RFC: "The server has fulfilled the request but does not need to return an entity-body"
        }
        else
        {
            log.log("Created");
            return CreateError(201, true, true); // udalo sie, zasob nowy. 201 created
            // za RFC: "The request has been fulfilled and resulted in a new resource being created."
        }
    }
    else
    {
        log.log("Failed to store resource");
        return CreateError(400, true, true); // kwestia sporna czy 500 internal server error nie nadaje sie lepiej.
        // ostatecznie jako ze jedynym spsoobem na blad bylo podanie nieprawidlowego uri.
        // optymalnie bylo by rozrozniac wszystkie mozliwe bledy ale zdecydowalismy sie tego nie robic w
        // tej implementacji
    }    
}

HttpResponse HttpResponse::CreateDelete(const HttpRequest &request)
{
    log.log("Processing DELETE for" + request.Uri);
    log.log("Trying to unlink resource...");
    bool success = unlink(request.Uri.c_str()) == 0;
    
    if(success)
    {
        log.log("Deleted " + request.Uri);
        return CreateError(204, true, true); // 204 no content - usunieto
    } 
    else
    {
        if(!FileExists(request.Uri))
        {
            log.log("Not found " + request.Uri);
            return CreateError(404, true, true); // not found - zasob nie istnial wiec nie zostal usuniety
        }
        else
        {
            log.log("Unknow error when trying to delete " + request.Uri);
            return CreateError(500, true, true); // wystapil inny (nieznany) blad
        }
    }
}

string HttpResponse::GetContentType(const string &path)
{
    // jesli nie mamy na liscie to zakladdamy domyslny "application/octet-stream"
    auto dot = path.find_last_of(".");
    if(dot == string::npos)
        return "application/octet-stream";
    auto extension = path.substr(dot);
    
    if(g_Mime.find(extension) == g_Mime.end())
        return "application/octet-stream";
    return g_Mime[extension];
}
