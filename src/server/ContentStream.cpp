#include "ContentStream.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <utility>
#include <algorithm>
#include <sys/types.h>
#include <exception>
#include "HttpException.h"
using namespace std;

ContentStream::ContentStream(void)
{
	buffer = (unsigned char *)malloc(CHUNK_SIZE);
	currentSize = 0;
	capacity = CHUNK_SIZE;	 
}

ContentStream::ContentStream(ContentStream &&obj)
{
    buffer = obj.buffer;
    currentSize = obj.currentSize;
    capacity = obj.capacity;
    obj.buffer = nullptr;
    obj.capacity = 0;
    obj.currentSize = 0;
}

ContentStream &ContentStream::operator=(ContentStream obj)
{
    std::swap(buffer, obj.buffer);
    std::swap(currentSize, obj.currentSize);
    std::swap(capacity, obj.capacity);
    return *this;
}
ContentStream::ContentStream(const ContentStream &obj)
{
    currentSize = obj.currentSize;
    capacity = obj.capacity;
    buffer = (unsigned char *)malloc(capacity);
    memcpy(buffer, obj.buffer, capacity);
}

// wszystkie metody z gatunku Append zwiekszaja bufor tak dlugo az nie osiagnie
// on wymaganej pojemnosci (petla while)

void ContentStream::Append(const void *data, size_t len)
{
	while(capacity - currentSize < len)
	{
		Resize();
	}
	memcpy(buffer + currentSize, data, len);
	currentSize += len;
}
void ContentStream::Append(unsigned char c)
{
	while(capacity - currentSize < 1)
	{
		Resize();
	}
	buffer[currentSize] = c;
	++currentSize;
}

void ContentStream::Append(const char *cstr)
{
	size_t len = strlen(cstr);
	Append(cstr, len);
}

void ContentStream::Append(const string &str)
{
    Append(str.c_str());
}

void ContentStream::Append(const ContentStream &stream)
{
    Append(stream.buffer, stream.currentSize);
}

const unsigned char *ContentStream::GetData(size_t &size) const
{
	size = currentSize;
	return buffer;	
}

void ContentStream::Resize(void)
{
    // zwieksz bufor o chunk
	buffer = (unsigned char *) realloc(buffer, capacity + CHUNK_SIZE);
	capacity += CHUNK_SIZE;
}	

std::string ContentStream::AsString(void) const
{
	return std::string((const char *)buffer, currentSize);
}

void ContentStream::AppendFile(const string &filename)
{
    // otworz plik
    auto file = open(filename.c_str(), O_RDONLY);
    if(file == -1)
        throw FileNotFoundException(filename);
    
    unsigned char* buffer = new unsigned char[FILE_BUFFER_SIZE];
    
    // odczytaj cala zawartosc
    while(true)
    {
        int readed = read(file, buffer, FILE_BUFFER_SIZE);
        Append(buffer, readed);
        if(readed == 0)
            break;        
    }
        
    delete buffer;
    close(file);   
}

bool ContentStream::ToFile(const std::string &filename) const
{
    auto file = open(filename.c_str(), O_CREAT | O_WRONLY, 0666);
    if(file == -1)
    {
        cout << "failed to open file " << filename << endl;
        return false;
    }
    
    int written = write(file, buffer, currentSize);
    close(file);
    // jezeli udalo sie zapisac calosc to zwroc sukces
    return written == (long long)currentSize;   
}

ContentStream::~ContentStream()
{
	free(buffer);
	buffer = nullptr;
}
