#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

enum GameState { MENU, PLAYING, BUYING };
enum WeaponType { AK47, AWP, KNIFE };

class Player {
public:
    Vector3 position;
    Camera camera;
    float speed;
    int health, money;
    Player() : speed(5.0f), health(100), money(800) {
        position = {0.0f, 2.0f, 4.0f};
        camera.position = position;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }
    void Update(const std::vector<Vector3>& walls) {
        Vector3 newPos = position;
        if (IsKeyDown(KEY_W)) newPos.z -= speed * GetFrameTime();
        if (IsKeyDown(KEY_S)) newPos.z += speed * GetFrameTime();
        if (IsKeyDown(KEY_A)) newPos.x -= speed * GetFrameTime();
        if (IsKeyDown(KEY_D)) newPos.x += speed * GetFrameTime();
        // Collision check
        bool canMove = true;
        for (const auto& wall : walls) {
            if (Vector3Distance(newPos, wall) < 1.5f) {
                canMove = false;
                break;
            }
        }
        if (canMove) position = newPos;
        camera.position = position;
    }
    void LookAround() {
        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x += mouseDelta.x * 0.003f;
        camera.target.y += mouseDelta.y * 0.003f;
    }
};

class Weapon {
public:
    WeaponType type;
    float sway, recoil, fireRate, zoomFOV;
    Weapon(WeaponType t = AK47) : type(t), sway(0), recoil(0), fireRate(0), zoomFOV(60.0f) {
        if (t == AWP) zoomFOV = 30.0f;
    }
    void Update(bool moving, Player& player) {
        if (moving) sway += 0.1f;
        else sway *= 0.9f;
        if (recoil > 0) recoil *= 0.8f;
        if (type == AK47 && fireRate > 0) fireRate -= GetFrameTime();
        if (type == AWP && IsKeyDown(KEY_F)) {
            player.camera.fovy = zoomFOV;
        } else {
            player.camera.fovy = 60.0f;
        }
    }
    void Draw() {
        Vector3 pos = {1.0f + sinf(sway) * 0.1f - recoil, -0.5f + cosf(sway) * 0.1f, 1.0f};
        Color col = (type == AK47) ? BROWN : (type == AWP) ? BLUE : GRAY;
        // Complex shape
        DrawCube(pos, 0.3f, 0.1f, 1.0f, col);  // Barrel
        DrawCube({pos.x - 0.2f, pos.y + 0.1f, pos.z}, 0.1f, 0.2f, 0.1f, col);  // Handle
        DrawCube({pos.x, pos.y - 0.1f, pos.z + 0.3f}, 0.1f, 0.3f, 0.05f, col);  // Mag
        if (type == AWP) DrawCube({pos.x - 0.3f, pos.y + 0.05f, pos.z}, 0.1f, 0.1f, 0.1f, BLACK);  // Scope
    }
    void Shoot() {
        if (type == AK47 && fireRate <= 0) {
            recoil = 0.1f;
            fireRate = 0.1f;
        } else if (type == AWP) {
            recoil = 0.5f;
        } else {
            recoil = 0.05f;
        }
    }
};

class MapManager {
public:
    std::vector<Vector3> wallPositions;
    std::vector<Vector3> boxPositions;
    void GenerateDust2() {
        // Dust2 A-Site layout: walls and boxes
        int map[20][20] = {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        };
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (map[i][j] == 1) {
                    wallPositions.push_back({(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f});
                } else if (map[i][j] == 2) {
                    boxPositions.push_back({(float)j * 2.0f - 20.0f, 0.5f, (float)i * 2.0f - 20.0f});
                }
            }
        }
    }
    void Draw() {
        for (const auto& pos : wallPositions) {
            DrawCube(pos, 1.0f, 4.0f, 1.0f, BEIGE);  // Sand walls
        }
        for (const auto& pos : boxPositions) {
            DrawCube(pos, 1.0f, 1.0f, 1.0f, BROWN);  // Boxes
        }
    }
};

class UIManager {
public:
    static void DrawMenu(int w, int h) {
        DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
        DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
    }
    static void DrawBuyMenu(int w, int h, int money) {
        DrawText("Buy Menu", w/2 - 50, h/2 - 100, 30, BLACK);
        DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 70, 20, GREEN);
        DrawText("1. AK-47 $2700", w/2 - 100, h/2 - 30, 20, BLACK);
        DrawText("2. AWP $4750", w/2 - 100, h/2, 20, BLACK);
        DrawText("3. Knife $0", w/2 - 100, h/2 + 30, 20, BLACK);
        DrawText("Press ESC to exit", w/2 - 100, h/2 + 60, 20, GRAY);
    }
    static void DrawHUD(int health, int ammo) {
        DrawText(TextFormat("Health: %d Ammo: %d", health, ammo), 10, 10, 20, WHITE);
    }
    static void DrawCrosshair(int w, int h) {
        int cx = w/2, cy = h/2;
        DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
        DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
    }
};

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    GameState currentState = MENU;
    Player player;
    Weapon weapon(AK47);
    MapManager mapManager;
    mapManager.GenerateDust2();

    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);
    DisableCursor();

    while (!WindowShouldClose()) {
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) currentState = PLAYING;
        } else if (currentState == PLAYING) {
            player.LookAround();
            player.Update(mapManager.wallPositions);
            bool moving = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
            weapon.Update(moving, player);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) weapon.Shoot();
            if (IsKeyPressed(KEY_B)) currentState = BUYING;
        } else if (currentState == BUYING) {
            if (IsKeyPressed(KEY_ONE) && player.money >= 2700) { player.money -= 2700; weapon = Weapon(AK47); }
            if (IsKeyPressed(KEY_TWO) && player.money >= 4750) { player.money -= 4750; weapon = Weapon(AWP); }
            if (IsKeyPressed(KEY_THREE)) { weapon = Weapon(KNIFE); }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = PLAYING;
        }

        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            UIManager::DrawMenu(screenWidth, screenHeight);
        } else if (currentState == BUYING) {
            ClearBackground(BLACK);
            UIManager::DrawBuyMenu(screenWidth, screenHeight, player.money);
        } else if (currentState == PLAYING) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.Draw();
            EndMode3D();
            weapon.Draw();
            UIManager::DrawHUD(player.health, 30);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}