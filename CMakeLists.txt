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

# Executable (only main.cpp)
add_executable(cs3_ai main.cpp)

# Link libraries
target_link_libraries(cs3_ai raylib nlohmann_json::nlohmann_json)

# Copy resources to build directory
configure_file(resources/models/dust2.obj ${CMAKE_BINARY_DIR}/resources/models/dust2.obj COPYONLY)
configure_file(resources/models/ak47.obj ${CMAKE_BINARY_DIR}/resources/models/ak47.obj COPYONLY)
configure_file(resources/models/awp.obj ${CMAKE_BINARY_DIR}/resources/models/awp.obj COPYONLY)
configure_file(resources/sounds/shot.wav ${CMAKE_BINARY_DIR}/resources/sounds/shot.wav COPYONLY)
configure_file(resources/sounds/footsteps.wav ${CMAKE_BINARY_DIR}/resources/sounds/footsteps.wav COPYONLY)

# Copy raylib.dll if needed
if(WIN32)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()