#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <iostream>

std::string GetResourcesPath() {
    std::filesystem::path exePath = std::filesystem::current_path();
    return exePath.string() + "/resources/";
}

enum GameState { MENU, MAP_SELECT, SINGLE_PLAYER, MULTIPLAYER, BUYING, CONNECTING };
enum WeaponType { AK47, AWP, DEAGLE };

class Player {
public:
    Vector3 position;
    Vector3 velocity;
    Camera camera;
    float speed, gravity, jumpForce, crouchY, bobbingTime;
    bool isCrouching, isGrounded;
    int health, money;
    Sound footstepSound;
    Player(std::string resPath) : speed(5.0f), gravity(-9.8f), jumpForce(8.0f), crouchY(0.0f), bobbingTime(0.0f), isCrouching(false), isGrounded(true), health(100), money(800), velocity({0,0,0}) {
        position = {0.0f, 2.0f, 4.0f};
        camera.position = position;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        std::string soundPath = resPath + "sounds/footsteps.wav";
        if (FileExists(soundPath.c_str())) {
            footstepSound = LoadSound(soundPath.c_str());
        } else {
            // Fallback: no sound
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
        if (!isGrounded) {
            velocity.x = targetVel.x;
            velocity.z = targetVel.z;
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
    ~Player() {
        UnloadSound(footstepSound);
    }
};

class Weapon {
public:
    Model model;
    WeaponType type;
    float sway, recoil, fireRate, zoomFOV;
    bool isZoomed;
    Sound shootSound;
    Weapon(WeaponType t, std::string resPath) : type(t), sway(0), recoil(0), fireRate(0), zoomFOV(60.0f), isZoomed(false) {
        if (t == AWP) zoomFOV = 20.0f;
        // Try .obj first, then .mdl (converted), fallback to cube
        std::string modelPath = resPath + "models/v_" + std::string(t == AK47 ? "ak47" : t == AWP ? "awp" : "knife") + ".obj";
        if (FileExists(modelPath.c_str())) {
            model = LoadModel(modelPath.c_str());
        } else {
            std::string mdlPath = resPath + "models/v_" + std::string(t == AK47 ? "ak47" : t == AWP ? "awp" : "knife") + ".mdl";
            if (FileExists(mdlPath.c_str())) {
                // Note: Raylib doesn't support .mdl directly. Convert to .obj first.
                model = LoadModelFromMesh(GenMeshCube(0.5f, 0.2f, 1.0f));  // Placeholder
            } else {
                model = LoadModelFromMesh(GenMeshCube(0.5f, 0.2f, 1.0f));
            }
        }
        std::string soundPath = resPath + "sounds/shot.wav";
        if (FileExists(soundPath.c_str())) {
            shootSound = LoadSound(soundPath.c_str());
        } else {
            // Fallback: no sound
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
        DrawModel(model, pos, 1.0f, WHITE);
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
    ~Weapon() {
        UnloadModel(model);
        UnloadSound(shootSound);
    }
};

class MapManager {
public:
    Model mapModel;
    std::vector<Vector3> wallPositions;
    std::string currentMap;
    MapManager(std::string resPath, std::string mapName = "dust2") : currentMap(mapName) {
        LoadMap(resPath, mapName);
    }
    void LoadMap(std::string resPath, std::string mapName) {
        currentMap = mapName;
        std::string mapPath = resPath + "maps/" + mapName + ".obj";
        if (FileExists(mapPath.c_str())) {
            mapModel = LoadModel(mapPath.c_str());
        } else {
            // Generate fallback based on mapName
            if (mapName == "dust2") {
                GenerateDust2();
            } else if (mapName == "mirage") {
                GenerateMirage();
            } else {
                GenerateDust2();  // Default
            }
        }
    }
    void GenerateDust2() {
        mapModel = LoadModelFromMesh(GenMeshCube(10.0f, 1.0f, 10.0f));
        wallPositions.clear();
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (i == 0 || i == 19 || j == 0 || j == 19) {
                    wallPositions.push_back({(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f});
                }
            }
        }
    }
    void GenerateMirage() {
        mapModel = LoadModelFromMesh(GenMeshCube(10.0f, 1.0f, 10.0f));
        wallPositions.clear();
        // Different layout
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (i == 5 && j > 5 && j < 15) {
                    wallPositions.push_back({(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f});
                }
            }
        }
    }
    void Draw() {
        DrawModel(mapModel, {0.0f, 0.0f, 0.0f}, 1.0f, BEIGE);
        for (const auto& pos : wallPositions) {
            DrawCube(pos, 1.0f, 4.0f, 1.0f, YELLOW);
        }
    }
    ~MapManager() {
        UnloadModel(mapModel);
    }
};

class UIManager {
public:
    static void DrawMenu(int w, int h, std::vector<std::string>& assetsFound) {
        DrawText("CS 3 AI", w/2 - 50, h/2 - 100, 40, BLACK);
        DrawText("Press 1: Single Player (Bots)", w/2 - 120, h/2 - 50, 20, GRAY);
        DrawText("Press 2: Multiplayer (Online)", w/2 - 120, h/2 - 20, 20, GRAY);
        DrawText("Press 3: Connect to CS 1.6 Server", w/2 - 140, h/2 + 10, 20, GRAY);
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

std::vector<std::string> DetectAssets(std::string resPath) {
    std::vector<std::string> assets;
    std::vector<std::string> dirs = {"models", "sounds"};
    std::vector<std::string> exts = {".obj", ".mdl", ".wav"};
    for (auto& dir : dirs) {
        std::string path = resPath + dir + "/";
        for (auto& ext : exts) {
            std::string file = "ak47" + ext;
            if (FileExists((path + file).c_str())) assets.push_back(dir + ": " + file);
            file = "awp" + ext;
            if (FileExists((path + file).c_str())) assets.push_back(dir + ": " + file);
            file = "shot" + ext;
            if (FileExists((path + file).c_str())) assets.push_back(dir + ": " + file);
        }
    }
    return assets;
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    std::string resPath = GetResourcesPath();
    GameState currentState = MENU;
    std::vector<std::string> assetsFound = DetectAssets(resPath);
    std::vector<std::string> maps = {"dust2", "mirage", "inferno", "cache"};
    std::string selectedMap = "dust2";
    Player player(resPath);
    Weapon weapon(AK47, resPath);
    MapManager mapManager(resPath, selectedMap);

    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    InitAudioDevice();
    SetTargetFPS(60);
    DisableCursor();

    bool playFootstep = false;

    while (!WindowShouldClose()) {
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ONE)) currentState = MAP_SELECT;
            if (IsKeyPressed(KEY_TWO)) currentState = MAP_SELECT;  // Then multiplayer
            if (IsKeyPressed(KEY_THREE)) currentState = CONNECTING;
        } else if (currentState == MAP_SELECT) {
            for (size_t i = 0; i < maps.size(); ++i) {
                if (IsKeyPressed(KEY_ONE + i)) {
                    selectedMap = maps[i];
                    mapManager.LoadMap(resPath, selectedMap);
                    currentState = (currentState == MAP_SELECT && IsKeyPressed(KEY_ONE)) ? SINGLE_PLAYER : MULTIPLAYER;
                    currentState = PLAYING;  // Simplified
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = MENU;
        } else if (currentState == CONNECTING) {
            // Cannot connect to CS 1.6
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
            if (IsKeyPressed(KEY_ONE) && player.money >= 2700) { player.money -= 2700; weapon = Weapon(AK47, resPath); }
            if (IsKeyPressed(KEY_TWO) && player.money >= 4750) { player.money -= 4750; weapon = Weapon(AWP, resPath); }
            if (IsKeyPressed(KEY_THREE) && player.money >= 700) { player.money -= 700; weapon = Weapon(DEAGLE, resPath); }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = PLAYING;
        }

        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            UIManager::DrawMenu(screenWidth, screenHeight, assetsFound);
        } else if (currentState == MAP_SELECT) {
            ClearBackground(BLUE);
            UIManager::DrawMapSelect(screenWidth, screenHeight, maps);
        } else if (currentState == CONNECTING) {
            ClearBackground(BLACK);
            DrawText("Cannot connect to CS 1.6 servers (different engine)", screenWidth/2 - 200, screenHeight/2, 20, RED);
            DrawText("Press ESC to return", screenWidth/2 - 100, screenHeight/2 + 30, 20, WHITE);
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
            weapon.Draw(sinf(player.bobbingTime), player.isCrouching);
            UIManager::DrawHUD(player.health, 30, player.money);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
        }
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}