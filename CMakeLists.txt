cmake_minimum_required(VERSION 3.11)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

# Find raylib
find_package(raylib REQUIRED)

# Find nlohmann_json
find_package(nlohmann_json REQUIRED)

# MSVC runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:DLL>")

# Executable (only main.cpp)
add_executable(cs3_ai main.cpp)

# Link libraries
target_link_libraries(cs3_ai raylib nlohmann_json::nlohmann_json)

# Copy entire resources folder to build directory
add_custom_command(TARGET cs3_ai POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:cs3_ai>/resources
    COMMENT "Copying resources (models, sounds, maps) to build directory"
)

# Copy raylib.dll if needed
if(WIN32)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()