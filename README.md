# Shell in C++

A simple terminal shell implemented in C++.

## Overview

This project is a basic command-line shell written in C++. It aims to provide users with a minimal interface to execute commands, similar to traditional UNIX shells.

## Features

* **Command Execution**: Execute standard system commands.
* **Custom Prompt**: Displays a custom shell prompt.
* **Basic Error Handling**: Handles simple errors during command execution.
* **Auto Completion**: Automatic command completions.

*Note: This shell is currently in the development phase, and additional features may be added in the future.*

## Getting Started

### Prerequisites

* A C++ compiler (e.g., `g++`)
* A Windows operating system (It is Windows specific)

### Compilation

To compile the shell, navigate to the project directory and run:

```bash
g++ start.cpp -o shell
```

*Ensure that all necessary source files are included in the compilation command.*

### Running the Shell

After compilation, run the shell with:

```bash
./shell
```

You should see the custom shell prompt, indicating that the shell is ready to accept commands.

## Project Structure

* `start.cpp`: The main entry point of the shell.
* `include/`: Directory containing header files.
* `src/`: Directory containing source files.
* `shell.exe`: Compiled executable (may vary based on your operating system).

## License

This project is open-source and available under the [MIT License](LICENSE).

---

*Created with ❤️ by Akash*
