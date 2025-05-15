#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::string findExecutable(const std::string &command);
void executeExternal(const std::vector<std::string> &args);

#endif
