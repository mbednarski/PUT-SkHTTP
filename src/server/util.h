#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>

// usun biale znaki z obu koncow
std::string trim(const std::string& str);

// nazwy mowia same za siebie

std::string ToString(int n);
int ToInt(const std::string &str);

bool FileExists(const std::string &path);
bool DirExists(const std::string &path);
size_t GetFileSize(const std::string &path);
void EnsureDirExists(const std::string &path);

#endif