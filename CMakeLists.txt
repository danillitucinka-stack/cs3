cmake_minimum_required(VERSION 3.21)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

# Try to find raylib via vcpkg or system
find_package(raylib QUIET)

if(NOT raylib_FOUND)
    # Fallback: Fetch raylib from GitHub
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Find nlohmann_json
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/MapManager.cpp
    src/UIManager.cpp
    src/GameLoop.cpp
)

# Executable
add_executable(cs3_ai ${SOURCES})

# Link libraries
target_link_libraries(cs3_ai raylib nlohmann_json::nlohmann_json)

# Copy raylib.dll if needed (for Windows runtime)
if(WIN32 AND TARGET raylib)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()