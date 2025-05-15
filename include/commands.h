#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

void echo(const std::vector<std::string>& args);
void pwd();
void ls();
void catCommand(const std::vector<std::string>& args);
void touch(const std::vector<std::string>& args);
void del(const std::vector<std::string>& args);
void mkdir(const std::vector<std::string>& args);
void deldir(const std::vector<std::string>& args);
void changeDirectory(const std::vector<std::string>& args);
bool isBuiltin(const std::string& command);


#endif
