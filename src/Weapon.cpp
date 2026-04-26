#include "Weapon.h"
#include "raylib.h"

enum WeaponType { AK47, AWP, KNIFE };

Weapon::Weapon(WeaponType type) : sway(0.0f), recoil(0.0f), weaponType(type) {
    position = {1.0f, -0.5f, 1.0f};  // Relative to camera
    // Create from cubes since no .obj
    if (type == AK47) {
        model = LoadModelFromMesh(GenMeshCube(0.3f, 0.1f, 1.0f));  // Barrel
        // Add handle and mag as separate, but for simplicity
    } else if (type == AWP) {
        model = LoadModelFromMesh(GenMeshCube(0.4f, 0.1f, 1.5f));  // Longer barrel
    } else {
        model = LoadModelFromMesh(GenMeshCube(0.2f, 0.1f, 0.5f));  // Knife
    }
}

void Weapon::LoadModel(const char* path) {
    model = LoadModel(path);
}

void Weapon::Update(bool moving) {
    if (moving) {
        sway += 0.1f;
    } else {
        sway *= 0.9f;
    }
    if (recoil > 0) recoil *= 0.8f;
}

void Weapon::Draw() {
    Vector3 drawPos = {position.x + sinf(sway) * 0.1f - recoil, position.y + cosf(sway) * 0.1f, position.z};
    DrawModel(model, drawPos, 1.0f, weaponType == KNIFE ? GRAY : BROWN);
}

void Weapon::Shoot() {
    recoil = 0.3f;
}