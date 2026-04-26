#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "raylib.h"
#include "game.h"
#include <vector>
#include <map>
#include <filesystem>

class GameLoop {
private:
    GameState currentState;
    Player player;
    Bot botData;
    std::map<std::string, Texture2D> textures;
    Model ground, bot;
    std::vector<Model> walls;
    std::vector<std::vector<int>> mapData;

    void LoadTextures();
    void LoadLevel();

public:
    GameLoop();
    void Initialize(int screenWidth, int screenHeight);
    void Update();
    void Draw(int screenWidth, int screenHeight);
    void Cleanup();
};

#endif