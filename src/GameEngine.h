#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include "raylib.h"
#include <string>

enum GameState { MENU, BUY_MENU, GAMEPLAY, PAUSE };

class GameEngine {
private:
    GameState currentState;
    int screenWidth, screenHeight;
    int money;

public:
    GameEngine(int width, int height);
    void Initialize();
    void Update();
    void Draw();
    void Cleanup();
    GameState GetState() { return currentState; }
    int GetMoney() { return money; }
};

#endif
