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
    float fireRate;  // For AK burst

public:
    Weapon(WeaponType type = AK47);
    void Update(bool moving);
    void Draw();
    void Shoot();
    WeaponType GetType() { return weaponType; }
};

#endif