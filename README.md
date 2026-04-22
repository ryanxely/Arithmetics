# README.md

# Project Title

This project serves as a universal, professional C/C++ project base template designed to scale into various types of applications over time. It provides a reusable foundation that can evolve into a numerical library, a desktop application with a GUI, a systems tool, a shared library, a networked application, an embedded-style module, or a mixed C/C++ project.

## Project Structure

- **src/**: Contains all source files (.c and .cpp).
  - **core/**: Pure logic, algorithms, and data structures (no dependencies).
  - **io/**: File handling and terminal input/output operations.
  - **net/**: Networking, sockets, and HTTP functionalities (reserved).
  - **ui/**: GUI layer (reserved).
  - **main.cpp**: Entry point of the application.

- **include/**: Contains all public headers (.h and .hpp).
  - **core/**: Public headers for core logic.
  - **io/**: Public headers for I/O operations.
  - **net/**: Public headers for networking.
  - **ui/**: Public headers for GUI components.

- **libs/**: Reserved for third-party libraries.

- **bindings/**: Wrappers to expose the project to Python, JS, etc.
  - **python_bridge.py**: Template for a Python bridge using ctypes.

- **tests/**: Unit tests for every module.
  - **test_template.cpp**: Minimal test template.

- **scripts/**: Utility shell/python scripts.
  - **new_module.sh**: Script to generate new module files.

- **docs/**: Reserved for documentation.

- **assets/**: Icons, fonts, configuration files, and data files.

- **build/**: Compiled output directory (not committed to git).

- **CMakeLists.txt**: Primary build system configuration.

- **Makefile**: Convenience wrapper around CMake commands.

- **.gitignore**: Specifies files and directories to ignore in git.

- **.editorconfig**: Enforces consistent formatting across editors.

## How to Build

To build the project, follow these steps:

1. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

2. Run CMake to configure the project:
   ```bash
   cmake ..
   ```

3. Build the project:
   ```bash
   cmake --build .
   ```

### Build Modes

- To build in Debug mode:
  ```bash
  cmake --build . --config Debug
  ```

- To build in Release mode:
  ```bash
  cmake --build . --config Release
  ```

## How to Add a New Module

To add a new module, use the provided script:

```bash
./scripts/new_module.sh <module_name>
```

This will generate the corresponding `.h`, `.c`, and test files based on the templates.

## Enabling Optional Features

You can enable optional features via CMake flags:

- To enable GUI support:
  ```bash
  cmake -DENABLE_GUI=ON ..
  ```

- To enable networking support:
  ```bash
  cmake -DENABLE_NET=ON ..
  ```

- To enable language bindings:
  ```bash
  cmake -DENABLE_BINDINGS=ON ..
  ```

## Evolution Guide

This template can evolve into various project types:

- **Numerical/Algorithms Library**: Implement core algorithms in `src/core/`.
- **C++ Desktop Application**: Add GUI code in `src/ui/` and link necessary libraries.
- **Systems Tool/CLI Utility**: Implement command-line logic in `src/main.cpp`.
- **Shared Library**: Create appropriate headers in `include/` and implement in `src/`.
- **Networked Application**: Add networking code in `src/net/`.
- **Embedded Module**: Focus on minimal dependencies in `src/io/`.
- **Mixed C/C++ Project**: Use both `src/core/` and `src/ui/` as needed.

This README provides a comprehensive overview of the project structure, build instructions, and guidelines for extending the project.