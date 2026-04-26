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
    float weaponSway;  // For weapon sway
    float weaponRecoil;  // For recoil
    int recoilFrames;

public:
    GameLoop() : currentState(MENU), playerSpeed(5.0f), mouseSensitivity(0.003f), flashFrames(0), weaponSway(0.0f), weaponRecoil(0.0f), recoilFrames(0) {
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
            bool moving = false;
            if (IsKeyDown(KEY_W)) { playerPosition.z -= playerSpeed * GetFrameTime(); moving = true; }
            if (IsKeyDown(KEY_S)) { playerPosition.z += playerSpeed * GetFrameTime(); moving = true; }
            if (IsKeyDown(KEY_A)) { playerPosition.x -= playerSpeed * GetFrameTime(); moving = true; }
            if (IsKeyDown(KEY_D)) { playerPosition.x += playerSpeed * GetFrameTime(); moving = true; }

            // Weapon sway
            if (moving) {
                weaponSway += 0.1f;
            } else {
                weaponSway *= 0.9f;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                TraceLog(LOG_INFO, "Shot fired");
                flashFrames = 10;  // Flash for 10 frames
                weaponRecoil = 20.0f;  // Recoil amount
                recoilFrames = 10;
            }
            if (flashFrames > 0) flashFrames--;
            if (recoilFrames > 0) {
                recoilFrames--;
                weaponRecoil *= 0.8f;
            } else {
                weaponRecoil = 0.0f;
            }

            // Enemy faces player
            Vector3 direction = Vector3Subtract(playerPosition, enemyPosition);
            float angle = atan2f(direction.x, direction.z);
            // For simplicity, we'll skip rotation for now, as DrawModel doesn't rotate easily
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
            DrawModel(ground, {0.0f, 0.0f, 0.0f}, 1.0f, DARKGREEN);  // Dark green floor
            // Draw grid on floor
            for (int i = -50; i <= 50; i += 10) {
                DrawLine3D({(float)i, 0.01f, -50.0f}, {(float)i, 0.01f, 50.0f}, GRAY);
                DrawLine3D({-50.0f, 0.01f, (float)i}, {50.0f, 0.01f, (float)i}, GRAY);
            }
            DrawModel(enemy, enemyPosition, 1.0f, RED);
            // Simple face on enemy
            DrawCube({enemyPosition.x, enemyPosition.y + 0.5f, enemyPosition.z + 0.3f}, 0.1f, 0.1f, 0.1f, BLACK);  // Eyes
            DrawCube({enemyPosition.x, enemyPosition.y + 0.3f, enemyPosition.z + 0.3f}, 0.05f, 0.05f, 0.05f, BLACK);  // Mouth
            if (flashFrames > 0) {
                // Shooting flash: yellow point near weapon
                DrawSphere({camera.position.x + 1.0f, camera.position.y - 0.5f, camera.position.z + 1.0f}, 0.1f, YELLOW);
            }
            EndMode3D();
            // Crosshair
            int centerX = screenWidth / 2;
            int centerY = screenHeight / 2;
            DrawLine(centerX - 10, centerY, centerX + 10, centerY, WHITE);
            DrawLine(centerX, centerY - 10, centerX, centerY + 10, WHITE);
            // Weapon: simple rectangle with sway and recoil
            float weaponX = screenWidth - 100 + sinf(weaponSway) * 5.0f - weaponRecoil;
            float weaponY = screenHeight - 100 + cosf(weaponSway) * 5.0f;
            DrawRectangle(weaponX, weaponY, 80, 40, BROWN);  // Simple gun shape
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