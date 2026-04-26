#include "GameEngine.h"

int main() {
    GameEngine engine(1280, 720);
    engine.Initialize();
    while (!WindowShouldClose()) {
        engine.Update();
        engine.Draw();
    }
    engine.Cleanup();
    return 0;
}