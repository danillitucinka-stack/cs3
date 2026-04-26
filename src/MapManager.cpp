#include "MapManager.h"
#include "raylib.h"
#include <vector>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

std::vector<std::vector<int>> MapManager::LoadMap(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::vector<int>> map;
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto json = nlohmann::json::parse(content);
        for (auto& row : json) {
            std::vector<int> rowVec;
            for (auto& cell : row) {
                rowVec.push_back(cell);
            }
            map.push_back(rowVec);
        }
    }
    return map;
}

std::vector<Model> MapManager::GenerateWalls(const std::vector<std::vector<int>>& map, Texture2D texture) {
    std::vector<Model> walls;
    for (size_t i = 0; i < map.size(); ++i) {
        for (size_t j = 0; j < map[i].size(); ++j) {
            if (map[i][j] == 1) {
                Model wall = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
                wall.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
                walls.push_back(wall);
            }
        }
    }
    return walls;
}

void MapManager::DrawWalls(const std::vector<Model>& walls, const std::vector<std::vector<int>>& map) {
    int index = 0;
    for (size_t i = 0; i < map.size(); ++i) {
        for (size_t j = 0; j < map[i].size(); ++j) {
            if (map[i][j] == 1) {
                DrawModel(walls[index], (Vector3){(float)j * 2.0f - 10.0f, 2.0f, (float)i * 2.0f - 10.0f}, 1.0f, WHITE);
                index++;
            }
        }
    }
}