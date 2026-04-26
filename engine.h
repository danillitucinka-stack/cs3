#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>  // For sinf, cosf
#include <nlohmann/json.hpp>
#include <fstream>

enum GameState { MENU, BUY_MENU, GAMEPLAY, PAUSE };
enum WeaponType { AK47, AWP, KNIFE };

class Player {
private:
    Vector3 position;
    float speed;
    Camera camera;

public:
    Player() : speed(5.0f) {
        position = {0.0f, 2.0f, 4.0f};
        camera.position = position;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
    }
    void Update() {
        camera.position = position;
        if (IsKeyDown(KEY_W)) position.z -= speed * GetFrameTime();
        if (IsKeyDown(KEY_S)) position.z += speed * GetFrameTime();
        if (IsKeyDown(KEY_A)) position.x -= speed * GetFrameTime();
        if (IsKeyDown(KEY_D)) position.x += speed * GetFrameTime();
    }
    Vector3 GetPosition() { return position; }
    Camera GetCamera() { return camera; }
};

class Weapon {
private:
    Model model;
    Vector3 position;
    float sway, recoil;
    WeaponType weaponType;
    float fireRate;

public:
    Weapon(WeaponType type = AK47) : sway(0.0f), recoil(0.0f), weaponType(type), fireRate(0.0f) {
        position = {1.0f, -0.5f, 1.0f};
        if (type == AK47) {
            model = LoadModelFromMesh(GenMeshCube(0.3f, 0.1f, 1.0f));
        } else if (type == AWP) {
            model = LoadModelFromMesh(GenMeshCube(0.4f, 0.1f, 1.5f));
        } else {
            model = LoadModelFromMesh(GenMeshCube(0.2f, 0.1f, 0.5f));
        }
    }
    void Update(bool moving) {
        if (moving) sway += 0.1f;
        else sway *= 0.9f;
        if (recoil > 0) recoil *= 0.8f;
        if (weaponType == AK47 && fireRate > 0) fireRate -= GetFrameTime();
    }
    void Draw() {
        Vector3 drawPos = {position.x + sinf(sway) * 0.1f - recoil, position.y + cosf(sway) * 0.1f, position.z};
        Color col = (weaponType == AK47) ? BROWN : (weaponType == AWP) ? BLUE : GRAY;
        DrawModel(model, drawPos, 1.0f, col);
    }
    void Shoot() {
        if (weaponType == AK47) {
            if (fireRate <= 0) {
                recoil = 0.1f;
                fireRate = 0.1f;
            }
        } else if (weaponType == AWP) {
            recoil = 0.5f;
        } else {
            recoil = 0.05f;
        }
    }
    WeaponType GetType() { return weaponType; }
};

class MapManager {
private:
    std::vector<Model> walls;
    std::vector<Model> boxes;

public:
    void GenerateDust2() {
        int map[20][20] = {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
            {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
            {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        };
        Model wallModel = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
        Model boxModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (map[i][j] == 1) walls.push_back(wallModel);
                else if (map[i][j] == 2) boxes.push_back(boxModel);
            }
        }
    }
    void DrawWalls() {
        int wallIndex = 0, boxIndex = 0;
        int map[20][20] = { /* same */ };
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                if (map[i][j] == 1 && wallIndex < walls.size()) {
                    DrawModel(walls[wallIndex], {(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f}, 1.0f, BEIGE);
                    wallIndex++;
                } else if (map[i][j] == 2 && boxIndex < boxes.size()) {
                    DrawModel(boxes[boxIndex], {(float)j * 2.0f - 20.0f, 0.5f, (float)i * 2.0f - 20.0f}, 1.0f, BROWN);
                    boxIndex++;
                }
            }
        }
    }
};

class UIManager {
public:
    static void DrawMenu(int w, int h) {
        DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
        DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
    }
    static void DrawBuyMenu(int w, int h, int money) {
        DrawText("Buy Menu", w/2 - 50, h/2 - 100, 30, BLACK);
        DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 70, 20, GREEN);
        DrawText("1. AK-47 $2700", w/2 - 100, h/2 - 30, 20, BLACK);
        DrawText("2. AWP $4750", w/2 - 100, h/2, 20, BLACK);
        DrawText("3. Knife $0", w/2 - 100, h/2 + 30, 20, BLACK);
        DrawText("Press ESC to exit", w/2 - 100, h/2 + 60, 20, GRAY);
    }
    static void DrawPause(int w, int h) {
        DrawText("PAUSED", w/2 - 50, h/2, 40, WHITE);
    }
    static void DrawHUD(int health, int ammo) {
        DrawText(TextFormat("Health: %d Ammo: %d", health, ammo), 10, 10, 20, WHITE);
    }
    static void DrawCrosshair(int w, int h) {
        int cx = w/2, cy = h/2;
        DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
        DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
    }
};

#endif