#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h"

enum WeaponType { AK47, AWP, KNIFE };

class Weapon {
private:
    Model model;
    Vector3 position;
    float sway, recoil;
    WeaponType weaponType;

public:
    Weapon(WeaponType type = AK47);
    void LoadModel(const char* path);
    void Update(bool moving);
    void Draw();
    void Shoot();
    WeaponType GetType() { return weaponType; }
};

#endif
