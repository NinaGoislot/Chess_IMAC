#pragma once

#include <array>
#include <imgui.h>
#include <glm/glm.hpp>
#include "Board/Board.hpp"
#include "Game/settings.hpp"
#include "3D/Shader.hpp"

struct ModelMeshData;


class Board3DRenderer
{
public:
    // constructor and destructor
    Board3DRenderer() = default;
    ~Board3DRenderer();

    // non-copyable
    Board3DRenderer(const Board3DRenderer&)            = delete;
    Board3DRenderer& operator=(const Board3DRenderer&) = delete;

    // main render function and texture
    void render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height);
    ImTextureID colorTexture() const;
    

private:
    struct UniformLocations
    {
        int mvp      = -1;
        int model    = -1;
        int color    = -1;
        int lightDir = -1;
        int ambient  = -1;

        bool isValid() const
        {
            return mvp >= 0 && model >= 0 && color >= 0 && lightDir >= 0 && ambient >= 0;
        }
    };

    struct PieceMeshGlData
    {
        unsigned int vao        = 0;
        unsigned int vbo        = 0;
        unsigned int ebo        = 0;
        int          indexCount = 0;

        bool isValid() const { return vao != 0 && vbo != 0 && ebo != 0 && indexCount > 0; }
    };

    // initialization and cleanup functions
    void initializeIfNeeded();
    void initializePieceModels();
    bool uploadPieceMesh(PieceType type, const ModelMeshData& meshData);
    const PieceMeshGlData* pieceMeshFor(PieceType type) const;
    void destroyPieceMeshes();
    void ensureFramebufferSize(int width, int height);
    void destroyFramebuffer();
    void destroyGlResources();

    // OpenGL resources
    unsigned int _vao           = 0;
    unsigned int _vbo           = 0;
    unsigned int _fbo           = 0;
    unsigned int _colorTexture  = 0;
    unsigned int _depthStencil  = 0;
    int          _framebufferW  = 0;
    int          _framebufferH  = 0;
    bool         _initialized   = false;

    static constexpr std::size_t PieceMeshCount = 6u;
    std::array<PieceMeshGlData, PieceMeshCount> _pieceMeshes{};

    // shader and uniform locations
    Shader _whiteTurnShader;
    Shader _blackTurnShader;

    const Shader*           _activeShader   = nullptr;
    const UniformLocations* _activeUniforms = nullptr;
    UniformLocations        _whiteUniforms{};
    UniformLocations        _blackUniforms{};

    // rendering helper functions
    bool prepareRenderState(int width, int height, PieceColor currentTurn);
    static UniformLocations queryUniformLocations(const Shader& shader);
    glm::mat4 calculateCameraViewProjection(const settings& gameSettings, float aspect,const glm::vec3& target) const;
    glm::vec3 calculatePieceTarget(const Board& board) const;
    void setupStaticLighting() const;
    void drawBoardTiles(const glm::mat4& viewProjection, const settings& gameSettings) const;
    void drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings) const;
};
