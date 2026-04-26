#include "Level.h"
#include "raylib.h"

void Level::Load(std::string filename) {
    // Load level data from JSON
    std::ifstream f(filename);
    if (f) {
        nlohmann::json j = nlohmann::json::parse(f);
        name = j["name"];
        enemyCount = j["enemies"];
        // Load map
        for (auto& w : j["walls"]) {
            Model wall = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
            walls.push_back(wall);
        }
    }
}

void Level::Draw() {
    for (size_t i = 0; i < walls.size(); ++i) {
        DrawModel(walls[i], {0.0f, 2.0f, (float)i * 2.0f}, 1.0f, BEIGE);
    }
}