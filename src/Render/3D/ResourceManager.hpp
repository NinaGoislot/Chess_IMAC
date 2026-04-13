#pragma once

#include <array>
#include <cstddef>
#include "Game/Pieces/Piece.hpp"

struct ModelMeshData;

namespace Render3D {

// Owns 3D model OpenGL buffers and skybox cubemap resources.


class ResourceManager {
public:
    // OpenGL handles for one piece mesh.
    struct PieceMeshGlData
    {
        unsigned int vao        = 0;
        unsigned int vbo        = 0;
        unsigned int ebo        = 0;
        int          indexCount = 0;

        bool isValid() const { return vao != 0 && vbo != 0 && ebo != 0 && indexCount > 0; }
    };

    // Constructors
    ResourceManager() = default;
    ~ResourceManager();

    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Init function: loads piece meshes and skybox assets.
    bool initialize(const std::string& assetBasePath);
    // Cleanup function: frees all loaded resources.
    void destroy();

    const PieceMeshGlData* getPieceMeshFor(PieceType type) const;
    unsigned int           getSkyboxCubemap() const { return _skyboxCubemap; }

private:
    // Uploads one normalized mesh to GPU buffers.
    bool uploadPieceMesh(PieceType type, const ModelMeshData& meshData);
    // Loads and initializes all piece meshes.
    void initializePieceModels(const std::string& assetBasePath);
    // Initializes optional chaos model resources.
    void initChaosModel(const std::string& assetBasePath, const std::string& modelName);
    // Loads cubemap textures for skybox rendering.
    bool loadSkyboxCubemap(const std::string& assetBasePath);
    // Destroys all piece mesh buffers.
    void destroyPieceMeshes();
    // Destroys skybox cubemap resource.
    void destroySkybox();

    static constexpr std::size_t PieceMeshCount = 6u;

    // Piece mesh data indexed by piece type.
    std::array<PieceMeshGlData, PieceMeshCount> _pieceMeshes{};

    // Skybox cubemap texture id.
    unsigned int _skyboxCubemap = 0;
    // Indicates whether resources are currently loaded.
    bool         _initialized   = false;
};

} // namespace Render3D
