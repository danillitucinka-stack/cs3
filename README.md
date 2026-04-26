# CS 3 AI

A 3D shooter game built with C++ and Raylib, featuring AI-generated textures and procedural levels.

## Prerequisites

- C++ compiler (MSVC on Windows)
- CMake
- vcpkg for dependencies
- Python 3 for AI scripts

## Building

1. Install vcpkg: `git clone https://github.com/Microsoft/vcpkg.git && cd vcpkg && .\bootstrap-vcpkg.bat && .\vcpkg integrate install`
2. Install dependencies: `vcpkg install raylib nlohmann-json`
3. Configure: `mkdir build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake`
4. Build: `cmake --build . --config Release`

## Running the Game

1. Run the AI engine: `python scripts/ai_engine.py`
   - This starts generating textures and bot behaviors in the background.
   - It will create 50+ textures in `assets/textures/` and update `bot_status.json`.
2. Run the game: `./build/cs3_ai.exe`
   - Use WASD to move, mouse to look, left click to shoot.
   - Press Enter in menu to start, P to pause.

## AI Texture Generation

Run `python scripts/ai_engine.py` to start the background process that generates textures via API calls. It checks every 10 seconds for new weapon requests (via `new_weapon_flag.txt`).