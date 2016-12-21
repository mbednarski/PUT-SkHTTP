#ifndef _HTTP_EXCEPTION_
#define _HTTP_EXCEPTION_

#include <exception>
#include <string>

// wyjatek opisujacy blad http (4xx 5xx)
// w niektorych kontekstach wygodnie jest nim rzucic i w procedurze obslugi
// wygenerowac odpowiedz z odpowiednim statusem
class HttpException : std::exception
{	
public:
	int ErrorCode;	
	virtual const char *what() const throw();
	HttpException(int code);	
};

// wyjatek do nieznalezienia pliku (lub niemoznosci jego otwarcia w odpowiednim trybie)
class FileNotFoundException : std::exception
{	
private:
	std::string filename;
public:	
	virtual const char *what() const throw();
	FileNotFoundException(const std::string &filename);	
};

// wyjatek do przedwczesnego zakocnzenia obslugi zadania (bez odsylanie response kodu)
class RecvException : std::exception
{	
public:	
	virtual const char *what() const throw();	
};

#endif
