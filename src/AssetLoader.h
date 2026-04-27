#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <string>
#include <vector>
#include <filesystem>
#include <HLLib/HLLib.h>
#include <raylib.h>

// Struct for loaded texture
struct LoadedTexture {
    std::string name;
    Texture2D texture;
};

// Struct for loaded model (simplified for now)
struct LoadedModel {
    std::string name;
    Model model;
};

// Struct for BSP data (simplified)
struct BSPData {
    std::string name;
    std::vector<Vector3> vertices;
    std::vector<int> indices;
    std::vector<LoadedTexture> textures;
};

class AssetLoader {
public:
    // Constructor initializes HLLib
    AssetLoader();

    // Destructor cleans up
    ~AssetLoader();

    // Load textures from .wad file
    std::vector<LoadedTexture> LoadWADTextures(const std::filesystem::path& wadPath);

    // Load models from .mdl file (placeholder for now)
    std::vector<LoadedModel> LoadMDLModels(const std::filesystem::path& mdlPath);

    // Load BSP geometry
    BSPData LoadBSP(const std::filesystem::path& bspPath);

private:
    // Helper to open package with HLLib
    bool OpenPackage(const std::filesystem::path& path);

    // Helper to close package
    void ClosePackage();

    // Current package handle
    HLLib::CPackage* package;
};

#endif // ASSET_LOADER_H