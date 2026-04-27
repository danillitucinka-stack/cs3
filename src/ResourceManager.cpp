#include "ResourceManager.h"
#include <iostream>
#include <fstream>
#include <cstring>

// WAD3 structures (simplified)
struct WADHeader {
    char magic[4]; // "WAD3"
    int numLumps;
    int dirOffset;
};

struct WADLump {
    int offset;
    int size;
    int sizeUncompressed;
    char type;
    char compression;
    short dummy;
    char name[16];
};

struct MIPTexture {
    char name[16];
    unsigned int width, height;
    unsigned int offsets[4]; // MIP levels
};

// MDL structures (GoldSrc v10 simplified)
struct MDLHeader {
    int id; // IDST
    int version; // 10
    char name[64];
    int length;
    Vector3 eyeposition;
    Vector3 min;
    Vector3 max;
    Vector3 bbmin;
    Vector3 bbmax;
    int flags;
    int numBones;
    int boneIndex;
    int numBoneControllers;
    int boneControllerIndex;
    int numHitBoxes;
    int hitBoxIndex;
    int numSeq;
    int seqIndex;
    int numSeqGroups;
    int seqGroupIndex;
    int numTextures;
    int textureIndex;
    int textureDataIndex;
    int numSkinRef;
    int numSkinFamilies;
    int skinIndex;
    int numBodyParts;
    int bodyPartIndex;
    int numAttachments;
    int attachmentIndex;
    int soundTable;
    int soundIndex;
    int soundGroups;
    int soundGroupIndex;
    int numTransitions;
    int transitionIndex;
};

// BSP structures (v30 simplified)
struct BSPHeader {
    int version; // 30
    int entitiesOffset;
    int entitiesSize;
    int planesOffset;
    int planesSize;
    int texturesOffset;
    int texturesSize;
    int verticesOffset;
    int verticesSize;
    int visibilityOffset;
    int visibilitySize;
    int nodesOffset;
    int nodesSize;
    int texInfoOffset;
    int texInfoSize;
    int facesOffset;
    int facesSize;
    int lightmapsOffset;
    int lightmapsSize;
    int clipNodesOffset;
    int clipNodesSize;
    int leavesOffset;
    int leavesSize;
    int markSurfacesOffset;
    int markSurfacesSize;
    int edgesOffset;
    int edgesSize;
    int surfEdgesOffset;
    int surfEdgesSize;
    int modelsOffset;
    int modelsSize;
};

struct BSPFace {
    short planeNum;
    short side;
    int firstEdge;
    short numEdges;
    short texInfo;
    char lightStyles[4];
    int lightmapOffset;
};

ResourceManager::ResourceManager() {
    // Initialize if needed
}

ResourceManager::~ResourceManager() {
    // Unload resources
    for (auto& tex : textures) {
        UnloadTexture(tex.second.texture);
    }
    for (auto& mod : models) {
        UnloadModel(mod.second.model);
        for (auto shape : mod.second.collisionShapes) {
            delete shape;
        }
    }
    for (auto& bsp : bsps) {
        delete bsp.second.physicsMesh;
        for (auto& tex : bsp.second.textures) {
            UnloadTexture(tex.texture);
        }
    }
    for (auto& snd : sounds) {
        UnloadSound(snd.second.sound);
    }
    for (auto& spr : sprites) {
        UnloadTexture(spr.second.texture);
    }
}

void ResourceManager::LoadResources(const std::filesystem::path& csDir) {
    std::filesystem::path cstrikeDir = csDir / "cstrike";
    if (!std::filesystem::exists(cstrikeDir)) {
        std::cerr << "CS 1.6 directory not found: " << cstrikeDir << std::endl;
        return;
    }

    // Load WAD textures
    for (const auto& entry : std::filesystem::directory_iterator(cstrikeDir)) {
        if (entry.path().extension() == ".wad") {
            LoadWADTextures(entry.path());
        }
    }

    // Load MDL models
    std::filesystem::path modelsDir = cstrikeDir / "models";
    if (std::filesystem::exists(modelsDir)) {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(modelsDir)) {
            if (entry.path().extension() == ".mdl") {
                LoadMDLModels(entry.path());
            }
        }
    }

    // Load BSP maps
    std::filesystem::path mapsDir = cstrikeDir / "maps";
    if (std::filesystem::exists(mapsDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(mapsDir)) {
            if (entry.path().extension() == ".bsp") {
                LoadBSPGeometry(entry.path());
            }
        }
    }

    // Load sounds
    std::filesystem::path soundDir = cstrikeDir / "sound";
    LoadSounds(soundDir);

    // Load sprites
    std::filesystem::path spritesDir = cstrikeDir / "sprites";
    LoadSprites(spritesDir);
}

// Getters
TextureResource* ResourceManager::GetTexture(const std::string& name) {
    auto it = textures.find(name);
    return it != textures.end() ? &it->second : nullptr;
}

ModelResource* ResourceManager::GetModel(const std::string& name) {
    auto it = models.find(name);
    return it != models.end() ? &it->second : nullptr;
}

BSPResource* ResourceManager::GetBSP(const std::string& name) {
    auto it = bsps.find(name);
    return it != bsps.end() ? &it->second : nullptr;
}

SoundResource* ResourceManager::GetSound(const std::string& name) {
    auto it = sounds.find(name);
    return it != sounds.end() ? &it->second : nullptr;
}

SpriteResource* ResourceManager::GetSprite(const std::string& name) {
    auto it = sprites.find(name);
    return it != sprites.end() ? &it->second : nullptr;
}

// Load WAD3 textures
void ResourceManager::LoadWADTextures(const std::filesystem::path& wadPath) {
    std::ifstream file(wadPath, std::ios::binary);
    if (!file) return;

    WADHeader header;
    file.read((char*)&header, sizeof(WADHeader));
    if (std::string(header.magic, 4) != "WAD3") return;

    std::vector<WADLump> lumps(header.numLumps);
    file.seekg(header.dirOffset);
    file.read((char*)lumps.data(), sizeof(WADLump) * header.numLumps);

    for (const auto& lump : lumps) {
        if (lump.type == 67) { // MIP texture
            file.seekg(lump.offset);
            MIPTexture mip;
            file.read((char*)&mip, sizeof(MIPTexture));

            // Load the highest MIP level (full res)
            int mipSize = mip.width * mip.height;
            std::vector<unsigned char> data(mipSize * 3); // RGB
            file.seekg(lump.offset + mip.offsets[0]);
            file.read((char*)data.data(), mipSize);

            // Convert to RGBA (assume no alpha, add 255)
            std::vector<unsigned char> rgba(mipSize * 4);
            for (int i = 0; i < mipSize; ++i) {
                rgba[i * 4] = data[i * 3];
                rgba[i * 4 + 1] = data[i * 3 + 1];
                rgba[i * 4 + 2] = data[i * 3 + 2];
                rgba[i * 4 + 3] = 255;
            }

            Image img = {
                .data = rgba.data(),
                .width = (int)mip.width,
                .height = (int)mip.height,
                .mipmaps = 1,
                .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
            };
            Texture2D tex = LoadTextureFromImage(img);

            TextureResource res;
            res.name = mip.name;
            res.texture = tex;
            textures[res.name] = res;
        }
    }
}

// Load MDL models (simplified)
void ResourceManager::LoadMDLModels(const std::filesystem::path& mdlPath) {
    std::ifstream file(mdlPath, std::ios::binary);
    if (!file) return;

    MDLHeader header;
    file.read((char*)&header, sizeof(MDLHeader));
    if (header.id != 0x54534449 || header.version != 10) return; // "IDST" and v10

    // For simplicity, create a dummy model or load mesh data
    // In real impl, parse vertices, triangles, etc.
    // Assume single body part, single mesh

    // Placeholder: create a cube model
    Model model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    ModelResource res;
    res.name = mdlPath.filename().string();
    res.model = model;
    // Add collision shape (box)
    btCollisionShape* shape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
    res.collisionShapes.push_back(shape);
    models[res.name] = res;
}

// Load BSP geometry
void ResourceManager::LoadBSPGeometry(const std::filesystem::path& bspPath) {
    std::ifstream file(bspPath, std::ios::binary);
    if (!file) return;

    BSPHeader header;
    file.read((char*)&header, sizeof(BSPHeader));
    if (header.version != 30) return;

    // Read vertices
    std::vector<Vector3> vertices(header.verticesSize / sizeof(Vector3));
    file.seekg(header.verticesOffset);
    file.read((char*)vertices.data(), header.verticesSize);

    // Read faces
    int numFaces = header.facesSize / sizeof(BSPFace);
    std::vector<BSPFace> faces(numFaces);
    file.seekg(header.facesOffset);
    file.read((char*)faces.data(), header.facesSize);

    // Read surfedges and edges to build indices
    std::vector<int> surfEdges(header.surfEdgesSize / sizeof(int));
    file.seekg(header.surfEdgesOffset);
    file.read((char*)surfEdges.data(), header.surfEdgesSize);

    struct BSPEdge { short v[2]; };
    std::vector<BSPEdge> edges(header.edgesSize / sizeof(BSPEdge));
    file.seekg(header.edgesOffset);
    file.read((char*)edges.data(), header.edgesSize);

    std::vector<int> indices;
    for (const auto& face : faces) {
        for (int i = 0; i < face.numEdges; ++i) {
            int edgeIndex = surfEdges[face.firstEdge + i];
            BSPEdge edge = edges[abs(edgeIndex)];
            if (edgeIndex >= 0) {
                indices.push_back(edge.v[0]);
                indices.push_back(edge.v[1]);
            } else {
                indices.push_back(edge.v[1]);
                indices.push_back(edge.v[0]);
            }
        }
    }

    // Generate physics mesh
    btBvhTriangleMeshShape* physicsMesh = GeneratePhysicsMesh(vertices, indices);

    BSPResource res;
    res.name = bspPath.filename().string();
    res.vertices = vertices;
    res.indices = indices;
    res.physicsMesh = physicsMesh;
    // Textures: placeholder
    TextureResource tex;
    tex.name = "bsp_tex";
    tex.texture = LoadTextureFromImage(GenImageColor(64, 64, GRAY));
    res.textures.push_back(tex);
    bsps[res.name] = res;
}

// Generate physics mesh from BSP data
btBvhTriangleMeshShape* ResourceManager::GeneratePhysicsMesh(const std::vector<Vector3>& vertices, const std::vector<int>& indices) {
    btTriangleMesh* mesh = new btTriangleMesh();
    for (size_t i = 0; i < indices.size(); i += 3) {
        btVector3 v0(vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z);
        btVector3 v1(vertices[indices[i+1]].x, vertices[indices[i+1]].y, vertices[indices[i+1]].z);
        btVector3 v2(vertices[indices[i+2]].x, vertices[indices[i+2]].y, vertices[indices[i+2]].z);
        mesh->addTriangle(v0, v1, v2);
    }
    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);
    return shape;
}

// Load sounds
void ResourceManager::LoadSounds(const std::filesystem::path& soundDir) {
    if (!std::filesystem::exists(soundDir)) return;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(soundDir)) {
        if (entry.path().extension() == ".wav") {
            SoundResource res;
            res.name = entry.path().filename().string();
            res.sound = LoadSound(entry.path().string().c_str());
            sounds[res.name] = res;
        }
    }
}

// Load sprites
void ResourceManager::LoadSprites(const std::filesystem::path& spriteDir) {
    if (!std::filesystem::exists(spriteDir)) return;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(spriteDir)) {
        if (entry.path().extension() == ".spr") {
            // .spr is Half-Life sprite format, simplified load as image
            // In real, parse .spr
            // Placeholder: assume .spr is image
            SpriteResource res;
            res.name = entry.path().filename().string();
            res.texture = LoadTexture(entry.path().string().c_str()); // Assume raylib can load
            sprites[res.name] = res;
        }
    }
}