#include "engine.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    GameState currentState = MENU;
    int money = 800;
    int health = 100, ammo = 30;
    Player player;
    Weapon weapon(AK47);
    MapManager mapManager;
    mapManager.GenerateDust2();

    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (currentState == MENU && IsKeyPressed(KEY_ENTER)) currentState = GAMEPLAY;
        if (currentState == GAMEPLAY && IsKeyPressed(KEY_B)) currentState = BUY_MENU;
        if (currentState == BUY_MENU) {
            if (IsKeyPressed(KEY_ONE) && money >= 2700) { money -= 2700; weapon = Weapon(AK47); }
            if (IsKeyPressed(KEY_TWO) && money >= 4750) { money -= 4750; weapon = Weapon(AWP); }
            if (IsKeyPressed(KEY_THREE)) { weapon = Weapon(KNIFE); }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = GAMEPLAY;
        }
        if (IsKeyPressed(KEY_P)) currentState = (currentState == PAUSE) ? GAMEPLAY : PAUSE;

        if (currentState == GAMEPLAY) {
            player.Update();
            bool moving = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
            weapon.Update(moving);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) weapon.Shoot();
        }

        BeginDrawing();
        if (currentState == MENU) {
            UIManager::DrawMenu(screenWidth, screenHeight);
        } else if (currentState == BUY_MENU) {
            UIManager::DrawBuyMenu(screenWidth, screenHeight, money);
        } else if (currentState == GAMEPLAY) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.GetCamera());
            mapManager.DrawWalls();
            EndMode3D();
            weapon.Draw();
            UIManager::DrawHUD(health, ammo);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
        } else if (currentState == PAUSE) {
            UIManager::DrawPause(screenWidth, screenHeight);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}