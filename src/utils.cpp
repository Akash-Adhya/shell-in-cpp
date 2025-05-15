#include "utils.h"
#include <windows.h>
#include <io.h>
#include <iostream>
#include <cstdlib>

using namespace std;


// Search for an executable in the PATH
string findExecutable(const string &command)
{
    char *path_env = getenv("PATH");
    if (path_env == nullptr)
        return "";

    string path(path_env);
    size_t pos = 0;

    while ((pos = path.find(';')) != string::npos)
    { // Use ';' for Windows PATH delimiter
        string dir = path.substr(0, pos);
        path.erase(0, pos + 1);

        string file_path = dir + "\\" + command;
        if (_access(file_path.c_str(), 4) == 0)
        {                     // Check for read and execute permissions
            return file_path; // Return the first match found
        }
    }

    // Check the remaining part of PATH
    if (!path.empty())
    {
        string file_path = path + "\\" + command;
        if (_access(file_path.c_str(), 4) == 0)
        {
            return file_path; // Return the first match found
        }
    }

    return ""; // No executable found
}

// Execute external commands
void executeExternal(const vector<string> &args)
{
    string commandLine;
    for (const string &arg : args)
    {
        commandLine += arg + " ";
    }

    // Set up process startup information
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;

    // Create the process
    if (!CreateProcess(nullptr, const_cast<char *>(commandLine.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi))
    {
        cerr << "Error: failed to execute command\n";
        return;
    }

    // Wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}