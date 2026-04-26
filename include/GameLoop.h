#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "raylib.h"
#include <vector>

enum GameState { STATE_MENU, STATE_GAME };

class GameLoop {
private:
    GameState currentState;
    Camera camera;
    Vector3 playerPosition;
    float playerSpeed;
    float mouseSensitivity;
    Texture2D wallTexture, botTexture;
    Model ground, bot;
    std::vector<Model> walls;
    std::vector<std::vector<int>> mapData;
    Vector3 botPosition = {0.0f, 1.0f, -5.0f};

public:
    GameLoop();
    void Initialize(int screenWidth, int screenHeight);
    void Update();
    void Draw(int screenWidth, int screenHeight);
    void Cleanup();
};

#endif