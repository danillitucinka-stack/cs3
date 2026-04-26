#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <string>

// Game states
enum GameState {
    MENU,
    BATTLE,
    PAUSE
};

// Player structure
struct Player {
    Vector3 position;
    float speed;
    float mouseSensitivity;
    int health;
    int ammo;
    Camera camera;
};

// Bot structure
struct Bot {
    Vector3 position;
    int health;
    std::string action;
    Model model;
};

#endif