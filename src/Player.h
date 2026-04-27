#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

class Player {
private:
    Vector3 position;
    float speed;
    Camera camera;

public:
    Player();
    void Update();
    void Draw();
    Vector3 GetPosition() { return position; }
    Camera GetCamera() { return camera; }
};

#endif
