#include "GameLoop.h"
#include "raylib.h"
#include "raymath.h"
#include "nlohmann/json.hpp"
#include "MapManager.h"
#include "UIManager.h"
#include <fstream>
#include <string>

GameLoop::GameLoop() : currentState(STATE_MENU), playerPosition{0.0f, 2.0f, 4.0f}, playerSpeed(5.0f), mouseSensitivity(0.003f) {
    camera.position = playerPosition;
    camera.target = (Vector3){0.0f, 2.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void GameLoop::Initialize(int screenWidth, int screenHeight) {
    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);

    wallTexture = LoadTexture("assets/textures/wall_texture.png");
    botTexture = LoadTexture("assets/textures/bot_texture.png");

    ground = LoadModelFromMesh(GenMeshPlane(20.0f, 20.0f, 1, 1));

    mapData = MapManager::LoadMap("map.json");
    walls = MapManager::GenerateWalls(mapData, wallTexture);

    bot = LoadModelFromMesh(GenMeshCube(1.0f, 2.0f, 1.0f));
    bot.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = botTexture;
}

void GameLoop::Update() {
    if (currentState == STATE_GAME) {
        camera.position = playerPosition;

        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x += mouseDelta.x * mouseSensitivity;
        camera.target.y += mouseDelta.y * mouseSensitivity;

        if (IsKeyDown(KEY_W)) playerPosition.z -= playerSpeed * GetFrameTime();
        if (IsKeyDown(KEY_S)) playerPosition.z += playerSpeed * GetFrameTime();
        if (IsKeyDown(KEY_A)) playerPosition.x -= playerSpeed * GetFrameTime();
        if (IsKeyDown(KEY_D)) playerPosition.x += playerSpeed * GetFrameTime();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetMouseRay(GetMousePosition(), camera);
            TraceLog(LOG_INFO, "Shot fired");
        }

        // Read bot status
        std::ifstream file("bot_status.json");
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            auto json = nlohmann::json::parse(content);
            if (json.contains("position")) {
                botPosition.x = json["position"]["x"];
                botPosition.y = json["position"]["y"];
                botPosition.z = json["position"]["z"];
            }
        }
    } else if (currentState == STATE_MENU) {
        if (IsKeyPressed(KEY_ENTER)) {
            currentState = STATE_GAME;
            DisableCursor();
        }
    }
}

void GameLoop::Draw(int screenWidth, int screenHeight) {
    BeginDrawing();
    if (currentState == STATE_MENU) {
        ClearBackground(WHITE);
        UIManager::DrawMenu(screenWidth, screenHeight);
    } else if (currentState == STATE_GAME) {
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);
        DrawModel(ground, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, GRAY);
        MapManager::DrawWalls(walls, mapData);
        DrawModel(bot, botPosition, 1.0f, WHITE);
        EndMode3D();
        UIManager::DrawHUD(100, 30);  // Example values
        UIManager::DrawCrosshair(screenWidth, screenHeight);
        DrawFPS(10, 10);
    }
    EndDrawing();
}

void GameLoop::Cleanup() {
    UnloadTexture(wallTexture);
    UnloadTexture(botTexture);
    UnloadModel(ground);
    for (auto& wall : walls) UnloadModel(wall);
    UnloadModel(bot);
    CloseWindow();
}