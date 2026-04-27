#include "Player.h"

Player::Player() : speed(5.0f) {
    position = {0.0f, 2.0f, 4.0f};
    camera.position = position;
    camera.target = {0.0f, 2.0f, 0.0f};
    camera.up = {0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void Player::Update() {
    camera.position = position;
    if (IsKeyDown(KEY_W)) position.z -= speed * GetFrameTime();
    if (IsKeyDown(KEY_S)) position.z += speed * GetFrameTime();
    if (IsKeyDown(KEY_A)) position.x -= speed * GetFrameTime();
    if (IsKeyDown(KEY_D)) position.x += speed * GetFrameTime();
}

void Player::Draw() {
    // Draw if needed
}
