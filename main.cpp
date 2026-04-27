#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include "src/PhysicsManager.h"
#include "src/MapManager.h"

enum WeaponType { AK47, AWP, DEAGLE };

enum GameState { MENU, MAP_SELECT, PLAYING, BUYING, CONNECTING };

class Player {
public:
    Vector3 position;
    Vector3 velocity;
    Camera camera;
    float speed, gravity, jumpForce, crouchY, bobbingTime;
    bool isCrouching, isGrounded;
    int health, money;
    Sound footstepSound;
    Player() : speed(5.0f), gravity(-9.8f), jumpForce(8.0f), crouchY(0.0f), bobbingTime(0.0f), isCrouching(false), isGrounded(true), health(100), money(800), velocity({0,0,0}) {
        position = {0.0f, 2.0f, 4.0f};
        camera.position = position;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        if (FileExists("resources/sounds/footsteps.wav")) {
            footstepSound = LoadSound("resources/sounds/footsteps.wav");
        }
    }
    void Update(bool& playFootstep) {
        Vector3 targetVel = {0, velocity.y, 0};
        if (IsKeyDown(KEY_W)) targetVel.z -= speed;
        if (IsKeyDown(KEY_S)) targetVel.z += speed;
        if (IsKeyDown(KEY_A)) targetVel.x -= speed;
        if (IsKeyDown(KEY_D)) targetVel.x += speed;
        velocity.x = Vector3Lerp(velocity, targetVel, GetFrameTime() * 5.0f).x;
        velocity.z = Vector3Lerp(velocity, targetVel, GetFrameTime() * 5.0f).z;

        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            isCrouching = true;
            crouchY = -1.0f;
        } else {
            isCrouching = false;
            crouchY = 0.0f;
        }

        if (isGrounded && IsKeyPressed(KEY_SPACE)) {
            velocity.y = jumpForce;
            isGrounded = false;
        }
        velocity.y += gravity * GetFrameTime();
        position.y += velocity.y * GetFrameTime();
        if (position.y < 0.0f) {
            position.y = 0.0f;
            velocity.y = 0;
            isGrounded = true;
        }

        position.x += velocity.x * GetFrameTime();
        position.z += velocity.z * GetFrameTime();

        camera.position = position;
        camera.position.y += crouchY;

        if (fabs(velocity.x) > 0.1f || fabs(velocity.z) > 0.1f) {
            bobbingTime += GetFrameTime() * 10.0f;
            camera.position.y += sinf(bobbingTime) * 0.05f;
            if (fmod(bobbingTime, 1.0f) < 0.1f) playFootstep = true;
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
        camera.target.y -= amount;
    }
};

class Weapon {
public:
    WeaponType type;
    float sway, recoil, fireRate, zoomFOV;
    bool isZoomed;
    Sound shootSound;
    Model weaponModel;
    Weapon(WeaponType t = AK47) : type(t), sway(0), recoil(0), fireRate(0), zoomFOV(60.0f), isZoomed(false) {
        if (t == AWP) zoomFOV = 20.0f;
        std::string soundPath = "resources/sounds/shot.wav";
        if (FileExists(soundPath.c_str())) {
            shootSound = LoadSound(soundPath.c_str());
        }
        std::string modelPath = "resources/models/ak47.obj";
        if (FileExists(modelPath.c_str())) {
            weaponModel = LoadModel(modelPath.c_str());
        }
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
    void Draw(float bobbing, bool crouching) {
        float tilt = -0.1f;
        float crouchOffset = crouching ? -0.5f : 0.0f;
        Vector3 pos = {1.0f + sinf(sway) * 0.1f - recoil, -0.5f + cosf(sway) * 0.1f + tilt + bobbing * 0.1f + crouchOffset, 1.0f};
        if (type == AK47) {
            if (IsModelReady(weaponModel)) {
                DrawModel(weaponModel, pos, 1.0f, WHITE);
            } else {
                DrawCube(pos, 1.0f, 1.0f, 1.0f, RED);
            }
        } else if (type == AWP) {
            DrawCylinder({pos.x + 0.7f, pos.y, pos.z}, 0.04f, 0.04f, 1.2f, 16, DARKGREEN);
            DrawCube({pos.x, pos.y, pos.z}, 0.5f, 0.15f, 0.7f, DARKGREEN);
            DrawCylinder({pos.x + 0.2f, pos.y + 0.1f, pos.z}, 0.02f, 0.02f, 0.2f, 16, BLACK);
            DrawCube({pos.x - 0.4f, pos.y, pos.z}, 0.3f, 0.1f, 0.5f, BROWN);
            DrawCube({pos.x - 0.2f, pos.y - 0.05f, pos.z}, 0.05f, 0.1f, 0.1f, GRAY);
        } else {
            DrawCylinder({pos.x + 0.2f, pos.y, pos.z}, 0.01f, 0.02f, 0.3f, 16, LIGHTGRAY);
            DrawCube({pos.x - 0.1f, pos.y, pos.z}, 0.15f, 0.05f, 0.2f, DARKBROWN);
        }
        if (fireRate > 0 && fireRate < 0.05f) {
            DrawSphere({pos.x + 0.8f, pos.y, pos.z}, 0.1f, YELLOW);
        }
    }
    void Shoot(Player& player) {
        PlaySound(shootSound);
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
    void GenerateDust2() {
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (i == 0 || i == 19 || j == 0 || j == 19) {
                    wallPositions.push_back({(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f});
                }
                if ((i == 5 && j > 5 && j < 15) || (i == 10 && j > 2 && j < 8) || (i == 15 && j > 10 && j < 18)) {
                    boxPositions.push_back({(float)j * 2.0f - 20.0f, 0.5f, (float)i * 2.0f - 20.0f});
                }
            }
        }
    }
    void Draw() {
        DrawCube({0.0f, -0.5f, 0.0f}, 100.0f, 1.0f, 100.0f, BEIGE);
        for (const auto& pos : wallPositions) {
            DrawCube(pos, 1.0f, 4.0f, 1.0f, YELLOW);
        }
        for (const auto& pos : boxPositions) {
            DrawCube(pos, 1.0f, 1.0f, 1.0f, DARKGRAY);
        }
    }
};

class UIManager {
public:
    static void DrawMenu(int w, int h, std::vector<std::string>& assetsFound) {
        DrawText("CS 3 AI", w/2 - 50, h/2 - 100, 40, BLACK);
        DrawText("Press 1: Single Player", w/2 - 120, h/2 - 50, 20, GRAY);
        DrawText("Press 2: Multiplayer", w/2 - 120, h/2 - 20, 20, GRAY);
        DrawText("Press 3: Connect to CS 1.6", w/2 - 140, h/2 + 10, 20, GRAY);
        DrawText("Assets Loaded:", w/2 - 80, h/2 + 50, 20, GREEN);
        for (size_t i = 0; i < assetsFound.size(); ++i) {
            DrawText(assetsFound[i].c_str(), w/2 - 100, h/2 + 70 + i * 20, 15, LIGHTGRAY);
        }
    }
    static void DrawMapSelect(int w, int h, std::vector<std::string>& maps) {
        DrawText("Select Map", w/2 - 60, h/2 - 100, 30, BLACK);
        for (size_t i = 0; i < maps.size(); ++i) {
            DrawText(TextFormat("%d. %s", i+1, maps[i].c_str()), w/2 - 100, h/2 - 50 + i*30, 20, GRAY);
        }
        DrawText("Press ESC to back", w/2 - 80, h/2 + 100, 20, WHITE);
    }
    static void DrawBuyMenu(int w, int h, int money) {
        DrawRectangle(w/2 - 200, h/2 - 150, 400, 300, Fade(BLACK, 0.8f));
        DrawText("Buy Menu", w/2 - 50, h/2 - 120, 30, WHITE);
        DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 90, 20, GREEN);
        DrawRectangle(w/2 - 150, h/2 - 50, 300, 40, GRAY);
        DrawText("1. AK-47 $2700", w/2 - 140, h/2 - 40, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2, 300, 40, GRAY);
        DrawText("2. AWP $4750", w/2 - 140, h/2 + 10, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2 + 50, 300, 40, GRAY);
        DrawText("3. Deagle $700", w/2 - 140, h/2 + 60, 20, BLACK);
        DrawText("Press ESC to exit", w/2 - 100, h/2 + 120, 20, WHITE);
    }
    static void DrawHUD(int health, int ammo, int money) {
        DrawText(TextFormat("Health: %d", health), 10, 10, 20, ORANGE);
        DrawText(TextFormat("Ammo: %d", ammo), 10, 35, 20, YELLOW);
        DrawText(TextFormat("Money: $%d", money), 10, 60, 20, GREEN);
    }
    static void DrawCrosshair(int w, int h) {
        int cx = w/2, cy = h/2;
        DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
        DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
    }
};

// Function to detect available assets in resources folder
// Checks for basic assets and CS 1.6 format files
std::vector<std::string> DetectAssets() {
    std::vector<std::string> assets;
    std::vector<std::string> dirs = {"models", "sounds"};
    std::vector<std::string> exts = {".obj", ".mdl", ".wav"};
    for (auto& dir : dirs) {
        for (auto& ext : exts) {
            std::string path = "resources/" + dir + "/ak47" + ext;
            if (FileExists(path.c_str())) assets.push_back(dir + ": ak47" + ext);
            path = "resources/" + dir + "/awp" + ext;
            if (FileExists(path.c_str())) assets.push_back(dir + ": awp" + ext);
            path = "resources/" + dir + "/shot" + ext;
            if (FileExists(path.c_str())) assets.push_back(dir + ": shot" + ext);
        }
    }

    // Auto-detect CS 1.6 resources path and check for .wad, .mdl, .bsp files
    std::filesystem::path resourcesPath = "resources";
    if (std::filesystem::exists(resourcesPath) && std::filesystem::is_directory(resourcesPath)) {
        // Check for .wad files (textures)
        for (const auto& entry : std::filesystem::recursive_directory_iterator(resourcesPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".wad") {
                    assets.push_back("CS 1.6 textures: " + entry.path().filename().string());
                } else if (ext == ".mdl") {
                    assets.push_back("CS 1.6 models: " + entry.path().filename().string());
                } else if (ext == ".bsp") {
                    assets.push_back("CS 1.6 maps: " + entry.path().filename().string());
                }
            }
        }
    } else {
        assets.push_back("Resources folder not found");
    }

    return assets;
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    GameState currentState = MENU;
    std::vector<std::string> assetsFound = DetectAssets();
    std::vector<std::string> maps = {"dust2", "mirage", "inferno", "cache"};
    std::string selectedMap = "dust2";
    Player player;
    Weapon weapon(AK47);
    MapManager mapManager;
    mapManager.GenerateDust2();

    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    InitAudioDevice();
    SetTargetFPS(60);
    DisableCursor();

    bool playFootstep = false;

    while (!WindowShouldClose()) {
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ONE)) currentState = MAP_SELECT;
            if (IsKeyPressed(KEY_TWO)) currentState = MAP_SELECT;
            if (IsKeyPressed(KEY_THREE)) currentState = CONNECTING;
        } else if (currentState == MAP_SELECT) {
            for (size_t i = 0; i < maps.size(); ++i) {
                if (IsKeyPressed(KEY_ONE + i)) {
                    selectedMap = maps[i];
                    currentState = PLAYING;
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = MENU;
        } else if (currentState == PLAYING) {
            player.LookAround();
            player.Update(playFootstep);
            if (playFootstep) {
                PlaySound(player.footstepSound);
                playFootstep = false;
            }
            bool moving = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
            weapon.Update(moving, player);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) weapon.Shoot(player);
            if (IsKeyPressed(KEY_B)) currentState = BUYING;
        } else if (currentState == BUYING) {
            if (IsKeyPressed(KEY_ONE) && player.money >= 2700) { player.money -= 2700; weapon = Weapon(AK47); }
            if (IsKeyPressed(KEY_TWO) && player.money >= 4750) { player.money -= 4750; weapon = Weapon(AWP); }
            if (IsKeyPressed(KEY_THREE) && player.money >= 700) { player.money -= 700; weapon = Weapon(DEAGLE); }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = PLAYING;
        } else if (currentState == CONNECTING) {
            // Cannot connect
        }

        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            UIManager::DrawMenu(screenWidth, screenHeight, assetsFound);
        } else if (currentState == MAP_SELECT) {
            ClearBackground(BLUE);
            UIManager::DrawMapSelect(screenWidth, screenHeight, maps);
        } else if (currentState == BUYING) {
            ClearBackground(BLACK);
            UIManager::DrawBuyMenu(screenWidth, screenHeight, player.money);
        } else if (currentState == PLAYING) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.Draw();
            EndMode3D();
            weapon.Draw(sinf(player.bobbingTime), player.isCrouching);
            UIManager::DrawHUD(player.health, 30, player.money);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
        } else if (currentState == CONNECTING) {
            ClearBackground(BLACK);
            DrawText("Cannot connect to CS 1.6 servers", screenWidth/2 - 200, screenHeight/2, 20, RED);
            DrawText("Press ESC to return", screenWidth/2 - 100, screenHeight/2 + 30, 20, WHITE);
        }
        EndDrawing();
    }

    UnloadModel(weapon.weaponModel);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}