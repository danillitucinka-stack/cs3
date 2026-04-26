#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

enum GameState { MENU, PLAYING, BUYING };
enum WeaponType { AK47, AWP, DEAGLE };

class Player {
public:
    Vector3 position;
    Vector3 velocity;
    Camera camera;
    float speed, gravity, jumpForce, groundY;
    float bobbingTime;
    int health, money;
    Player() : speed(5.0f), gravity(-9.8f), jumpForce(8.0f), groundY(0.0f), bobbingTime(0.0f), health(100), money(800), velocity({0,0,0}) {
        position = {0.0f, 2.0f, 4.0f};
        camera.position = position;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }
    void Update(const std::vector<Vector3>& walls) {
        // Movement with inertia
        Vector3 targetVel = {0, velocity.y, 0};
        float currentSpeed = speed;
        if (IsKeyDown(KEY_LEFT_SHIFT)) currentSpeed *= 2.0f;  // Run
        if (IsKeyDown(KEY_W)) targetVel.z -= currentSpeed;
        if (IsKeyDown(KEY_S)) targetVel.z += currentSpeed;
        if (IsKeyDown(KEY_A)) targetVel.x -= currentSpeed;
        if (IsKeyDown(KEY_D)) targetVel.x += currentSpeed;
        velocity.x = Vector2Lerp({velocity.x, 0}, {targetVel.x, 0}, GetFrameTime() * 5.0f).x;
        velocity.z = Vector2Lerp({velocity.z, 0}, {targetVel.z, 0}, GetFrameTime() * 5.0f).x;

        // Jump
        if (IsKeyPressed(KEY_SPACE) && position.y <= groundY + 0.1f) {
            velocity.y = jumpForce;
        }
        velocity.y += gravity * GetFrameTime();
        position.y += velocity.y * GetFrameTime();
        if (position.y < groundY) {
            position.y = groundY;
            velocity.y = 0;
        }

        position.x += velocity.x * GetFrameTime();
        position.z += velocity.z * GetFrameTime();

        // Collision
        bool canMove = true;
        for (const auto& wall : walls) {
            if (Vector3Distance(position, wall) < 1.5f) {
                canMove = false;
                velocity = {0, velocity.y, 0};
                break;
            }
        }
        if (!canMove) {
            position.x -= velocity.x * GetFrameTime();
            position.z -= velocity.z * GetFrameTime();
        }

        camera.position = position;

        // Bobbing
        if (fabs(velocity.x) > 0.1f || fabs(velocity.z) > 0.1f) {
            bobbingTime += GetFrameTime() * 10.0f;
            camera.position.y += sinf(bobbingTime) * 0.1f;
        } else {
            bobbingTime = 0.0f;
        }
    }
    void LookAround() {
        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x += mouseDelta.x * 0.005f;
        camera.target.y += mouseDelta.y * 0.005f;
    }
    void TakeRecoil(float amount) {
        camera.target.y -= amount;  // Up recoil
    }
};

class Weapon {
public:
    WeaponType type;
    float sway, recoil, fireRate, zoomFOV;
    bool isZoomed;
    Weapon(WeaponType t = AK47) : type(t), sway(0), recoil(0), fireRate(0), zoomFOV(60.0f), isZoomed(false) {
        if (t == AWP) zoomFOV = 20.0f;
    }
    void Update(bool moving, Player& player) {
        if (moving) sway += 0.2f;
        else sway *= 0.9f;
        if (recoil > 0) recoil *= 0.85f;
        if (type == AK47 && fireRate > 0) fireRate -= GetFrameTime();
        if (type == AWP && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            isZoomed = true;
            player.camera.fovy = zoomFOV;
        } else {
            isZoomed = false;
            player.camera.fovy = 60.0f;
        }
    }
    void Draw(float bobbing) {
        float tilt = -0.1f;
        Vector3 pos = {1.0f + sinf(sway) * 0.1f - recoil, -0.5f + cosf(sway) * 0.1f + tilt + bobbing * 0.1f, 1.0f};
        if (type == AK47) {
            DrawCylinder({pos.x + 0.5f, pos.y, pos.z}, 0.05f, 0.05f, 0.8f, 16, GRAY);  // Barrel
            DrawCube({pos.x, pos.y, pos.z}, 0.4f, 0.15f, 0.6f, DARKGRAY);  // Receiver
            DrawCapsule({pos.x - 0.3f, pos.y, pos.z}, {pos.x - 0.1f, pos.y, pos.z}, 0.1f, 16, 16, BROWN);  // Stock
            DrawCube({pos.x - 0.1f, pos.y - 0.1f, pos.z}, 0.1f, 0.2f, 0.1f, BROWN);  // Handle
            DrawCube({pos.x, pos.y - 0.05f, pos.z + 0.2f}, 0.08f, 0.4f, 0.05f, DARKGRAY);  // Mag
        } else if (type == AWP) {
            DrawCylinder({pos.x + 0.7f, pos.y, pos.z}, 0.04f, 0.04f, 1.2f, 16, GREEN);  // Barrel
            DrawCube({pos.x, pos.y, pos.z}, 0.5f, 0.15f, 0.7f, DARKGREEN);  // Receiver
            DrawCylinder({pos.x + 0.2f, pos.y + 0.1f, pos.z}, 0.02f, 0.02f, 0.2f, 16, BLACK);  // Scope
            DrawCube({pos.x - 0.4f, pos.y, pos.z}, 0.3f, 0.1f, 0.5f, BROWN);  // Stock
            DrawCube({pos.x - 0.2f, pos.y - 0.05f, pos.z}, 0.05f, 0.1f, 0.1f, GRAY);  // Bipod
        } else {
            DrawCapsule({pos.x + 0.2f, pos.y, pos.z}, {pos.x + 0.3f, pos.y, pos.z}, 0.01f, 16, 16, SILVER);  // Blade
            DrawCube({pos.x - 0.1f, pos.y, pos.z}, 0.15f, 0.05f, 0.2f, DARKBROWN);  // Handle
        }
        // Muzzle flash
        if (fireRate > 0 && fireRate < 0.05f) {
            DrawSphere({pos.x + 0.8f, pos.y, pos.z}, 0.1f, YELLOW);
        }
    }
    void Shoot(Player& player) {
        if (type == AK47 && fireRate <= 0) {
            recoil = 0.15f;
            fireRate = 0.1f;
            player.TakeRecoil(0.1f);
        } else if (type == AWP) {
            recoil = 0.6f;
            player.TakeRecoil(0.2f);
        } else {
            recoil = 0.3f;
            player.TakeRecoil(0.15f);
        }
    }
};

class MapManager {
public:
    std::vector<Vector3> wallPositions;
    std::vector<Vector3> boxPositions;
    std::vector<Vector3> rampPositions;
    void GenerateDust2() {
        // A-Site: walls, boxes, ramp
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if ((i == 0 || i == 19 || j == 0 || j == 19) && !(i == 10 && j > 5 && j < 15)) {  // Walls with opening
                    wallPositions.push_back({(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f});
                }
                if ((i == 5 && j > 5 && j < 15) || (i == 10 && j > 2 && j < 8)) {  // Boxes
                    boxPositions.push_back({(float)j * 2.0f - 20.0f, 0.5f, (float)i * 2.0f - 20.0f});
                }
                if (i > 12 && i < 18 && j > 10 && j < 16) {  // Ramp area
                    rampPositions.push_back({(float)j * 2.0f - 20.0f, (float)(i - 12) * 0.5f, (float)i * 2.0f - 20.0f});
                }
            }
        }
    }
    void Draw() {
        // Ground
        DrawCube({0.0f, -0.5f, 0.0f}, 100.0f, 1.0f, 100.0f, BEIGE);
        // Walls
        for (const auto& pos : wallPositions) {
            DrawCube(pos, 1.0f, 4.0f, 1.0f, YELLOW);
        }
        // Boxes
        for (const auto& pos : boxPositions) {
            DrawCube(pos, 1.0f, 1.0f, 1.0f, DARKGRAY);
        }
        // Ramp
        for (const auto& pos : rampPositions) {
            DrawCube(pos, 1.0f, 0.5f, 1.0f, GRAY);
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
        DrawRectangle(w/2 - 200, h/2 - 150, 400, 300, Fade(BLACK, 0.8f));
        DrawText("Buy Menu", w/2 - 50, h/2 - 120, 30, WHITE);
        DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 90, 20, GREEN);
        // Buttons
        DrawRectangle(w/2 - 150, h/2 - 50, 300, 40, GRAY);
        DrawText("1. AK-47 $2700", w/2 - 140, h/2 - 40, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2, 300, 40, GRAY);
        DrawText("2. AWP $4750", w/2 - 140, h/2 + 10, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2 + 50, 300, 40, GRAY);
        DrawText("3. Deagle $700", w/2 - 140, h/2 + 60, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2 + 100, 300, 40, GRAY);
        DrawText("4. Knife $0", w/2 - 140, h/2 + 110, 20, BLACK);
        DrawText("Press ESC to exit", w/2 - 100, h/2 + 150, 20, WHITE);
    }
    static void DrawHUD(int health, int ammo, int money) {
        DrawText(TextFormat("Health: %d Ammo: %d Money: $%d", health, ammo, money), 10, 10, 20, WHITE);
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
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) weapon.Shoot(player);
            if (IsKeyPressed(KEY_B)) currentState = BUYING;
        } else if (currentState == BUYING) {
            if (IsKeyPressed(KEY_ONE) && player.money >= 2700) { player.money -= 2700; weapon = Weapon(AK47); }
            if (IsKeyPressed(KEY_TWO) && player.money >= 4750) { player.money -= 4750; weapon = Weapon(AWP); }
            if (IsKeyPressed(KEY_THREE) && player.money >= 700) { player.money -= 700; weapon = Weapon(DEAGLE); }
            if (IsKeyPressed(KEY_FOUR)) { weapon = Weapon(AK47); }  // Knife placeholder
            if (IsKeyPressed(KEY_ESCAPE)) currentState = PLAYING;
        }

        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            UIManager::DrawMenu(screenWidth, screenHeight);
        } else if (currentState == BUYING) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.Draw();
            EndMode3D();
            UIManager::DrawBuyMenu(screenWidth, screenHeight, player.money);
        } else if (currentState == PLAYING) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.Draw();
            EndMode3D();
            weapon.Draw(sinf(player.bobbingTime));
            UIManager::DrawHUD(player.health, 30, player.money);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}