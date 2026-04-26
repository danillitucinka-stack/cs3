#include "UI.h"
#include "raylib.h"

void UI::DrawMenu(int w, int h) {
    DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
    DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
}

void UI::DrawBuyMenu(int w, int h, int money) {
    DrawText("Buy Menu", w/2 - 50, h/2 - 100, 30, BLACK);
    DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 70, 20, GREEN);
    DrawText("1. AK-47 $2700", w/2 - 100, h/2 - 30, 20, BLACK);
    DrawText("2. AWP $4750", w/2 - 100, h/2, 20, BLACK);
    DrawText("3. Knife $0", w/2 - 100, h/2 + 30, 20, BLACK);
    DrawText("Press ESC to exit", w/2 - 100, h/2 + 60, 20, GRAY);
}

void UI::DrawPause(int w, int h) {
    DrawText("PAUSED", w/2 - 50, h/2, 40, WHITE);
}

void UI::DrawHUD(int health, int ammo) {
    DrawText(TextFormat("Health: %d Ammo: %d", health, ammo), 10, 10, 20, WHITE);
}

void UI::DrawCrosshair(int w, int h) {
    int cx = w/2, cy = h/2;
    DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
    DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
}