#include <cstdlib>
#include <iostream>

int main() {
    // Command to open a new terminal and run the shell program
    const char* shellCommand = "start cmd /k theMainFile.exe";

    // Execute the command
    if (std::system(shellCommand) != 0) {
        std::cerr << "Failed to launch external terminal." << std::endl;
        return 1;
    }

    return 0;
}
