#ifndef UIMANAGER_H
#define UIMANAGER_H

class UIManager {
public:
    static void DrawMenu(int screenWidth, int screenHeight);
    static void DrawHUD(int health, int ammo);
    static void DrawCrosshair(int screenWidth, int screenHeight);
};

#endif