#include <windows.h>
#include <iostream>

int main() {
    // Path to the shell executable
    std::string shellPath = "shell.exe";

    // Command to open a new cmd window and run the shell
    std::string command = "cmd /c start cmd /c \"" + shellPath + "\"";

    // Set up process startup info
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    // Create a new process (new terminal window)
    if (!CreateProcess(nullptr, const_cast<char*>(command.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "Failed to open new terminal.\n";
        return 1;
    }

    // Close process handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
