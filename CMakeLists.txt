cmake_minimum_required(VERSION 3.11)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

# MSVC runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:DLL>")

# Build raylib from external
if(EXISTS ${CMAKE_SOURCE_DIR}/external/raylib/CMakeLists.txt)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/raylib)
else()
    message(FATAL_ERROR "raylib sources not found in external/raylib.")
endif()

# Build nlohmann/json from external
if(EXISTS ${CMAKE_SOURCE_DIR}/external/json/CMakeLists.txt)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/json)
else()
    message(FATAL_ERROR "nlohmann/json sources not found in external/json.")
endif()

# Build Bullet Physics from external
if(EXISTS ${CMAKE_SOURCE_DIR}/external/bullet3/CMakeLists.txt)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/bullet3)
else()
    message(FATAL_ERROR "Bullet Physics sources not found in external/bullet3.")
endif()

# Executable (include HLLib sources directly)
add_executable(cs3_ai main.cpp src/PhysicsManager.cpp src/ResourceManager.cpp src/external/hllib/HLLib.cpp)

# Include directories
target_include_directories(cs3_ai SYSTEM PRIVATE
    ${bullet3_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(cs3_ai
    raylib
    nlohmann_json::nlohmann_json
    BulletDynamics
    BulletCollision
    LinearMath
)

# Copy resources to build directory
file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)

# Copy dlls if needed
if(WIN32)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()

install(DIRECTORY resources DESTINATION .)