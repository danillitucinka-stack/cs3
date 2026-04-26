#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include "raylib.h"

class Level {
public:
    std::string name;
    int enemyCount;
    std::vector<Model> walls;
    void Load(std::string filename);
    void Draw();
};

#endif