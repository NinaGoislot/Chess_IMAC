#pragma once

#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <string>
#include <utility>
#include "Game/Pieces/Piece.hpp"
#include "Game/State/SelectionState.hpp"
#include "Game/settings.hpp"
#include "Render/3D/PieceEffects.hpp"
#include "Render/3D/Shader.hpp"


class Board;

namespace Render3D {

class ResourceManager;

// Owns low-level OpenGL draw pipeline for board, pieces, skybox, and effects.

class GLRenderer {
public:
    using AnimatedPiecePositions = std::unordered_map<const Piece*, glm::vec3>;

    // Constructors
    GLRenderer() = default;
    ~GLRenderer();

    // Non-copyable
    GLRenderer(const GLRenderer&)            = delete;
    GLRenderer& operator=(const GLRenderer&) = delete;

    // Init function: loads shaders and creates shared geometry.
    bool initialize(const std::string& shaderDir);
    // Cleanup function: destroys allocated OpenGL resources.
    void destroy();

    // Starts board pass with selected turn tint shader.
    bool beginBoardPass(PieceColor currentTurn) const;
    // Applies static lighting uniforms for board/piece shading.
    void setupStaticLighting() const;

    // Render function: draws board body and tiles.
    void drawBoard(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition,
                   const SelectionState& selection) const;
    // Render function: draws gaps between board tiles.
    void drawBoardGaps(const glm::mat4& viewProjection, const settings& gameSettings) const;
    // Render function: draws top board tile quads.
    void drawTiles(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition,
                   const SelectionState& selection) const;
    // Render function: draws board side geometry.
    void drawBoardEdges(const glm::mat4& viewProjection, const settings& gameSettings) const;

    // Render function: draws regular and animated pieces.
    void drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager,
                    const AnimatedPiecePositions& animatedPiecePositions, const ExplodingPiecePositions& explodingPiecePositions) const;
    // Render function: draws procedural/texture skybox.
    void drawSkybox(const glm::mat4& view, const glm::mat4& projection, const settings& gameSettings, const ResourceManager& resourceManager) const;

private:
    // Cached uniform locations for board shader.
    struct UniformLocations {
        int mvp      = -1;
        int model    = -1;
        int color    = -1;
        int lightDir = -1;
        int ambient  = -1;
        int turnTint = -1;

        bool isValid() const
        {
            return mvp >= 0 && model >= 0 && color >= 0 && lightDir >= 0 && ambient >= 0 && turnTint >= 0;
        }
    };

    // Cached uniform locations for skybox shader.
    struct SkyboxUniformLocations {
        int vp          = -1;
        int topColor    = -1;
        int bottomColor = -1;
        int cubemap      = -1;
        int useCubemap   = -1;

        bool isValid() const
        {
            return vp >= 0
                   && topColor >= 0
                   && bottomColor >= 0
                   && cubemap >= 0
                   && useCubemap >= 0;
        }
    };

    // Cached uniform locations for explosion shader.
    struct ExplosionUniformLocations
    {
        int mvp      = -1;
        int model    = -1;
        int color    = -1;
        int lightDir = -1;
        int ambient  = -1;
        int progress = -1;

        bool isValid() const
        {
            return mvp >= 0 && model >= 0 && color >= 0 && lightDir >= 0 && ambient >= 0 && progress >= 0;
        }
    };

    // Queries and caches board shader uniform locations.
    static UniformLocations queryUniformLocations(const Shader& shader);
    // Queries and caches explosion shader uniform locations.
    static ExplosionUniformLocations queryExplosionUniformLocations(const Shader& shader);
    // Creates shared cube geometry VAO/VBO used by board rendering.
    void                    initializeCubeGeometry();

    // Draws one standard piece instance.
    void drawSinglePiece(const glm::mat4& viewProj, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ,
                         float topY, const ResourceManager& resourceManager) const;
    // Draws one exploding piece instance.
    void drawSingleExplodingPiece(const glm::mat4& viewProj, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ,
                                  float topY, float explosionProgress, const ResourceManager& resourceManager) const;

    // Shared cube geometry state.
    unsigned int _vao         = 0;
    unsigned int _vbo         = 0;
    bool         _initialized = false;
    bool         _skyboxReady = false;

    // Shader programs used by different render passes.
    Shader _boardShader;
    Shader _pieceExplosionShader;
    Shader _skyboxShader;

    UniformLocations         _boardUniforms{};
    ExplosionUniformLocations _pieceExplosionUniforms{};
    SkyboxUniformLocations   _skyboxUniforms{};
    bool                     _pieceExplosionReady = false;
};

} // namespace Render3D
