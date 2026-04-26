cmake_minimum_required(VERSION 3.11)
project(cs3_ai)

include(FetchContent)

# Try to find raylib via vcpkg
find_package(raylib QUIET)

if(NOT raylib_FOUND)
    # Fallback: Fetch raylib 5.0
    set(RAYLIB_VERSION 5.0)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif()

# Try to find nlohmann/json via vcpkg
find_package(nlohmann_json QUIET)

if(NOT nlohmann_json_FOUND)
    # Fallback: Fetch nlohmann/json
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

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
if(WIN32)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()