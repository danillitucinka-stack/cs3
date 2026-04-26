#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "raylib.h"
#include <vector>

class MapManager {
private:
    std::vector<Model> walls;
    std::vector<Model> boxes;

public:
    void GenerateMap();
    void Draw();
};

#endif