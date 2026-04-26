import os
import shutil

def create_project_structure():
    directories = [
        "src",
        "include",
        "assets/models",
        "assets/textures",
        "scripts",
        "build"
    ]

    for dir_path in directories:
        os.makedirs(dir_path, exist_ok=True)
        print(f"Created directory: {dir_path}")

    # Move existing files to appropriate directories
    if os.path.exists("main.cpp"):
        shutil.move("main.cpp", "src/main.cpp")
        print("Moved main.cpp to src/")

    if os.path.exists("ai_engine.py"):
        shutil.move("ai_engine.py", "scripts/ai_engine.py")
        print("Moved ai_engine.py to scripts/")

    # Create CMakeLists.txt in root
    cmake_content = """cmake_minimum_required(VERSION 3.10)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

# Find Raylib
find_package(raylib REQUIRED)

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

# Copy assets to build directory
configure_file(assets/textures/wall_texture.png ${CMAKE_BINARY_DIR}/assets/textures/wall_texture.png COPYONLY)
configure_file(assets/textures/bot_texture.png ${CMAKE_BINARY_DIR}/assets/textures/bot_texture.png COPYONLY)
"""
    with open("CMakeLists.txt", "w") as f:
        f.write(cmake_content)
    print("Created CMakeLists.txt")

    print("Project structure setup complete!")

if __name__ == "__main__":
    create_project_structure()