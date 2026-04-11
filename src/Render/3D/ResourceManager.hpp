#pragma once

#include <array>
#include <cstddef>
#include "Game/Pieces/Piece.hpp"

struct ModelMeshData;

namespace Render3D {

// -------- EXPLANATION --------
// ResourceManager is responsible for managing the OpenGL resources for 3D models, such as VAOs, VBOs, and EBOs.


class ResourceManager {
public:
    struct PieceMeshGlData
    {
        unsigned int vao        = 0;
        unsigned int vbo        = 0;
        unsigned int ebo        = 0;
        int          indexCount = 0;

        bool isValid() const { return vao != 0 && vbo != 0 && ebo != 0 && indexCount > 0; }
    };

    ResourceManager() = default;
    ~ResourceManager();

    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    bool initialize(const std::string& assetBasePath);
    void destroy();

    const PieceMeshGlData* pieceMeshFor(PieceType type) const;
    unsigned int           skyboxCubemap() const { return _skyboxCubemap; }

private:
    bool uploadPieceMesh(PieceType type, const ModelMeshData& meshData);
    void initializePieceModels(const std::string& assetBasePath);
    bool loadSkyboxCubemap(const std::string& assetBasePath);
    void destroyPieceMeshes();
    void destroySkybox();

    static constexpr std::size_t PieceMeshCount = 6u;

    std::array<PieceMeshGlData, PieceMeshCount> _pieceMeshes{};

    unsigned int _skyboxCubemap = 0;
    bool         _initialized   = false;
};

} // namespace Render3D
