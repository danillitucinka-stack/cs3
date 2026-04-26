cmake_minimum_required(VERSION 3.10)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

# Find Raylib
find_package(raylib REQUIRED)

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

# Copy assets to build directory (optional for build)
# configure_file(assets/textures/wall_texture.png ${CMAKE_BINARY_DIR}/assets/textures/wall_texture.png COPYONLY)
# configure_file(assets/textures/bot_texture.png ${CMAKE_BINARY_DIR}/assets/textures/bot_texture.png COPYONLY)