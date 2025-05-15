#include "parser.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <vector>

using namespace std;

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
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