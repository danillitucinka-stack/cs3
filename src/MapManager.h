#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "raylib.h"
#include "AssetLoader.h"
#include <vector>

class MapManager {
private:
    std::vector<Model> walls;
    std::vector<Model> boxes;
    BSPData bspData;
    Model mapModel;

public:
    void SetBSPData(const BSPData& data);
    void GenerateDust2();
    void Draw();
};

#endif