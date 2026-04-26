import os

def create_project_structure():
    # Create directories
    os.makedirs("src", exist_ok=True)
    os.makedirs("assets/models", exist_ok=True)
    os.makedirs("assets/textures", exist_ok=True)

    # Define files and their initial content
    files = {
        "src/GameEngine.h": """#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "raylib.h"
#include <string>

enum GameState { MENU, BUY_MENU, GAMEPLAY, PAUSE };

class GameEngine {
private:
    GameState currentState;
    int screenWidth, screenHeight;

public:
    GameEngine(int width, int height);
    void Initialize();
    void Update();
    void Draw();
    void Cleanup();
    GameState GetState() { return currentState; }
};

#endif
""",
        "src/GameEngine.cpp": """#include "GameEngine.h"
#include "UI.h"
#include "Player.h"
#include "MapManager.h"

GameEngine::GameEngine(int width, int height) : currentState(MENU), screenWidth(width), screenHeight(height) {}

void GameEngine::Initialize() {
    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);
    // Initialize other components
}

void GameEngine::Update() {
    if (currentState == MENU && IsKeyPressed(KEY_ENTER)) currentState = GAMEPLAY;
    if (currentState == GAMEPLAY && IsKeyPressed(KEY_B)) currentState = BUY_MENU;
    if (currentState == BUY_MENU && IsKeyPressed(KEY_ESCAPE)) currentState = GAMEPLAY;
    if (IsKeyPressed(KEY_P)) currentState = (currentState == PAUSE) ? GAMEPLAY : PAUSE;
    // Update other components
}

void GameEngine::Draw() {
    BeginDrawing();
    if (currentState == MENU) {
        UI::DrawMenu(screenWidth, screenHeight);
    } else if (currentState == BUY_MENU) {
        UI::DrawBuyMenu(screenWidth, screenHeight);
    } else if (currentState == GAMEPLAY) {
        // Draw game
    } else if (currentState == PAUSE) {
        UI::DrawPause(screenWidth, screenHeight);
    }
    EndDrawing();
}

void GameEngine::Cleanup() {
    CloseWindow();
}
""",
        "src/Player.h": """#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

class Player {
private:
    Vector3 position;
    float speed;
    Camera camera;

public:
    Player();
    void Update();
    void Draw();
    Vector3 GetPosition() { return position; }
};

#endif
""",
        "src/Player.cpp": """#include "Player.h"

Player::Player() : speed(5.0f) {
    position = {0.0f, 2.0f, 4.0f};
    camera.position = position;
    camera.target = {0.0f, 2.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void Player::Update() {
    camera.position = position;
    // Movement logic
    if (IsKeyDown(KEY_W)) position.z -= speed * GetFrameTime();
    // Add other keys
}

void Player::Draw() {
    // Draw player if needed
}
""",
        "src/Weapon.h": """#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h"

class Weapon {
private:
    Model model;
    Vector3 position;
    float sway, recoil;

public:
    Weapon();
    void LoadModel(const char* path);
    void Update();
    void Draw();
    void Shoot();
};

#endif
""",
        "src/Weapon.cpp": """#include "Weapon.h"

Weapon::Weapon() : sway(0.0f), recoil(0.0f) {
    position = {1.0f, -0.5f, 1.0f};  // Relative to camera
    // Load or create model
    model = LoadModelFromMesh(GenMeshCube(0.5f, 0.2f, 1.5f));  // Barrel, handle, magazine
}

void Weapon::LoadModel(const char* path) {
    model = LoadModel(path);
}

void Weapon::Update() {
    // Sway and recoil logic
}

void Weapon::Draw() {
    DrawModel(model, position, 1.0f, BROWN);
}

void Weapon::Shoot() {
    recoil = 0.2f;
    // Flash effect
}
""",
        "src/MapManager.h": """#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "raylib.h"
#include <vector>

class MapManager {
private:
    std::vector<Model> walls;
    std::vector<Model> boxes;

public:
    void GenerateMap();
    void Draw();
};

#endif
""",
        "src/MapManager.cpp": """#include "MapManager.h"

void MapManager::GenerateMap() {
    // Generate Dust2-like map: walls, boxes, passages
    // Example: Add walls and boxes
    Model wall = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
    walls.push_back(wall);
    // Add more
}

void MapManager::Draw() {
    for (auto& w : walls) DrawModel(w, {0.0f, 2.0f, 0.0f}, 1.0f, GRAY);
    for (auto& b : boxes) DrawModel(b, {5.0f, 1.0f, 0.0f}, 1.0f, BROWN);
}
""",
        "src/UI.h": """#ifndef UI_H
#define UI_H

class UI {
public:
    static void DrawMenu(int w, int h);
    static void DrawBuyMenu(int w, int h);
    static void DrawPause(int w, int h);
    static void DrawHUD(int health, int ammo);
};

#endif
""",
        "src/UI.cpp": """#include "UI.h"
#include "raylib.h"

void UI::DrawMenu(int w, int h) {
    DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
    DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
}

void UI::DrawBuyMenu(int w, int h) {
    DrawText("Buy Menu: Press 1 for AK, 2 for Pistol", w/2 - 150, h/2, 20, BLACK);
}

void UI::DrawPause(int w, int h) {
    DrawText("PAUSED", w/2 - 50, h/2, 40, WHITE);
}

void UI::DrawHUD(int health, int ammo) {
    DrawText(TextFormat("Health: %d Ammo: %d", health, ammo), 10, 10, 20, WHITE);
}
""",
        "CMakeLists.txt": """cmake_minimum_required(VERSION 3.11)
project(CS3_AI)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# Fetch raylib 5.0
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Fetch nlohmann/json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

# Auto-find sources
file(GLOB SOURCES "src/*.cpp")

# Executable
add_executable(cs3_ai ${SOURCES})

# Link libraries
target_link_libraries(cs3_ai raylib nlohmann_json::nlohmann_json)

# Copy raylib.dll if needed
if(WIN32)
    add_custom_command(TARGET cs3_ai POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:raylib> $<TARGET_FILE_DIR:cs3_ai>
    )
endif()
""",
        "main.cpp": """#include "src/GameEngine.h"

int main() {
    GameEngine engine(1280, 720);
    engine.Initialize();
    while (!WindowShouldClose()) {
        engine.Update();
        engine.Draw();
    }
    engine.Cleanup();
    return 0;
}
"""
    }

    for file_path, content in files.items():
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, "w") as f:
            f.write(content)
        print(f"Created {file_path}")

    print("Project structure created. Push to GitHub!")

if __name__ == "__main__":
    create_project_structure()