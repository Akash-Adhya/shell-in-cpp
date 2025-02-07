#include <iostream>
#include <fstream>
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
#include <errno.h>

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

// Function to split the input into tokens
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

// Printing all the files and directories present in the current directory
void ls(){
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData); // Search for all files in the current directory

    if (hFind == INVALID_HANDLE_VALUE) {
        cerr << "Error: Unable to open directory!" << endl;
        exit(0);
    } 

    do {
        string name = findFileData.cFileName;

        // Skip "." and ".."
        if (name == "." || name == "..") {
            continue;
        }

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            cout << name << "\\";
        } else {
            cout<< name;

            // Display file size
            // LARGE_INTEGER fileSize;
            // fileSize.LowPart = findFileData.nFileSizeLow;
            // fileSize.HighPart = findFileData.nFileSizeHigh;
            // cout << " [FILE] Size: " << fileSize.QuadPart << " bytes";
        }

        cout << "\t";
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind); // Close the file handle
    cout<<endl;
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

// Execute `cat` command with multiple file display and line numbering
void catCommand(const vector<string> &args)
{
    bool showLineNumbers = false;
    int startIndex = 1;

    if (args.size() > 1 && args[1] == "-n")
    {
        showLineNumbers = true;
        startIndex = 2;
    }

    if (startIndex >= args.size())
    {
        cerr << "Error: No file specified!" << endl;
        return;
    }

    for (size_t i = startIndex; i < args.size(); i++)
    {
        ifstream file(args[i]);
        if (!file)
        {
            cerr << "Error: Cannot open file " << args[i] << endl;
            continue;
        }

        string line;
        int lineNumber = 1;
        while (getline(file, line))
        {
            if (showLineNumbers)
            {
                cout << lineNumber++ << ": ";
            }
            cout << line << endl;
        }
        file.close();
        cout << endl;
    }
}

// touch command creates single / multiple files
void touch(const vector<string> &args){
    bool err = false;
    for(string filename : args){
        if(filename == "touch") continue;
        ofstream file(filename);    // create the file
        if(!file){
            cerr<<"Error creating file: "<<filename<<endl;
            err = true;
        }
    }
    if(!err) {
        if((args.size()-1) > 1) cout<<"Files created!"<<endl;
        else cout<<"File created!"<<endl;
    }
}

// Del command that deletes single / multiple files
void del(const vector<string> &args) {
    bool err = false;
    for (const string &filename : args) {
        if (filename == "del") continue; // Skip command name if included

        if (remove(filename.c_str()) != 0) {
            cerr << "Error deleting file: " << filename << endl;
            err = true;
        }
    }
    if (!err) {
        if ((args.size() - 1) > 1) cout << "Files deleted!" << endl;
        else cout << "File deleted!" << endl;
    }
}

// Helper function for mkdir command
int createFolder(const string &folderName) {
    if (_mkdir(folderName.c_str()) != 0) {
        if (errno == EEXIST) {
            cerr << "Error: Folder already exists: " << folderName << endl;
        } else {
            cerr << "Error: Unable to create folder: " << folderName << endl;
        }
        return 1; //indicating error
    }
    return 0; //indicates no error
}

// `mkdir` command that creates single or multiple folders
void mkdir(const vector<string> &args){
    int isErr = false;
    for(string filename: args){
        if(filename == "mkdir") continue;
        isErr = createFolder(filename);
        if(isErr) break;
    }
    if(!isErr){
        if((args.size()-1) > 1) cout<<"Folders created!"<<endl;
        else cout<<"Folder created!"<<endl;
    }
}

// Helper function to delete a folder
int deleteFolder(const string &folderName) {
    if (_rmdir(folderName.c_str()) != 0) {
         
        if (errno == ENOENT) {
            cerr << "Error: Folder does not exist: " << folderName << endl;
        } else if (errno == EINVAL) {
            cerr << "Error: Invalid folder name: " << folderName << endl;
        } else {
            cerr << "Error: Unable to delete folder (Folder may not be empty): " << folderName << endl;
        }
        return 1; // Indicating error
    }
    return 0; // No error, do not print anything
}

// `rmdir` command that deletes single or multiple folders
void deldir(const vector<string> &args) {
    int isErr = false;
    for (const string &folderName : args) {
        if (folderName == "deldir") continue; // Skip command name if included
        isErr = deleteFolder(folderName);
        if (isErr) break;
    }
    if (!isErr) {
        if ((args.size() - 1) > 1) cout << "Folders deleted!" << endl;
        else cout << "Folder deleted!" << endl;
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
    vector<string> builtins = {"type", "echo", "exit", "pwd", "cd" , "ls", "cat", "touch", "del", "mkdir", "deldir"};

    // Flush after every std::cout / std::cerr
    cout << unitbuf;
    cerr << unitbuf;

    // Here the coloring of the lines is used
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 6);
    cout<<"<---------------------- WELCOME TO MY SHELL ---------------------->"<<endl;
    SetConsoleTextAttribute(hConsole, 2);
    cout<<"                 Made with ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cout<<"LOVE && RESILIENCE ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    cout<<"by AKASH.                    ";
    cout<<endl<<endl<<endl;
    // SetConsoleTextAttribute(HANDLE, WHITE | BLACK)
    SetConsoleTextAttribute(hConsole, 15 | 0);


    while (true)
    {
        cout << "$ ";

        string input;
        getline(cin, input);

        // Exiting the shell
        if (input == "exit 0")
        {
            cout<<"\n Exiting...\n";
            Sleep(1000);
            exit(0);
            return 0;
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

        // Handle the `clear` command
        else if(command == "clear"){
            system("cls");
        }

        // Handle the `cat` command
        else if (command == "cat")
        {
            catCommand(args);
        }

        // Handle the `ls` command
        else if (command == "ls"){
            ls();
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
                const char *HOMEPATH = getenv("USERPROFILE");
                printf("%s\n", HOMEPATH);
                if (HOMEPATH && chdir(HOMEPATH) == 0)
                { /* Successfully changed to home directory */ }
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

        // Handle the `touch` command
        else if(command == "touch"){
            touch(args);
        }

        // Handle the `mkdir` command
        else if(command == "mkdir"){
            mkdir(args);
        }

        // Handle the `deldir` command
        else if(command == "deldir"){
            deldir(args);
        }

        // Handle the `del` command
        else if(command == "del"){
            del(args);
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
