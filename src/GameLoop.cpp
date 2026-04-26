#include "GameLoop.h"
#include "raylib.h"
#include "raymath.h"
#include "nlohmann/json.hpp"
#include "MapManager.h"
#include "UIManager.h"
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

GameLoop::GameLoop() : currentState(MENU) {
    player.position = {0.0f, 2.0f, 4.0f};
    player.speed = 5.0f;
    player.mouseSensitivity = 0.003f;
    player.health = 100;
    player.ammo = 30;
    player.camera.position = player.position;
    player.camera.target = (Vector3){0.0f, 2.0f, 0.0f};
    player.camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    player.camera.fovy = 60.0f;
    player.camera.projection = CAMERA_PERSPECTIVE;

    botData.position = {0.0f, 1.0f, -5.0f};
    botData.health = 100;
    botData.action = "idle";
}

void GameLoop::LoadTextures() {
    for (const auto& entry : fs::directory_iterator("assets/textures/")) {
        if (entry.path().extension() == ".png") {
            std::string filename = entry.path().stem().string();
            textures[filename] = LoadTexture(entry.path().string().c_str());
        }
    }
}

void GameLoop::LoadLevel() {
    mapData = MapManager::LoadMap("map.json");
    if (!textures.count("wall_texture")) return;
    walls = MapManager::GenerateWalls(mapData, textures["wall_texture"]);
}

void GameLoop::Initialize(int screenWidth, int screenHeight) {
    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);

    LoadTextures();
    LoadLevel();

    ground = LoadModelFromMesh(GenMeshPlane(20.0f, 20.0f, 1, 1));

    if (textures.count("bot_texture")) {
        bot = LoadModelFromMesh(GenMeshCube(1.0f, 2.0f, 1.0f));
        bot.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = textures["bot_texture"];
        botData.model = bot;
    }
}

void GameLoop::Update() {
    if (currentState == BATTLE) {
        player.camera.position = player.position;

        Vector2 mouseDelta = GetMouseDelta();
        player.camera.target.x += mouseDelta.x * player.mouseSensitivity;
        player.camera.target.y += mouseDelta.y * player.mouseSensitivity;

        if (IsKeyDown(KEY_W)) player.position.z -= player.speed * GetFrameTime();
        if (IsKeyDown(KEY_S)) player.position.z += player.speed * GetFrameTime();
        if (IsKeyDown(KEY_A)) player.position.x -= player.speed * GetFrameTime();
        if (IsKeyDown(KEY_D)) player.position.x += player.speed * GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetMouseRay(GetMousePosition(), player.camera);
            TraceLog(LOG_INFO, "Shot fired");
            player.ammo--;
        }

        // Read bot status
        std::ifstream file("bot_status.json");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            auto json = nlohmann::json::parse(content);
            if (json.contains("position")) {
                botData.position.x = json["position"]["x"];
                botData.position.y = json["position"]["y"];
                botData.position.z = json["position"]["z"];
            }
            if (json.contains("action")) {
                botData.action = json["action"];
            }
        }

        // Pause
        if (IsKeyPressed(KEY_P)) currentState = PAUSE;
    } else if (currentState == MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = BATTLE;
            DisableCursor();
        }
    } else if (currentState == PAUSE) {
        if (IsKeyPressed(KEY_P)) currentState = BATTLE;
        if (IsKeyPressed(KEY_ESCAPE)) currentState = MENU;
    }
}

void GameLoop::Draw(int screenWidth, int screenHeight) {
    BeginDrawing();
    if (currentState == MENU) {
        ClearBackground(WHITE);
        UIManager::DrawMenu(screenWidth, screenHeight);
    } else if (currentState == BATTLE) {
        ClearBackground(RAYWHITE);
        BeginMode3D(player.camera);
        DrawModel(ground, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, GRAY);
        MapManager::DrawWalls(walls, mapData);
        DrawModel(botData.model, botData.position, 1.0f, WHITE);
        EndMode3D();
        UIManager::DrawHUD(player.health, player.ammo);
        UIManager::DrawCrosshair(screenWidth, screenHeight);
        DrawFPS(10, 10);
    } else if (currentState == PAUSE) {
        ClearBackground(BLACK);
        DrawText("PAUSED", screenWidth / 2 - 50, screenHeight / 2, 40, WHITE);
        DrawText("Press P to resume, ESC to menu", screenWidth / 2 - 150, screenHeight / 2 + 50, 20, WHITE);
    }
    EndDrawing();
}

void GameLoop::Cleanup() {
    for (auto& tex : textures) {
        UnloadTexture(tex.second);
    }
    UnloadModel(ground);
    for (auto& wall : walls) UnloadModel(wall);
    UnloadModel(botData.model);
    CloseWindow();
}