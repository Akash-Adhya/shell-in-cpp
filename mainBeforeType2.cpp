#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sstream>
#include <sys/wait.h>

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

// Function to split input into arguments while respecting quotes
vector<string> splitInput(const string &input)
{
    vector<string> args;
    string arg;
    bool inQuotes = false;
    bool inSingleQuotes = false;

    for (size_t i = 0; i < input.length(); ++i)
    {
        char ch = input[i];

        if (inQuotes)
        {
            if (ch == '"')
            {
                inQuotes = false;
            }
            else if (ch == '\\' && i + 1 < input.length() && (input[i + 1] == '"' || input[i + 1] == '\\' || input[i + 1] == '$'))
            {
                arg += input[++i]; // Handle escape sequences
            }
            else
            {
                arg += ch;
            }
        }
        else if (inSingleQuotes)
        {
            if (ch == '\'')
            {
                inSingleQuotes = false;
            }
            else
            {
                arg += ch;
            }
        }
        else
        {
            if (isspace(ch))
            {
                if (!arg.empty())
                {
                    args.push_back(arg);
                    arg.clear();
                }
            }
            else if (ch == '"')
            {
                inQuotes = true;
            }
            else if (ch == '\'')
            {
                inSingleQuotes = true;
            }
            else if(ch == '\\' && i < input.length()-1){
                arg += input[++i];
            }
            else
            {
                arg += ch;
            }
        }
    }

    if (!arg.empty())
    {
        args.push_back(arg);
    }

    return args;
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

    while ((pos = path.find(':')) != string::npos)
    {
        string dir = path.substr(0, pos);
        path.erase(0, pos + 1);

        string file_path = dir + "/" + command;
        if (access(file_path.c_str(), R_OK | X_OK) == 0)
        {
            return file_path; // Return the first match found
        }
    }

    // Check the remaining part of PATH
    if (!path.empty())
    {
        string file_path = path + "/" + command;
        if (access(file_path.c_str(), R_OK | X_OK) == 0)
        {
            return file_path; // Return the first match found
        }
    }

    return ""; // No executable found
}

// Execute external commands
void executeExternal(const vector<string> &args)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        cerr << "Error: failed to fork process\n";
        return;
    }

    if (pid == 0) // Child process
    {
        // Convert vector<string> to char* array for execvp
        vector<char *> c_args;
        for (const string &arg : args)
        {
            c_args.push_back(const_cast<char *>(arg.c_str()));
        }
        c_args.push_back(nullptr); // Null-terminate the array

        if (execvp(c_args[0], c_args.data()) == -1)
        {
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }
    else // Parent process
    {
        int status;
        waitpid(pid, &status, 0); // Wait for child to finish
    }
}

// Finding Present working directory
void pwd()
{
    char pwd[PATH_MAX];
    if (getcwd(pwd, sizeof(pwd)) != NULL)
    {
        cout << pwd << endl;
    }
    else
    {
        perror("getcwd() error");
    }
}

// Echo command
void echo(const vector<string> &args)
{
    for (size_t i = 1; i < args.size(); ++i)
    {
        if (i > 1)
            cout << " ";
        cout << args[i];
    }
    cout << endl;
}

int main()
{
    // List of built-in commands
    vector<string> builtins = {"type", "echo", "exit", "pwd", "cd",};

    // Flush after every std::cout / std::cerr
    cout << unitbuf;
    cerr << unitbuf;

    while (true)
    {
        cout << "$ ";

        string input;
        getline(cin, input);

        // Exiting the shell
        if (input == "exit 0")
        {
            exit(0);
        }

        // Parse the input into arguments
        vector<string> args = splitInput(input);
        if (args.empty())
            continue;

        string command = args[0];

        // Handle the `echo` command
        if (command == "echo")
        {
            echo(args);
        }

        // Handle the `cat` command
        else if (command == "cat")
        {
            executeExternal(args);
        }

        // Handle the `pwd` command
        else if (command == "pwd")
        {
            if (args.size() == 1)
                pwd();
            else
            {
                cerr << "pwd: No parameters required." << endl;
            }
        }

        // Handle the `cd` command
        else if (command == "cd")
        {
            if (args.size() == 1 || args[1] == "~")
            {
                const char *HOMEPATH = getenv("HOME");
                if (HOMEPATH && chdir(HOMEPATH) == 0)
                {
                    // Successfully changed to home directory
                }
                else
                {
                    cerr << command << ": Unable to access home directory" << endl;
                }
            }
            else if (chdir(args[1].c_str()) != 0)
            {
                cerr << command << ": " << args[1] << ": No such file or directory" << endl;
            }
        }

        // Handle the `type` command
        else if (command == "type")
        {
            if (args.size() == 1)
            {
                cerr << "type: missing argument\n";
                continue;
            }

            if (isBuiltin(args[1], builtins))
            {
                cout << args[1] << " is a shell builtin\n";
            }
            else if (args[1].find('/') != string::npos && access(args[1].c_str(), R_OK | X_OK) == 0)
            {
                cout << args[1] << " is " << args[1] << "\n";
            }
            else
            {
                string execPath = findExecutable(args[1]);
                if (!execPath.empty())
                {
                    cout << args[1] << " is " << execPath << "\n";
                }
                else
                {
                    cerr << args[1] << ": not found\n";
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
