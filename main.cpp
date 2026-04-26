#include "GameLoop.h"

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