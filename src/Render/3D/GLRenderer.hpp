#pragma once

#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "Game/Pieces/Piece.hpp"
#include "Game/settings.hpp"
#include "Render/3D/PieceEffects.hpp"
#include "Render/3D/Shader.hpp"

class Board;

namespace Render3D {

class ResourceManager;

// -------- EXPLANATION --------
// GLRenderer is the "Painter". It holds shaders, VAOs, and knows how to draw the board and pieces.
// It is a simple renderer for 3D graphics.

class GLRenderer {
public:
    using AnimatedPiecePositions = std::unordered_map<const Piece*, glm::vec3>;

    // Constructor and destructor
    GLRenderer() = default;
    ~GLRenderer();

    // Non-copyable
    GLRenderer(const GLRenderer&)            = delete;
    GLRenderer& operator=(const GLRenderer&) = delete;

    // Initialization and cleanup
    bool initialize(const std::string& shaderDir);
    void destroy();

    // Rendering functions
    bool beginBoardPass(PieceColor currentTurn) const;
    void setupStaticLighting() const;


    void drawBoard(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings) const;
    void drawBoardGaps(const glm::mat4& viewProjection, const settings& gameSettings) const;
    void drawTiles(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings) const;
    void drawBoardEdges(const glm::mat4& viewProjection, const settings& gameSettings) const;

    void drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager,
                    const AnimatedPiecePositions& animatedPiecePositions, const ExplodingPiecePositions& explodingPiecePositions) const;
    void drawSkybox(const glm::mat4& view, const glm::mat4& projection, const settings& gameSettings, const ResourceManager& resourceManager) const;

private:
    struct UniformLocations
    {
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

    struct SkyboxUniformLocations
    {
        int vp           = -1;
        int topColor     = -1;
        int bottomColor  = -1;
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

    static UniformLocations queryUniformLocations(const Shader& shader);
    static ExplosionUniformLocations queryExplosionUniformLocations(const Shader& shader);
    void                    initializeCubeGeometry();

    void drawSinglePiece(const glm::mat4& viewProj, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ,
                         float topY, const ResourceManager& resourceManager) const;
    void drawSingleExplodingPiece(const glm::mat4& viewProj, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ,
                                  float topY, float explosionProgress, const ResourceManager& resourceManager) const;

    unsigned int _vao        = 0;
    unsigned int _vbo        = 0;
    bool         _initialized = false;
    bool         _skyboxReady = false;

    Shader _boardShader;
    Shader _pieceExplosionShader;
    Shader _skyboxShader;

    UniformLocations         _boardUniforms{};
    ExplosionUniformLocations _pieceExplosionUniforms{};
    SkyboxUniformLocations   _skyboxUniforms{};
    bool                     _pieceExplosionReady = false;
};

} // namespace Render3D
