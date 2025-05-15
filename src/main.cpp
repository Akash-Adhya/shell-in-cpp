#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <io.h>
#include <cstdlib>
#include <unistd.h>

#include "commands.h"
#include "parser.h"
#include "utils.h"

using namespace std;

int main()
{
    cout << unitbuf;
    cerr << unitbuf;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 6);
    cout << "<---------------------- WELCOME TO MY SHELL ---------------------->" << endl;
    SetConsoleTextAttribute(hConsole, 2);
    cout << "                 Made with ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cout << "LOVE && RESILIENCE ";
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    cout << "by AKASH.                    ";
    cout << endl << endl << endl;
    SetConsoleTextAttribute(hConsole, 15 | 0);

    while (true)
    {
        cout << "$ ";

        string input;
        getline(cin, input);

        if (input == "exit 0")
        {
            cout << "\n Exiting...\n";
            Sleep(1000);
            exit(0);
        }

        vector<string> args = splitInput(input);
        if (args.empty()) continue;

        string command = args[0];

        if (command == "echo")
        {
            echo(args);
        }
        else if (command == "clear")
        {
            system("cls");
        }
        else if (command == "cat")
        {
            catCommand(args);
        }
        else if (command == "ls")
        {
            ls();
        }
        else if (command == "pwd")
        {
            if (args.size() == 1)
                pwd();
            else
                cerr << "pwd: No parameters required." << endl;
        }
        else if (command == "cd")
        {
            changeDirectory(args);
        }
        else if (command == "touch")
        {
            touch(args);
        }
        else if (command == "mkdir")
        {
            mkdir(args);
        }
        else if (command == "deldir")
        {
            deldir(args);
        }
        else if (command == "del")
        {
            del(args);
        }
        else if (command == "type")
        {
            if (args.size() == 1)
            {
                cerr << "type: missing argument\n";
                continue;
            }

            if (isBuiltin(args[1]))
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
