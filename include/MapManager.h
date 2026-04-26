#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include "raylib.h"
#include <vector>
#include <string>

class MapManager {
public:
    static std::vector<std::vector<int>> LoadMap(const std::string& filename);
    static std::vector<Model> GenerateWalls(const std::vector<std::vector<int>>& map, Texture2D texture);
    static void DrawWalls(const std::vector<Model>& walls, const std::vector<std::vector<int>>& map);
};

#endif