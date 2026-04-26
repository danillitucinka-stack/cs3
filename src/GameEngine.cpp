#include "GameEngine.h"
#include "UI.h"
#include "Player.h"
#include "MapManager.h"
#include "Weapon.h"

GameEngine::GameEngine(int width, int height) : currentState(MENU), screenWidth(width), screenHeight(height), money(800), health(100), ammo(30) {}

void GameEngine::Initialize() {
    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);
    // Initialize components
}

void GameEngine::Update() {
    if (currentState == MENU && IsKeyPressed(KEY_ENTER)) currentState = GAMEPLAY;
    if (currentState == GAMEPLAY && IsKeyPressed(KEY_B)) currentState = BUY_MENU;
    if (currentState == BUY_MENU) {
        if (IsKeyPressed(KEY_ONE) && money >= 2700) { money -= 2700; /* set weapon to AK */ }
        if (IsKeyPressed(KEY_TWO) && money >= 4750) { money -= 4750; /* set weapon to AWP */ }
        if (IsKeyPressed(KEY_THREE)) { /* set weapon to Knife */ }
        if (IsKeyPressed(KEY_ESCAPE)) currentState = GAMEPLAY;
    }
    if (IsKeyPressed(KEY_P)) currentState = (currentState == PAUSE) ? GAMEPLAY : PAUSE;
    // Update other components
}

void GameEngine::Draw() {
    BeginDrawing();
    if (currentState == MENU) {
        UI::DrawMenu(screenWidth, screenHeight);
    } else if (currentState == BUY_MENU) {
        UI::DrawBuyMenu(screenWidth, screenHeight, money);
    } else if (currentState == GAMEPLAY) {
        ClearBackground(DARKBLUE);
        // Draw game world
        UI::DrawHUD(health, ammo);
        UI::DrawCrosshair(screenWidth, screenHeight);
    } else if (currentState == PAUSE) {
        UI::DrawPause(screenWidth, screenHeight);
    }
    EndDrawing();
}

void GameEngine::Cleanup() {
    CloseWindow();
}