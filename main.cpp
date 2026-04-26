#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

enum GameState { MENU, PLAYING, BUYING };
enum WeaponType { AK47, AWP, DEAGLE };

class Player {
public:
    Vector3 position;
    Vector3 velocity;
    Camera camera;
    float speed, gravity, jumpForce, crouchY, bobbingTime;
    bool isCrouching, isGrounded;
    int health, money;
    Sound footstepSound;
    Player() : speed(5.0f), gravity(-9.8f), jumpForce(8.0f), crouchY(0.0f), bobbingTime(0.0f), isCrouching(false), isGrounded(true), health(100), money(800), velocity({0,0,0}) {
        position = {0.0f, 2.0f, 4.0f};
        camera.position = position;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        footstepSound = LoadSound("resources/sounds/footsteps.wav");
    }
    void Update(bool& playFootstep) {
        Vector3 targetVel = {0, velocity.y, 0};
        if (IsKeyDown(KEY_W)) targetVel.z -= speed;
        if (IsKeyDown(KEY_S)) targetVel.z += speed;
        if (IsKeyDown(KEY_A)) targetVel.x -= speed;
        if (IsKeyDown(KEY_D)) targetVel.x += speed;
        velocity.x = Vector3Lerp(velocity, targetVel, GetFrameTime() * 5.0f).x;
        velocity.z = Vector3Lerp(velocity, targetVel, GetFrameTime() * 5.0f).z;

        // Crouch
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            isCrouching = true;
            crouchY = -1.0f;
        } else {
            isCrouching = false;
            crouchY = 0.0f;
        }

        // Jump and Air Strafing
        if (isGrounded && IsKeyPressed(KEY_SPACE)) {
            velocity.y = jumpForce;
            isGrounded = false;
        }
        if (!isGrounded) {
            // Air strafing: allow full control
            velocity.x = targetVel.x;
            velocity.z = targetVel.z;
        }
        velocity.y += gravity * GetFrameTime();
        position.y += velocity.y * GetFrameTime();
        if (position.y < 0.0f) {
            position.y = 0.0f;
            velocity.y = 0;
            isGrounded = true;
        }

        position.x += velocity.x * GetFrameTime();
        position.z += velocity.z * GetFrameTime();

        camera.position = position;
        camera.position.y += crouchY;

        // Bobbing
        if (fabs(velocity.x) > 0.1f || fabs(velocity.z) > 0.1f) {
            bobbingTime += GetFrameTime() * 10.0f;
            camera.position.y += sinf(bobbingTime) * 0.05f;
            if (fmod(bobbingTime, 1.0f) < 0.1f) playFootstep = true;
        } else {
            bobbingTime = 0.0f;
        }
    }
    void LookAround() {
        Vector2 mouseDelta = GetMouseDelta();
        camera.target.x += mouseDelta.x * 0.005f;
        camera.target.y += mouseDelta.y * 0.005f;
    }
    void TakeRecoil(float amount) {
        camera.target.y -= amount;
    }
    ~Player() {
        UnloadSound(footstepSound);
    }
};

class Weapon {
public:
    Model model;
    WeaponType type;
    float sway, recoil, fireRate, zoomFOV;
    bool isZoomed;
    Sound shootSound;
    Weapon(WeaponType t = AK47) : type(t), sway(0), recoil(0), fireRate(0), zoomFOV(60.0f), isZoomed(false) {
        if (t == AWP) zoomFOV = 20.0f;
        std::string modelPath = "resources/models/" + std::string(t == AK47 ? "ak47" : t == AWP ? "awp" : "deagle") + ".obj";
        if (FileExists(modelPath.c_str())) {
            model = LoadModel(modelPath.c_str());
        } else {
            model = LoadModelFromMesh(GenMeshCube(0.5f, 0.2f, 1.0f));  // Fallback cube
        }
        shootSound = LoadSound("resources/sounds/shot.wav");
    }
    void Update(bool moving, Player& player) {
        if (moving) sway += 0.2f;
        else sway *= 0.9f;
        if (recoil > 0) recoil *= 0.85f;
        if (type == AK47 && fireRate > 0) fireRate -= GetFrameTime();
        if (type == AWP && IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            isZoomed = true;
            player.camera.fovy = zoomFOV;
        } else {
            isZoomed = false;
            player.camera.fovy = 60.0f;
        }
    }
    void Draw(float bobbing, bool crouching) {
        float tilt = -0.1f;
        float crouchOffset = crouching ? -0.5f : 0.0f;
        Vector3 pos = {1.0f + sinf(sway) * 0.1f - recoil, -0.5f + cosf(sway) * 0.1f + tilt + bobbing * 0.1f + crouchOffset, 1.0f};
        DrawModel(model, pos, 1.0f, WHITE);
        // Muzzle flash
        if (fireRate > 0 && fireRate < 0.05f) {
            DrawSphere({pos.x + 0.8f, pos.y, pos.z}, 0.1f, YELLOW);
        }
    }
    void Shoot(Player& player) {
        PlaySound(shootSound);
        if (type == AK47 && fireRate <= 0) {
            recoil = 0.15f;
            fireRate = 0.1f;
            player.TakeRecoil(0.1f);
        } else if (type == AWP) {
            recoil = 0.6f;
            player.TakeRecoil(0.2f);
        } else {
            recoil = 0.3f;
            player.TakeRecoil(0.15f);
        }
    }
    ~Weapon() {
        UnloadModel(model);
        UnloadSound(shootSound);
    }
};

class MapManager {
public:
    Model mapModel;
    MapManager() {
        if (FileExists("resources/models/dust2.obj")) {
            mapModel = LoadModel("resources/models/dust2.obj");
        } else {
            mapModel = LoadModelFromMesh(GenMeshCube(10.0f, 1.0f, 10.0f));  // Fallback
        }
    }
    void Draw() {
        DrawModel(mapModel, {0.0f, 0.0f, 0.0f}, 1.0f, BEIGE);
    }
    ~MapManager() {
        UnloadModel(mapModel);
    }
};

class UIManager {
public:
    static void DrawMenu(int w, int h) {
        DrawText("CS 3 AI", w/2 - 50, h/2 - 50, 40, BLACK);
        DrawText("Press ENTER to Play", w/2 - 100, h/2, 20, GRAY);
    }
    static void DrawBuyMenu(int w, int h, int money) {
        DrawRectangle(w/2 - 200, h/2 - 150, 400, 300, Fade(BLACK, 0.8f));
        DrawText("Buy Menu", w/2 - 50, h/2 - 120, 30, WHITE);
        DrawText(TextFormat("Money: $%d", money), w/2 - 50, h/2 - 90, 20, GREEN);
        DrawRectangle(w/2 - 150, h/2 - 50, 300, 40, GRAY);
        DrawText("1. AK-47 $2700", w/2 - 140, h/2 - 40, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2, 300, 40, GRAY);
        DrawText("2. AWP $4750", w/2 - 140, h/2 + 10, 20, BLACK);
        DrawRectangle(w/2 - 150, h/2 + 50, 300, 40, GRAY);
        DrawText("3. Deagle $700", w/2 - 140, h/2 + 60, 20, BLACK);
        DrawText("Press ESC to exit", w/2 - 100, h/2 + 120, 20, WHITE);
    }
    static void DrawHUD(int health, int ammo, int money) {
        DrawText(TextFormat("Health: %d", health), 10, 10, 20, ORANGE);
        DrawText(TextFormat("Ammo: %d", ammo), 10, 35, 20, YELLOW);
        DrawText(TextFormat("Money: $%d", money), 10, 60, 20, GREEN);
    }
    static void DrawCrosshair(int w, int h) {
        int cx = w/2, cy = h/2;
        DrawLine(cx - 10, cy, cx + 10, cy, WHITE);
        DrawLine(cx, cy - 10, cx, cy + 10, WHITE);
    }
};

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    GameState currentState = MENU;
    Player player;
    Weapon weapon(AK47);
    MapManager mapManager;

    InitWindow(screenWidth, screenHeight, "CS 3 AI");
    InitAudioDevice();
    SetTargetFPS(60);
    DisableCursor();

    bool playFootstep = false;

    while (!WindowShouldClose()) {
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) currentState = PLAYING;
        } else if (currentState == PLAYING) {
            player.LookAround();
            player.Update(playFootstep);
            if (playFootstep) {
                PlaySound(player.footstepSound);
                playFootstep = false;
            }
            bool moving = IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D);
            weapon.Update(moving, player);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) weapon.Shoot(player);
            if (IsKeyPressed(KEY_B)) currentState = BUYING;
        } else if (currentState == BUYING) {
            if (IsKeyPressed(KEY_ONE) && player.money >= 2700) { player.money -= 2700; weapon = Weapon(AK47); }
            if (IsKeyPressed(KEY_TWO) && player.money >= 4750) { player.money -= 4750; weapon = Weapon(AWP); }
            if (IsKeyPressed(KEY_THREE) && player.money >= 700) { player.money -= 700; weapon = Weapon(DEAGLE); }
            if (IsKeyPressed(KEY_ESCAPE)) currentState = PLAYING;
        }

        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            UIManager::DrawMenu(screenWidth, screenHeight);
        } else if (currentState == BUYING) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.Draw();
            EndMode3D();
            UIManager::DrawBuyMenu(screenWidth, screenHeight, player.money);
        } else if (currentState == PLAYING) {
            ClearBackground(DARKBLUE);
            BeginMode3D(player.camera);
            mapManager.Draw();
            EndMode3D();
            weapon.Draw(sinf(player.bobbingTime), player.isCrouching);
            UIManager::DrawHUD(player.health, 30, player.money);
            UIManager::DrawCrosshair(screenWidth, screenHeight);
        }
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}