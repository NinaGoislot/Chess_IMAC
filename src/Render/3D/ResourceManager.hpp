#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <glm/vec3.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include "Game/Pieces/Piece.hpp"
#include "utilities/AppConfig.hpp"

struct ModelMeshData;
struct TextureData;

namespace Render3D {

// Owns 3D model OpenGL buffers and skybox cubemap resources.


class ResourceManager {
public:
    static constexpr std::string_view BoardEdgeTextureId = "board.edge";

    // OpenGL handles for one piece mesh.
    struct PieceMeshGlData
    {
        unsigned int vao        = 0;
        unsigned int vbo        = 0;
        unsigned int ebo        = 0;
        int          indexCount = 0;
        unsigned int textureId  = 0;
        struct SubMeshGlData
        {
            uint32_t indexOffset = 0;
            uint32_t indexCount  = 0;
            unsigned int textureId = 0;
            glm::vec3 baseColorFactor{1.f, 1.f, 1.f};
        };
        std::vector<SubMeshGlData> submeshes;

        bool isValid() const { return vao != 0 && vbo != 0 && ebo != 0 && indexCount > 0; }
    };

    // Constructors
    ResourceManager() = default;
    ~ResourceManager();

    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Init function: loads piece meshes and skybox assets.
    bool initialize(const AssetPaths& assetPaths);
    // Cleanup function: frees all loaded resources.
    void destroy();

    const PieceMeshGlData* getPieceMeshFor(PieceType type) const;
    const PieceMeshGlData* getKirbyMesh() const;
    unsigned int           getSkyboxCubemap() const { return _skyboxCubemap; }
    unsigned int           getTexture2D(const std::string& textureId) const;

private:
    // Uploads one normalized mesh to GPU buffers.
    bool uploadPieceMesh(PieceType type, const ModelMeshData& meshData);
    bool uploadMesh(PieceMeshGlData& mesh, const ModelMeshData& meshData);
    // Loads and initializes all piece meshes.
    void initializePieceModels(const std::string& modelsDirectory);
    // Initializes optional chaos model resources.
    void initChaosModel(const std::string& modelsDirectory, const std::string& modelName);
    // Loads one 2D texture into the cache using ordered file candidates.
    bool loadTexture2D(const std::string& textureId, const std::vector<std::string>& candidatePaths, bool optional = true);
    // Loads one 2D texture into the cache from raw pixel data.
    bool loadTexture2D(const std::string& textureId, const TextureData& data, bool generateMipmaps = true);
    // Loads all board-related textures.
    void initializeBoardTextures(const std::string& boardTexturesDirectory);
    // Loads cubemap textures for skybox rendering.
    bool loadSkyboxCubemap(const std::string& skyboxDirectory);
    // Destroys all piece mesh buffers.
    void destroyPieceMeshes();
    // Destroys the Kirby mesh buffers.
    void destroyKirbyMesh();
    // Destroys one mesh buffer set.
    void destroyMesh(PieceMeshGlData& mesh);
    // Destroys skybox cubemap resource.
    void destroySkybox();
    // Destroys a cached 2D texture resource.
    void destroyTexture2D(const std::string& textureId);
    // Destroys all cached 2D texture resources.
    void destroyAllTextures2D();

    static constexpr std::size_t PieceMeshCount = 6u;

    // Piece mesh data indexed by piece type.
    std::array<PieceMeshGlData, PieceMeshCount> _pieceMeshes{};
    // Kirby mesh used in chaos mode.
    PieceMeshGlData _kirbyMesh{};

    // Skybox cubemap texture id.
    unsigned int _skyboxCubemap = 0;
    // Cached 2D textures by logical id.
    std::unordered_map<std::string, unsigned int> _textures2D;
    // Indicates whether resources are currently loaded.
    bool         _initialized   = false;
};

} // namespace Render3D
