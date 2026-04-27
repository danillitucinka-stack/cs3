#include "GameEngine.h"
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
