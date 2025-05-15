#include "commands.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <errno.h>
#include <limits.h>

using namespace std;

// Printing all the files and directories present in the current directory
void ls()
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("*", &findFileData); // Search for all files in the current directory

    if (hFind == INVALID_HANDLE_VALUE)
    {
        cerr << "Error: Unable to open directory!" << endl;
        exit(0);
    }

    do
    {
        string name = findFileData.cFileName;

        // Skip "." and ".."
        if (name == "." || name == "..")
        {
            continue;
        }

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            cout << name << "\\";
        }
        else
        {
            cout << name;

            // Display file size
            // LARGE_INTEGER fileSize;
            // fileSize.LowPart = findFileData.nFileSizeLow;
            // fileSize.HighPart = findFileData.nFileSizeHigh;
            // cout << " [FILE] Size: " << fileSize.QuadPart << " bytes";
        }

        cout << "\t";
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind); // Close the file handle
    cout << endl;
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
void touch(const vector<string> &args)
{
    bool err = false;
    for (string filename : args)
    {
        if (filename == "touch")
            continue;
        ofstream file(filename); // create the file
        if (!file)
        {
            cerr << "Error creating file: " << filename << endl;
            err = true;
        }
    }
    if (!err)
    {
        if ((args.size() - 1) > 1)
            cout << "Files created!" << endl;
        else
            cout << "File created!" << endl;
    }
}

// Del command that deletes single / multiple files
void del(const vector<string> &args)
{
    bool err = false;
    for (const string &filename : args)
    {
        if (filename == "del")
            continue; // Skip command name if included

        if (remove(filename.c_str()) != 0)
        {
            cerr << "Error deleting file: " << filename << endl;
            err = true;
        }
    }
    if (!err)
    {
        if ((args.size() - 1) > 1)
            cout << "Files deleted!" << endl;
        else
            cout << "File deleted!" << endl;
    }
}

// Helper function for mkdir command
int createFolder(const string &folderName)
{
    if (_mkdir(folderName.c_str()) != 0)
    {
        if (errno == EEXIST)
        {
            cerr << "Error: Folder already exists: " << folderName << endl;
        }
        else
        {
            cerr << "Error: Unable to create folder: " << folderName << endl;
        }
        return 1; // indicating error
    }
    return 0; // indicates no error
}

// `mkdir` command that creates single or multiple folders
void mkdir(const vector<string> &args)
{
    int isErr = false;
    for (string filename : args)
    {
        if (filename == "mkdir")
            continue;
        isErr = createFolder(filename);
        if (isErr)
            break;
    }
    if (!isErr)
    {
        if ((args.size() - 1) > 1)
            cout << "Folders created!" << endl;
        else
            cout << "Folder created!" << endl;
    }
}

// Helper function to delete a folder
int deleteFolder(const string &folderName)
{
    if (_rmdir(folderName.c_str()) != 0)
    {

        if (errno == ENOENT)
        {
            cerr << "Error: Folder does not exist: " << folderName << endl;
        }
        else if (errno == EINVAL)
        {
            cerr << "Error: Invalid folder name: " << folderName << endl;
        }
        else
        {
            cerr << "Error: Unable to delete folder (Folder may not be empty): " << folderName << endl;
        }
        return 1; // Indicating error
    }
    return 0; // No error, do not print anything
}

// `rmdir` command that deletes single or multiple folders
void deldir(const vector<string> &args)
{
    int isErr = false;
    for (const string &folderName : args)
    {
        if (folderName == "deldir")
            continue; // Skip command name if included
        isErr = deleteFolder(folderName);
        if (isErr)
            break;
    }
    if (!isErr)
    {
        if ((args.size() - 1) > 1)
            cout << "Folders deleted!" << endl;
        else
            cout << "Folder deleted!" << endl;
    }
}

// Echo command
void echo(const std::vector<std::string> &args)
{
    for (size_t i = 1; i < args.size(); ++i)
    {
        if (i > 1)
            cout << " ";
        cout << args[i];
    }
    cout << endl;
}

void changeDirectory(const vector<string> &args)
{
    if (args.size() == 1 || args[1] == "~")
    {
        const char *HOMEPATH = getenv("USERPROFILE");
        if (HOMEPATH)
            cout << HOMEPATH << endl;

        if (HOMEPATH && _chdir(HOMEPATH) == 0)
        {
            // Successfully changed to home directory
        }
        else
        {
            cerr << "cd: Unable to access home directory" << endl;
        }
    }
    else if (_chdir(args[1].c_str()) != 0)
    {
        cerr << "cd: " << args[1] << ": No such file or directory" << endl;
    }
}

// Function to check if a command is a built-in
bool isBuiltin(const std::string &cmd)
{
    static const std::vector<std::string> builtins = {
        "type", "echo", "exit", "pwd", "cd", "ls", "cat", "touch", "del", "mkdir", "deldir", "clear"};
    return std::find(builtins.begin(), builtins.end(), cmd) != builtins.end();
}