cmake_minimum_required(VERSION 3.11)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# Fetch raylib 5.0
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Set raylib build options for speed
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# MSVC runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:DLL>")

# Fetch nlohmann/json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Fetch Bullet Physics
FetchContent_Declare(
    bullet3
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG 3.24
)
FetchContent_MakeAvailable(bullet3)

# Fetch HLLib for asset loading
FetchContent_Declare(
    hllib
    GIT_REPOSITORY https://github.com/nemesisx00/HLLib.git
    GIT_TAG master
)
FetchContent_MakeAvailable(hllib)

# Executable
add_executable(cs3_ai main.cpp src/AssetLoader.cpp src/PhysicsManager.cpp)

# Link libraries
target_link_libraries(cs3_ai raylib nlohmann_json::nlohmann_json BulletDynamics BulletCollision LinearMath)

# Copy resources to build directory
file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)

# Copy raylib.dll if needed
if(WIN32)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()

install(DIRECTORY resources DESTINATION .)