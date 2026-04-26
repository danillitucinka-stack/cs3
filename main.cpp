#include "raylib.h"
#include "raymath.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>

enum GameState { MENU, BATTLE, PAUSE };

class GameLoop {
private:
    GameState currentState;
    Camera camera;
    Vector3 playerPosition;
    float playerSpeed;
    float mouseSensitivity;
    Model ground;
    Model enemy;
    Vector3 enemyPosition;
    int flashFrames;  // For shooting flash

public:
    GameLoop() : currentState(MENU), playerSpeed(5.0f), mouseSensitivity(0.003f), flashFrames(0) {
        playerPosition = {0.0f, 2.0f, 4.0f};
        camera.position = playerPosition;
        camera.target = {0.0f, 2.0f, 0.0f};
        camera.up = {0.0f, 1.0f, 0.0f};
        camera.fovy = 60.0f;
        camera.projection = CAMERA_PERSPECTIVE;
        enemyPosition = {0.0f, 1.0f, -5.0f};
    }

    void Initialize(int screenWidth, int screenHeight) {
        InitWindow(screenWidth, screenHeight, "CS 3 AI");
        SetTargetFPS(60);
        ground = LoadModelFromMesh(GenMeshPlane(100.0f, 100.0f, 10, 10));  // Larger grid plane
        enemy = LoadModelFromMesh(GenMeshCylinder(0.5f, 2.0f, 16));  // Cylinder for enemy
    }

    void Update() {
        if (currentState == BATTLE) {
            camera.position = playerPosition;
            Vector2 mouseDelta = GetMouseDelta();
            camera.target.x += mouseDelta.x * mouseSensitivity;
            camera.target.y += mouseDelta.y * mouseSensitivity;
            if (IsKeyDown(KEY_W)) playerPosition.z -= playerSpeed * GetFrameTime();
            if (IsKeyDown(KEY_S)) playerPosition.z += playerSpeed * GetFrameTime();
            if (IsKeyDown(KEY_A)) playerPosition.x -= playerSpeed * GetFrameTime();
            if (IsKeyDown(KEY_D)) playerPosition.x += playerSpeed * GetFrameTime();
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                TraceLog(LOG_INFO, "Shot fired");
                flashFrames = 30;  // Flash for 30 frames
            }
            if (flashFrames > 0) flashFrames--;
            // Read bot_status.json
            std::ifstream file("bot_status.json");
            if (file.is_open()) {
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                auto json = nlohmann::json::parse(content);
                if (json.contains("position")) {
                    enemyPosition.x = json["position"]["x"];
                    enemyPosition.y = json["position"]["y"];
                    enemyPosition.z = json["position"]["z"];
                }
            }
        } else if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = BATTLE;
                DisableCursor();
            }
        } else if (currentState == PAUSE) {
            if (IsKeyPressed(KEY_P)) currentState = BATTLE;
            if (IsKeyPressed(KEY_ESCAPE)) currentState = MENU;
        }
        if (IsKeyPressed(KEY_P) && currentState == BATTLE) currentState = PAUSE;
    }

    void Draw(int screenWidth, int screenHeight) {
        BeginDrawing();
        if (currentState == MENU) {
            ClearBackground(WHITE);
            DrawText("CS 3 AI", screenWidth / 2 - MeasureText("CS 3 AI", 40) / 2, screenHeight / 2 - 50, 40, BLACK);
            DrawText("Press ENTER to START", screenWidth / 2 - MeasureText("Press ENTER to START", 20) / 2, screenHeight / 2, 20, GRAY);
        } else if (currentState == BATTLE) {
            ClearBackground(DARKBLUE);  // Sky background
            BeginMode3D(camera);
            DrawModel(ground, {0.0f, 0.0f, 0.0f}, 1.0f, GRAY);
            DrawModel(enemy, enemyPosition, 1.0f, RED);
            // Simple face on enemy
            DrawCube({enemyPosition.x, enemyPosition.y + 0.5f, enemyPosition.z + 0.3f}, 0.1f, 0.1f, 0.1f, BLACK);  // Eyes
            DrawCube({enemyPosition.x, enemyPosition.y + 0.3f, enemyPosition.z + 0.3f}, 0.05f, 0.05f, 0.05f, BLACK);  // Mouth
            if (flashFrames > 0) {
                // Shooting flash: yellow sphere at look direction
                Vector3 flashPos = Vector3Add(camera.position, Vector3Scale(Vector3Normalize(Vector3Subtract(camera.target, camera.position)), 5.0f));
                DrawSphere(flashPos, 0.2f, YELLOW);
            }
            EndMode3D();
            // Crosshair
            int centerX = screenWidth / 2;
            int centerY = screenHeight / 2;
            DrawLine(centerX - 10, centerY, centerX + 10, centerY, WHITE);
            DrawLine(centerX, centerY - 10, centerX, centerY + 10, WHITE);
            DrawText("WASD to move, Mouse to look, Left click to shoot", 10, 10, 20, WHITE);
            DrawFPS(10, 30);
        } else if (currentState == PAUSE) {
            ClearBackground(BLACK);
            DrawText("PAUSED", screenWidth / 2 - 50, screenHeight / 2, 40, WHITE);
            DrawText("Press P to resume, ESC to menu", screenWidth / 2 - 150, screenHeight / 2 + 50, 20, WHITE);
        }
        EndDrawing();
    }

    void Cleanup() {
        UnloadModel(ground);
        UnloadModel(enemy);
        CloseWindow();
    }
};

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    GameLoop gameLoop;
    gameLoop.Initialize(screenWidth, screenHeight);
    while (!WindowShouldClose()) {
        gameLoop.Update();
        gameLoop.Draw(screenWidth, screenHeight);
    }
    gameLoop.Cleanup();
    return 0;
}