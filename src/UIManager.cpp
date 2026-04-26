#include "UIManager.h"
#include "raylib.h"

void UIManager::DrawMenu(int screenWidth, int screenHeight) {
    DrawText("CS 3 AI", screenWidth / 2 - MeasureText("CS 3 AI", 40) / 2, screenHeight / 2 - 50, 40, BLACK);
    DrawText("Press ENTER to START", screenWidth / 2 - MeasureText("Press ENTER to START", 20) / 2, screenHeight / 2, 20, GRAY);
}

void UIManager::DrawHUD(int health, int ammo) {
    // Health bar
    DrawRectangle(10, 10, 200, 20, RED);
    DrawRectangle(10, 10, (health * 200) / 100, 20, GREEN);
    DrawText(TextFormat("Health: %d", health), 10, 35, 20, BLACK);

    // Ammo
    DrawText(TextFormat("Ammo: %d", ammo), 10, 60, 20, BLACK);
}

void UIManager::DrawCrosshair(int screenWidth, int screenHeight) {
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;
    DrawLine(centerX - 10, centerY, centerX + 10, centerY, WHITE);
    DrawLine(centerX, centerY - 10, centerX, centerY + 10, WHITE);
}