cmake_minimum_required(VERSION 3.20)

# Project metadata
project(MyUniversalProject LANGUAGES C CXX)

# Compiler options
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable warnings
if(CMAKE_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
elseif(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

# Source file discovery
file(GLOB_RECURSE SOURCES
    src/*.c
    src/*.cpp
)

# Include directories
include_directories(include)

# Optional features
option(ENABLE_GUI "Enable GUI support" OFF)
option(ENABLE_NET "Enable networking" OFF)
option(ENABLE_BINDINGS "Enable language bindings" OFF)

# Add GUI support if enabled
if(ENABLE_GUI)
    add_definitions(-DENABLE_GUI)
endif()

# Add networking support if enabled
if(ENABLE_NET)
    add_definitions(-DENABLE_NET)
endif()

# Add bindings support if enabled
if(ENABLE_BINDINGS)
    add_definitions(-DENABLE_BINDINGS)
endif()

# Add executable target
add_executable(MyUniversalProject ${SOURCES})

# Linking third-party libraries (stub section)
# target_link_libraries(MyUniversalProject <library_name>)

# Test target
enable_testing()
add_subdirectory(tests)