#ifndef UI_H
#define UI_H

class UI {
public:
    static void DrawMenu(int w, int h);
    static void DrawBuyMenu(int w, int h, int money);
    static void DrawPause(int w, int h);
    static void DrawHUD(int health, int ammo);
    static void DrawCrosshair(int w, int h);
};

#endif
