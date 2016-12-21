#include "util.h"
#include <sys/stat.h>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <utility>
#include "HttpException.h"

using namespace std;

string trim(const string& str)
{
    // pierwszy niebialy znak
    size_t first = str.find_first_not_of(" \t\r\n");
    if(first == string::npos) // jesli string zaweira same biale znaki zwroc pusty
    {
        return "";
    }
    // ostatni bialy znak
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last-first+1));
}

string ToString(int n)
{
    stringstream ss;
    ss << n;
    string s;
    ss >> s;
    return s;
}

int ToInt(const std::string &str)
{
    stringstream ss;
    ss << str;
    int n;
    ss >> n;
    return n;
}

bool FileExists(const string &path)
{
    struct stat fileStat;  
    if ( stat(path.c_str(), &fileStat) )
    {
        return false;
    }
    if ( !S_ISREG(fileStat.st_mode) )
    {
        return false;
    }
    return true;
}

bool DirExists(const string &path)
{
    struct stat fileStat;
    if ( stat(path.c_str(), &fileStat) )
    {
        return false;
    }
    if ( !S_ISDIR(fileStat.st_mode) )
    {
        return false;
    }
    return true;
}

size_t GetFileSize(const std::string &path)
{
    auto file = open(path.c_str(), O_RDONLY);
    if(file == -1)
        throw FileNotFoundException(path);
        
     size_t size = lseek(file, 0, SEEK_END);
     close(file);
     
     return size;
}


void EnsureDirExists(const std::string &dir)
{
    if(!DirExists(dir))
        mkdir(dir.c_str(), 0666);
}


