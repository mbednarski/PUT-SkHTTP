#include "HttpException.h" 
#include "HttpUtil.h"

HttpException::HttpException(int code)
{
	ErrorCode = code;
}

const char *HttpException::what() const  throw()
{
	return g_CodeToReasonPhase[ErrorCode].c_str();
}

const char *RecvException::what() const  throw()
{
	return "recv() returned -1";
}

FileNotFoundException::FileNotFoundException(const std::string &filename)
{
	this->filename = filename;
}

const char *FileNotFoundException::what() const  throw()
{
    std::string msg = "Failed to open file: " + filename + "\n";
	return msg.c_str();
}

