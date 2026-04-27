#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <btBulletDynamicsCommon.h>
#include <raylib.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Structs for loaded resources
struct TextureResource {
    std::string name;
    Texture2D texture;
};

struct ModelResource {
    std::string name;
    Model model;
    std::vector<btCollisionShape*> collisionShapes; // For physics
};

struct BSPResource {
    std::string name;
    std::vector<Vector3> vertices;
    std::vector<int> indices;
    std::vector<TextureResource> textures;
    btBvhTriangleMeshShape* physicsMesh; // Physics mesh
};

struct SoundResource {
    std::string name;
    Sound sound;
};

struct SpriteResource {
    std::string name;
    Texture2D texture;
};

// ResourceManager class
class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Load resources from CS 1.6 directory
    void LoadResources(const std::filesystem::path& csDir);

    // Getters
    TextureResource* GetTexture(const std::string& name);
    ModelResource* GetModel(const std::string& name);
    BSPResource* GetBSP(const std::string& name);
    SoundResource* GetSound(const std::string& name);
    SpriteResource* GetSprite(const std::string& name);

private:
    // Loaders
    void LoadWADTextures(const std::filesystem::path& wadPath);
    void LoadMDLModels(const std::filesystem::path& mdlPath);
    void LoadBSPGeometry(const std::filesystem::path& bspPath);
    void LoadSounds(const std::filesystem::path& soundDir);
    void LoadSprites(const std::filesystem::path& spriteDir);

    // Generate physics mesh from BSP
    btBvhTriangleMeshShape* GeneratePhysicsMesh(const std::vector<Vector3>& vertices, const std::vector<int>& indices);

    // Maps for resources
    std::unordered_map<std::string, TextureResource> textures;
    std::unordered_map<std::string, ModelResource> models;
    std::unordered_map<std::string, BSPResource> bsps;
    std::unordered_map<std::string, SoundResource> sounds;
    std::unordered_map<std::string, SpriteResource> sprites;
};

#endif // RESOURCE_MANAGER_H