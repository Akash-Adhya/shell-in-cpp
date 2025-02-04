#include<iostream>
#include<windows.h>

using namespace std;

int main(){
    string executableName = "start.exe";

    // Define the name of the C++ file
    string cppFilePath = "C:\\Users\\HP\\OneDrive\\Desktop\\something\\shell-in-cpp\\theMainFile.cpp";

    // Command to compile the given C++ file
    string compileCommand = "g++ \"" + cppFilePath + "\" -o \"" + executableName + "\"";

    // Compile the C++ file
    cout << "Compiling the file...\n";
    if (system(compileCommand.c_str()) != 0) {
        cerr << "Compilation failed. Please check the file path and syntax.\n";
        return 1;
    }
    cout << "Compilation successful.\n";

    // Command to open a new cmd window, run the executable, and close the window after execution
    string runCommand = "start cmd /c \"" + executableName + " & exit\"";

    // Run the executable in a new cmd window
    cout << "Running the program in a new command prompt...\n";
    system(runCommand.c_str());

    // Wait for a short duration to ensure the program completes (optional)
    Sleep(2000);

    // Cleanup: Delete the temporary executable file
    string cleanupCommand = "del \"" + executableName + "\"";
    cout << "Cleaning up temporary files...\n";
    system(cleanupCommand.c_str());

    cout << "Done.\n";
    return 0;
}