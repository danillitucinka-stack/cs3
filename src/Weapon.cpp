#include "Weapon.h"
#include "raylib.h"

Weapon::Weapon(WeaponType type) : sway(0.0f), recoil(0.0f), weaponType(type), fireRate(0.0f) {
    position = {1.0f, -0.5f, 1.0f};
    if (type == AK47) {
        model = LoadModelFromMesh(GenMeshCube(0.3f, 0.1f, 1.0f));  // Brown for AK
    } else if (type == AWP) {
        model = LoadModelFromMesh(GenMeshCube(0.4f, 0.1f, 1.5f));  // Blue for AWP
    } else {
        model = LoadModelFromMesh(GenMeshCube(0.2f, 0.1f, 0.5f));  // Gray for Knife
    }
}

void Weapon::Update(bool moving) {
    if (moving) sway += 0.1f;
    else sway *= 0.9f;
    if (recoil > 0) recoil *= 0.8f;
    if (weaponType == AK47 && fireRate > 0) fireRate -= GetFrameTime();
}

void Weapon::Draw() {
    Vector3 drawPos = {position.x + sinf(sway) * 0.1f - recoil, position.y + cosf(sway) * 0.1f, position.z};
    Color col = (weaponType == AK47) ? BROWN : (weaponType == AWP) ? BLUE : GRAY;
    DrawModel(model, drawPos, 1.0f, col);
}

void Weapon::Shoot() {
    if (weaponType == AK47) {
        if (fireRate <= 0) {
            recoil = 0.1f;
            fireRate = 0.1f;  // Burst rate
        }
    } else if (weaponType == AWP) {
        recoil = 0.5f;  // Strong recoil, slow
    } else {
        recoil = 0.05f;  // Knife
    }
}