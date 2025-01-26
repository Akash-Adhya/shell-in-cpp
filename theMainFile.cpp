#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <cstdlib>
#include <limits.h>
#include <direct.h>
#include <io.h>
#include <windows.h>

using namespace std;

// Helper functions to trim strings
inline void ltrim(string &s)
{
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch)
                               { return !isspace(ch); }));
}

inline void rtrim(string &s)
{
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                    { return !isspace(ch); })
                .base(),
            s.end());
}

// Partition the command from the input
string partitionCommand(const string &input)
{
    string command = "";
    int i = 0;

    while (i < input.length() && input[i] != ' ')
    {
        command += input[i];
        i++;
    }

    return command;
}

// Partition parameters from the input
string partitionParameters(string input, int index)
{
    return input.substr(index);
}

// Function to check if a command is a built-in
bool isBuiltin(const string &command, const vector<string> &builtins)
{
    return find(builtins.begin(), builtins.end(), command) != builtins.end();
}

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

// Finding Present working directory
void pwd()
{
    char pwd[PATH_MAX];
    if (_getcwd(pwd, sizeof(pwd)) != NULL)
    {
        cout << pwd << endl;
    }
    else
    {
        perror("_getcwd() error");
    }
}

// Processing the Quotations
string processQuotedSegments(const string& parameters) {
    string result="";
    bool isContainQuote = false;
    int  n = parameters.length();

    for(char c : parameters){
        if(c == '\'' || c == '"'){
            isContainQuote = true; 
            break;
        }
    }

    if(!isContainQuote){
        stringstream ss(parameters);
        string word;
        while (ss >> word)
        {
            result += word+" ";
        }
        
    }
    else{
        bool isQuoted = false;
        string temp = "";
        int i = 0;
        while(i < n){
            char c = parameters[i];

             if (c == '\'' || c == '"') {
                if (isQuoted) {
                    result += temp;
                    temp = "";
                    isQuoted = false;
                } else {
                    isQuoted = true;
                    temp = ""; 
                }
            } else if (isQuoted) {
                temp += c;
            } else {
                result += c;
            }

            i++;
        }
    }

    return result;
}


// Echo command
void echo(const string& parameters) {
    string processed = processQuotedSegments(parameters);
    cout << processed << endl; // Print the processed result
}

int main()
{
    // List of built-in commands
    vector<string> builtins = {"type", "echo", "exit", "pwd"};

    // Flush after every std::cout / std::cerr
    cout << unitbuf;
    cerr << unitbuf;

    cout << "Hello there! Welcome to my shell. Use it however you want." << endl;
    cout << "Created with LOVE by Akash ❤" << endl;

    while (true)
    {
        cout << "$ ";

        string input;
        getline(cin, input);

        // Capture command and parameters
        string command = partitionCommand(input);
        string parameters = partitionParameters(input, command.length());

        ltrim(command);
        rtrim(command);
        ltrim(parameters);
        rtrim(parameters);

        // Exiting the shell
        if (command == "exit")
        {
            if (parameters == "0")
                exit(0);
            else
                cout << "Use 'exit 0' to exit." << endl;
        }

        // Split input into command and arguments
        vector<string> args;
        args.push_back(command);
        size_t pos = 0;
        string temp = parameters;
        while ((pos = temp.find(' ')) != string::npos)
        {
            args.push_back(temp.substr(0, pos));
            temp.erase(0, pos + 1);
        }
        if (!temp.empty())
        {
            args.push_back(temp);
        }

        // Handle the `echo` command
        if (command == "echo")
        {
            echo(parameters);
        }

        // Handle the cat Command
        else if(command == "cat"){
            system(input.c_str());
        }

        // Handle the `pwd` command
        else if (command == "pwd")
        {
            if (parameters.empty())
                pwd();
            else
            {
                cerr << "pwd : No parameters required." << endl;
            }
        }

        // Handle the `cd` command
        else if (command == "cd")
        {
            const char *targetDir = parameters.c_str();
            if (parameters == "~")
            {
                const char *HOMEPATH = getenv("USERPROFILE");
                if (HOMEPATH && _chdir(HOMEPATH) == 0)
                {
                    // Successfully changed to home directory.
                }
                else
                {
                    cout << command << ": " << parameters << ": Unable to access home directory" << endl;
                }
            }
            else if (_chdir(targetDir) != 0)
            {
                cout << command << ": " << parameters << " No such file or directory";
            }
        }

        // Handle the `type` command
        else if (command == "type")
        {
            if (parameters.empty())
            {
                cerr << "type: missing argument\n";
                continue;
            }

            if (isBuiltin(parameters, builtins))
            {
                cout << parameters << " is a shell builtin\n";
            }
            else if (parameters.find('\\') != string::npos && _access(parameters.c_str(), 4) == 0)
            {
                cout << parameters << " is " << parameters << "\n";
            }
            else
            {
                string execPath = findExecutable(parameters);
                if (!execPath.empty())
                {
                    cout << parameters << " is " << execPath << "\n";
                }
                else
                {
                    cerr << parameters << ": not found\n";
                }
            }
        }

        // Handle unknown or external commands
        else
        {
            string execPath = findExecutable(command);
            if (!execPath.empty())
            {
                executeExternal(args);
            }
            else
            {
                cerr << command << ": command not found\n";
            }
        }
    }

    return 0;
}
