#include "AssetLoader.h"
#include <iostream>

// Constructor: Initialize HLLib
AssetLoader::AssetLoader() : package(nullptr) {
    hlInitialize();
}

// Destructor: Cleanup
AssetLoader::~AssetLoader() {
    if (package) {
        delete package;
    }
    hlShutdown();
}

// Helper to open package
bool AssetLoader::OpenPackage(const std::filesystem::path& path) {
    if (package) {
        delete package;
    }
    package = new HLLib::CPackage();
    HLLib::CDirectoryFolder* root = nullptr;
    if (package->Open(static_cast<HLLib::EPackageType>(hlGetPackageTypeFromName(path.string().c_str())), path.string().c_str(), &root) == hlTrue) {
        return true;
    } else {
        delete package;
        package = nullptr;
        return false;
    }
}

// Helper to close package
void AssetLoader::ClosePackage() {
    if (package) {
        package->Close();
        delete package;
        package = nullptr;
    }
}

// Load textures from .wad file
std::vector<LoadedTexture> AssetLoader::LoadWADTextures(const std::filesystem::path& wadPath) {
    std::vector<LoadedTexture> textures;
    if (!OpenPackage(wadPath)) {
        std::cerr << "Failed to open WAD file: " << wadPath << std::endl;
        return textures;
    }

    // Assume package is open, get root folder
    HLLib::CDirectoryFolder* root = package->GetRoot();
    if (!root) {
        ClosePackage();
        return textures;
    }

    // Iterate through items (simplified, assuming miptex)
    for (hlUInt i = 0; i < root->GetCount(); ++i) {
        HLLib::CDirectoryItem* item = root->GetItem(i);
        if (item->GetType() == hlItemTypeFile) {
            HLLib::CDirectoryFile* file = static_cast<HLLib::CDirectoryFile*>(item);
            // For WAD, items are lumps, need to check type
            // This is simplified; in reality, check lump type for miptex (67)
            hlChar name[256];
            file->GetName(name, sizeof(name));
            // Load texture data (placeholder)
            // In real impl, extract miptex data and create raylib texture
            LoadedTexture tex;
            tex.name = name;
            // tex.texture = LoadTextureFromMemory(...); // Need to implement
            // For now, create dummy
            tex.texture = LoadTextureFromImage(GenImageColor(64, 64, RED)); // Placeholder
            textures.push_back(tex);
        }
    }

    ClosePackage();
    return textures;
}

// Load models from .mdl file (placeholder)
std::vector<LoadedModel> AssetLoader::LoadMDLModels(const std::filesystem::path& mdlPath) {
    std::vector<LoadedModel> models;
    // HLLib may not directly support .mdl as package, so treat as file
    // For now, placeholder: load as obj if possible, else dummy
    std::string pathStr = mdlPath.string();
    if (FileExists(pathStr.c_str())) {
        // If .mdl can be loaded as model, but raylib doesn't support .mdl
        // Placeholder: create dummy model
        LoadedModel mod;
        mod.name = mdlPath.filename().string();
        mod.model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f)); // Dummy
        models.push_back(mod);
    }
    return models;
}

// Load BSP geometry
BSPData AssetLoader::LoadBSP(const std::filesystem::path& bspPath) {
    BSPData data;
    data.name = bspPath.filename().string();
    if (!OpenPackage(bspPath)) {
        std::cerr << "Failed to open BSP file: " << bspPath << std::endl;
        return data;
    }

    // Simplified BSP loading
    // In reality, parse lumps for vertices, faces, etc.
    // For now, create dummy geometry
    data.vertices = {
        {0,0,0}, {1,0,0}, {1,1,0}, {0,1,0},
        {0,0,1}, {1,0,1}, {1,1,1}, {0,1,1}
    };
    data.indices = {
        0,1,2, 0,2,3, // bottom
        4,5,6, 4,6,7, // top
        0,1,5, 0,5,4, // front
        2,3,7, 2,7,6, // back
        1,2,6, 1,6,5, // right
        3,0,4, 3,4,7  // left
    };
    // Dummy texture
    LoadedTexture tex;
    tex.name = "dummy";
    tex.texture = LoadTextureFromImage(GenImageColor(64, 64, BLUE));
    data.textures.push_back(tex);

    ClosePackage();
    return data;
}