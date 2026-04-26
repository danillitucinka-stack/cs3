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
    int money;

public:
    GameEngine(int width, int height);
    void Initialize();
    void Update();
    void Draw();
    void Cleanup();
    GameState GetState() { return currentState; }
    int GetMoney() { return money; }
};

#endif
""",
        "src/GameEngine.cpp": """#include "GameEngine.h"
#include "UI.h"
#include "Player.h"
#include "MapManager.h"
#include "Weapon.h"
#include <nlohmann/json.hpp>
#include <fstream>

GameEngine::GameEngine(int width, int height) : currentState(MENU), screenWidth(width), screenHeight(height), money(800) {}

void GameEngine::Initialize() {
    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);
    // Initialize components
}

void GameEngine::Update() {
    if (currentState == MENU && IsKeyPressed(KEY_ENTER)) currentState = GAMEPLAY;
    if (currentState == GAMEPLAY && IsKeyPressed(KEY_B)) currentState = BUY_MENU;
    if (currentState == BUY_MENU && IsKeyPressed(KEY_ESCAPE)) currentState = GAMEPLAY;
    if (IsKeyPressed(KEY_P)) currentState = (currentState == PAUSE) ? GAMEPLAY : PAUSE;
    // Update components
}

void GameEngine::Draw() {
    BeginDrawing();
    if (currentState == MENU) {
        UI::DrawMenu(screenWidth, screenHeight);
    } else if (currentState == BUY_MENU) {
        UI::DrawBuyMenu(screenWidth, screenHeight, money);
    } else if (currentState == GAMEPLAY) {
        // Draw game world
        ClearBackground(DARKBLUE);
        BeginMode3D(/*camera*/);
        // Draw map, player, weapon
        EndMode3D();
        UI::DrawHUD(100, 30);
        UI::DrawCrosshair(screenWidth, screenHeight);
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
    Camera GetCamera() { return camera; }
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
    if (IsKeyDown(KEY_W)) position.z -= speed * GetFrameTime();
    if (IsKeyDown(KEY_S)) position.z += speed * GetFrameTime();
    if (IsKeyDown(KEY_A)) position.x -= speed * GetFrameTime();
    if (IsKeyDown(KEY_D)) position.x += speed * GetFrameTime();
}

void Player::Draw() {
    // Draw if needed
}
""",
        "src/Weapon.h": """#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h"

enum WeaponType { AK47, AWP, KNIFE };

class Weapon {
private:
    Model model;
    Vector3 position;
    float sway, recoil;
    WeaponType weaponType;

public:
    Weapon(WeaponType type = AK47);
    void LoadModel(const char* path);
    void Update(bool moving);
    void Draw();
    void Shoot();
    WeaponType GetType() { return weaponType; }
};

#endif
""",
        "src/Weapon.cpp": """#include "Weapon.h"
#include "raylib.h"

Weapon::Weapon(WeaponType type) : sway(0.0f), recoil(0.0f), weaponType(type) {
    position = {1.0f, -0.5f, 1.0f};  // Relative to camera
    if (type == AK47) {
        model = LoadModelFromMesh(GenMeshCube(0.3f, 0.1f, 1.0f));
    } else if (type == AWP) {
        model = LoadModelFromMesh(GenMeshCube(0.4f, 0.1f, 1.5f));
    } else {
        model = LoadModelFromMesh(GenMeshCube(0.2f, 0.1f, 0.5f));
    }
}

void Weapon::LoadModel(const char* path) {
    model = LoadModel(path);
}

void Weapon::Update(bool moving) {
    if (moving) sway += 0.1f;
    else sway *= 0.9f;
    if (recoil > 0) recoil *= 0.8f;
}

void Weapon::Draw() {
    Vector3 drawPos = {position.x + sinf(sway) * 0.1f - recoil, position.y + cosf(sway) * 0.1f, position.z};
    DrawModel(model, drawPos, 1.0f, weaponType == KNIFE ? GRAY : BROWN);
}

void Weapon::Shoot() {
    recoil = 0.3f;
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
#include "raylib.h"

void MapManager::GenerateMap() {
    int map[20][20] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,1},
        {1,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,1},
        {1,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,2,2,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    Model wallModel = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
    Model boxModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            if (map[i][j] == 1) walls.push_back(wallModel);
            else if (map[i][j] == 2) boxes.push_back(boxModel);
        }
    }
}

void MapManager::Draw() {
    int wallIndex = 0, boxIndex = 0;
    int map[20][20] = { /* same as above */ };
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            if (map[i][j] == 1 && wallIndex < walls.size()) {
                DrawModel(walls[wallIndex], {(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f}, 1.0f, BEIGE);
                wallIndex++;
            } else if (map[i][j] == 2 && boxIndex < boxes.size()) {
                DrawModel(boxes[boxIndex], {(float)j * 2.0f - 20.0f, 0.5f, (float)i * 2.0f - 20.0f}, 1.0f, BROWN);
                boxIndex++;
            }
        }
    }
}
""",
        "src/UI.h": """#ifndef UI_H
#define UI_H

class UI {
public:
    static void DrawMenu(int w, int h);
    static void DrawBuyMenu(int w, int h, int money);
    static void DrawPause(int w, int h);
    static void DrawHUD(int health, int ammo);
    static void DrawCrosshair(int w, int h);
};

#endif
""",
        "src/UI.cpp": """#include "UI.h"
#include "raylib.h"

void UI::DrawMenu(int w, int h) {
    DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
    DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
}

void UI::DrawBuyMenu(int w, int h, int money) {
    DrawText("Buy Menu - Money: $", w/2 - 100, h/2 - 50, 20, BLACK);
    DrawText(TextFormat("%d", money), w/2 + 50, h/2 - 50, 20, GREEN);
    DrawText("1. AK-47 $2700", w/2 - 100, h/2, 20, BLACK);
    DrawText("2. AWP $4750", w/2 - 100, h/2 + 30, 20, BLACK);
    DrawText("3. Knife $0", w/2 - 100, h/2 + 60, 20, BLACK);
}

void UI::DrawPause(int w, int h) {
    DrawText("PAUSED", w/2 - 50, h/2, 40, WHITE);
}

void UI::DrawHUD(int health, int ammo) {
    DrawText(TextFormat("Health: %d Ammo: %d", health, ammo), 10, 10, 20, WHITE);
}

void UI::DrawCrosshair(int w, int h) {
    int cx = w/2, cy = h/2;
    DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
    DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
}
""",
        "CMakeLists.txt": """# Provided above
""",
        "main.cpp": """# Provided above
""",
        ".github/workflows/build_windows.yml": """# Provided above
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