#include "MapManager.h"
#include "raylib.h"

void MapManager::SetBSPData(const BSPData& data) {
    bspData = data;
}

void MapManager::GenerateDust2() {
    if (!bspData.vertices.empty()) {
        // Load from BSP data
        Mesh mesh = {0};
        mesh.vertexCount = bspData.vertices.size();
        mesh.triangleCount = bspData.indices.size() / 3;
        mesh.vertices = (float*)malloc(sizeof(float) * 3 * mesh.vertexCount);
        mesh.indices = (unsigned short*)malloc(sizeof(unsigned short) * bspData.indices.size());
        for (size_t i = 0; i < bspData.vertices.size(); ++i) {
            mesh.vertices[i * 3] = bspData.vertices[i].x;
            mesh.vertices[i * 3 + 1] = bspData.vertices[i].y;
            mesh.vertices[i * 3 + 2] = bspData.vertices[i].z;
        }
        for (size_t i = 0; i < bspData.indices.size(); ++i) {
            mesh.indices[i] = bspData.indices[i];
        }
        // Add UVs and normals if available, simplified
        mesh.texcoords = (float*)malloc(sizeof(float) * 2 * mesh.vertexCount);
        for (int i = 0; i < mesh.vertexCount; ++i) {
            mesh.texcoords[i * 2] = 0.0f;
            mesh.texcoords[i * 2 + 1] = 0.0f;
        }
        UploadMesh(&mesh, false);
        mapModel = LoadModelFromMesh(mesh);
        if (!bspData.textures.empty()) {
            mapModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = bspData.textures[0].texture;
        }
        return;
    }

    // Fallback to procedural generation
    // Dust2 A-site: walls around, boxes in center, ramp, etc.
    int map[20][20] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},  // Boxes
        {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
        {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    Model wallModel = LoadModelFromMesh(GenMeshCube(1.0f, 4.0f, 1.0f));
    Model boxModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            if (map[i][j] == 1) walls.push_back(wallModel);
            else if (map[i][j] == 2) boxes.push_back(boxModel);
        }
    }
}

void MapManager::Draw() {
    if (!bspData.vertices.empty()) {
        // Draw BSP model
        DrawModel(mapModel, {0, 0, 0}, 1.0f, WHITE);
        return;
    }

    // Draw procedural map
    int wallIndex = 0, boxIndex = 0;
    int map[20][20] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},  // Boxes
        {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
        {1,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,2,2,2,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            if (map[i][j] == 1 && wallIndex < walls.size()) {
                DrawModel(walls[wallIndex], {(float)j * 2.0f - 20.0f, 2.0f, (float)i * 2.0f - 20.0f}, 1.0f, BEIGE);
                wallIndex++;
            } else if (map[i][j] == 2 && boxIndex < boxes.size()) {
                DrawModel(boxes[boxIndex], {(float)j * 2.0f - 20.0f, 0.5f, (float)i * 2.0f - 20.0f}, 1.0f, BROWN);
                boxIndex++;
            }
        }
    }
}