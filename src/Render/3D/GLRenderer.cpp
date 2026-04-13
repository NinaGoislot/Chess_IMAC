#include "GLRenderer.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Board/Board.hpp"
#include "Primitives.hpp"
#include "ResourceManager.hpp"

namespace {

constexpr std::size_t PIECE_TYPE_COUNT = 6u;

constexpr float BOARD_TILE_SIZE  = 0.94f;
constexpr float BOARD_CENTER_Y   = -0.05f;
constexpr float PIECE_LIFT_Y     = 0.01f;
constexpr float PIECE_BASE_WIDTH = 0.45f;

constexpr std::array<float, PIECE_TYPE_COUNT> PIECES_HEIGHT = {
    0.6f,
    0.95f,
    0.8f,
    0.9f,
    1.1f,
    1.2f,
};

const glm::vec3 WHITE_TURN_TINT{1.08f, 1.06f, 1.00f};
const glm::vec3 BLACK_TURN_TINT{0.88f, 0.94f, 1.08f};

std::size_t pieceTypeIndex(PieceType type)
{
    return static_cast<std::size_t>(type);
}

glm::vec3 colorForPiece(const Piece* piece)
{
    if (piece == nullptr)
        return glm::vec3{0.f, 0.f, 0.f};

    if (piece->color() == PieceColor::White)
        return glm::vec3{0.9f, 0.9f, 0.9f};

    return glm::vec3{0.15f, 0.15f, 0.2f};
}

void drawCube(int mvpLocation, int modelLocation, int colorLocation, const glm::mat4& viewProjection, const glm::mat4& model, const glm::vec3& color)
{
    const glm::mat4 mvp = viewProjection * model;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(colorLocation, color.x, color.y, color.z);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawIndexedMesh(int mvpLocation, int modelLocation, int colorLocation, const glm::mat4& viewProjection, const glm::mat4& model, const glm::vec3& color, unsigned int vao, int indexCount)
{
    const glm::mat4 mvp = viewProjection * model;
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(colorLocation, color.x, color.y, color.z);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

} // namespace

namespace Render3D {

GLRenderer::~GLRenderer()
{
    destroy();
}

GLRenderer::UniformLocations GLRenderer::queryUniformLocations(const Shader& shader)
{
    UniformLocations locations;
    locations.mvp      = shader.getUniform("uMVP");
    locations.model    = shader.getUniform("uModel");
    locations.color    = shader.getUniform("uColor");
    locations.lightDir = shader.getUniform("uLightDirection");
    locations.ambient  = shader.getUniform("uAmbientStrength");
    locations.turnTint = shader.getUniform("uTurnTint");
    return locations;
}

GLRenderer::ExplosionUniformLocations GLRenderer::queryExplosionUniformLocations(const Shader& shader)
{
    ExplosionUniformLocations locations;
    locations.mvp      = shader.getUniform("uMVP");
    locations.model    = shader.getUniform("uModel");
    locations.color    = shader.getUniform("uColor");
    locations.lightDir = shader.getUniform("uLightDirection");
    locations.ambient  = shader.getUniform("uAmbientStrength");
    locations.progress = shader.getUniform("uExplosionProgress");
    return locations;
}

void GLRenderer::initializeCubeGeometry()
{
    const auto& vertices = Primitives::cubeVertices();

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    glBindVertexArray(0);
}

bool GLRenderer::initialize(const std::string& shaderDir)
{
    if (_initialized)
        return true;

    const bool boardLoaded = _boardShader.load(shaderDir + "/board.vs.glsl", shaderDir + "/board.fs.glsl");
    if (!boardLoaded)
    {
        return false;
    }

    _boardUniforms = queryUniformLocations(_boardShader);
    if (!_boardUniforms.isValid())
    {
        return false;
    }

    const bool pieceExplosionLoaded = _pieceExplosionShader.load(shaderDir + "/piece_explosion.vs.glsl", shaderDir + "/piece_explosion.fs.glsl");
    if (pieceExplosionLoaded)
    {
        _pieceExplosionUniforms = queryExplosionUniformLocations(_pieceExplosionShader);
        _pieceExplosionReady    = _pieceExplosionUniforms.isValid();
    }

    initializeCubeGeometry();

    const bool skyboxShaderLoaded = _skyboxShader.load(shaderDir + "/skybox.vs.glsl", shaderDir + "/skybox.fs.glsl");
    if (skyboxShaderLoaded)
    {
        _skyboxUniforms.vp          = _skyboxShader.getUniform("uVP");
        _skyboxUniforms.topColor    = _skyboxShader.getUniform("uTopColor");
        _skyboxUniforms.bottomColor = _skyboxShader.getUniform("uBottomColor");
        _skyboxUniforms.cubemap     = _skyboxShader.getUniform("uSkybox");
        _skyboxUniforms.useCubemap  = _skyboxShader.getUniform("uUseCubemap");
        _skyboxReady                = _skyboxUniforms.isValid();
    }

    _initialized = true;
    return true;
}

void GLRenderer::destroy()
{
    glDeleteBuffers(1, &_vbo);
    _vbo = 0;

    glDeleteVertexArrays(1, &_vao);
    _vao = 0;

    _boardUniforms  = UniformLocations{};
    _pieceExplosionUniforms = ExplosionUniformLocations{};
    _skyboxUniforms = SkyboxUniformLocations{};
    _pieceExplosionReady = false;
    _skyboxReady    = false;
    _initialized    = false;
}

bool GLRenderer::beginBoardPass(PieceColor currentTurn) const
{
    if (!_initialized || !_boardUniforms.isValid() || _vao == 0)
        return false;

    _boardShader.use();

    const glm::vec3 turnTint = (currentTurn == PieceColor::White) ? WHITE_TURN_TINT : BLACK_TURN_TINT;
    glUniform3f(_boardUniforms.turnTint, turnTint.x, turnTint.y, turnTint.z);

    glBindVertexArray(_vao);
    return true;
}

void GLRenderer::setupStaticLighting() const
{
    if (!_boardUniforms.isValid())
        return;

    glUniform3f(_boardUniforms.lightDir, -0.35f, 1.f, 0.25f);
    glUniform1f(_boardUniforms.ambient, 0.30f);
}

void GLRenderer::drawBoard(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition) const
{
    if (!_boardUniforms.isValid())
        return;

    // The main function is now just a clean orchestrator.
    drawBoardGaps(viewProjection, gameSettings);
    drawTiles(viewProjection, board, gameSettings, kirbyPosition);
    drawBoardEdges(viewProjection, gameSettings);
}

void GLRenderer::drawBoardGaps(const glm::mat4& viewProjection, const settings& gameSettings) const
{
    const float boardHalfSize   = (static_cast<float>(Board::SIZE) - 1.f) * 0.5f + BOARD_TILE_SIZE * 0.5f;
    const float fullBoardSize   = boardHalfSize * 2.f;
    const float gapLayerHeight  = gameSettings.boardThickness * 0.4f;
    const float gapLayerTopY    = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f - 0.004f;
    const float gapLayerCenterY = gapLayerTopY - gapLayerHeight * 0.5f;

    const glm::vec3 boardGapColor = gameSettings.getBoardGapColorVec3();
    const glm::mat4 gapLayerModel = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, gapLayerCenterY, 0.f})
                                    * glm::scale(glm::mat4{1.f}, glm::vec3{fullBoardSize, gapLayerHeight, fullBoardSize});

    drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, gapLayerModel, boardGapColor);
}

void GLRenderer::drawTiles(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition) const
{
    // Math specific to the tiles lives here now
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;

    const glm::vec3 whiteTileColor = gameSettings.getWhiteVec3();
    const glm::vec3 blackTileColor = gameSettings.getBlackVec3();
    const glm::vec3 selectedOwnPieceColor{0.20f, 0.45f, 1.f};
    const glm::vec3 availableMoveColor{0.20f, 0.75f, 0.25f};
    const glm::vec3 captureMoveColor{1.f, 0.55f, 0.f};
    
    const glm::mat4 tileScale = glm::scale(glm::mat4{1.f}, glm::vec3{BOARD_TILE_SIZE, gameSettings.boardThickness, BOARD_TILE_SIZE});

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case&     tileCase    = board.getCase(x, y);
            const bool      isWhiteTile = ((x + y) % 2) == 0;
            glm::vec3       tileColor   = isWhiteTile ? whiteTileColor : blackTileColor;
            if (tileCase.isActive())
            {
                if (board.isSelectedCase(x, y))
                {
                    tileColor = selectedOwnPieceColor;
                }
                else if (!tileCase.hasPiece())
                {
                    tileColor = availableMoveColor;
                }
                else
                {
                    tileColor = captureMoveColor;
                }
            }

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, BOARD_CENTER_Y, worldZ}) * tileScale;

            drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, model, tileColor);

            const bool kirbyHere = kirbyPosition.has_value() && kirbyPosition->first == x && kirbyPosition->second == y;
            if (kirbyHere)
            {
                const float     kirbySize  = BOARD_TILE_SIZE * 0.55f;
                const float     kirbyY     = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f + kirbySize * 0.5f;
                const glm::mat4 kirbyModel = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, kirbyY, worldZ})
                                             * glm::scale(glm::mat4{1.f}, glm::vec3{kirbySize, kirbySize, kirbySize});
                drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, kirbyModel, kirbyColor);
            }
        }
    }
}

void GLRenderer::drawBoardEdges(const glm::mat4& viewProjection, const settings& gameSettings) const
{
    const float boardHalfSize = (static_cast<float>(Board::SIZE) - 1.f) * 0.5f + BOARD_TILE_SIZE * 0.5f;

    const float sideThickness = gameSettings.boardSideThickness;
    const float sideHeight    = gameSettings.boardThickness + gameSettings.boardSideDrop;
    const float sideCenterY   = BOARD_CENTER_Y - gameSettings.boardSideDrop * 0.5f;
    const float sideLength    = (boardHalfSize + sideThickness) * 2.f;

    const glm::vec3 sideColor           = gameSettings.getBoardSideColorVec3();
    const glm::mat4 horizontalSideScale = glm::scale(glm::mat4{1.f}, glm::vec3{sideLength, sideHeight, sideThickness});
    const glm::mat4 verticalSideScale   = glm::scale(glm::mat4{1.f}, glm::vec3{sideThickness, sideHeight, sideLength});

    const glm::mat4 northSide = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, sideCenterY, boardHalfSize + sideThickness * 0.5f})
                                * horizontalSideScale;
    const glm::mat4 southSide = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, sideCenterY, -(boardHalfSize + sideThickness * 0.5f)})
                                * horizontalSideScale;
    const glm::mat4 eastSide = glm::translate(glm::mat4{1.f}, glm::vec3{boardHalfSize + sideThickness * 0.5f, sideCenterY, 0.f})
                               * verticalSideScale;
    const glm::mat4 westSide = glm::translate(glm::mat4{1.f}, glm::vec3{-(boardHalfSize + sideThickness * 0.5f), sideCenterY, 0.f})
                               * verticalSideScale;

    drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, northSide, sideColor);
    drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, southSide, sideColor);
    drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, eastSide, sideColor);
    drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, westSide, sideColor);
}

void GLRenderer::drawSinglePiece(const glm::mat4& viewProj, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ,
                                 float topY, const ResourceManager& resourceManager) const
{
    if (piece == nullptr)
        return;

    const std::size_t pieceIndex = static_cast<std::size_t>(piece->type());
    if (pieceIndex >= PIECES_HEIGHT.size())
        return;

    // Gather basic piece info
    const glm::vec3 pieceColor  = colorForPiece(piece);
    const float     pieceHeight = PIECES_HEIGHT[pieceIndex] * 1.35f;
    const float     worldX      = originX + boardX;
    const float     worldZ      = originZ + boardY;

    // Try to get the 3D model
    const ResourceManager::PieceMeshGlData* modelMesh = resourceManager.pieceMeshFor(piece->type());

    if (modelMesh != nullptr && modelMesh->isValid())
    {
        // Path A: Draw the beautiful 3D GLB model
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + yOffset, worldZ})
                                * glm::scale(glm::mat4{1.f}, glm::vec3{pieceHeight, pieceHeight, pieceHeight});

        // drawIndexedMesh already binds its own VAO inside, so we are safe.
        drawIndexedMesh(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProj, model, pieceColor, modelMesh->vao, modelMesh->indexCount);
    }
    else
    {
        // Path B: Fallback to drawing a basic cube
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + pieceHeight * 0.5f + yOffset, worldZ})
                                * glm::scale(glm::mat4{1.f}, glm::vec3{PIECE_BASE_WIDTH, pieceHeight, PIECE_BASE_WIDTH});

        // FIX: Explicitly bind the cube VAO right before we draw it.
        // This completely eliminates the need for the ugly `cubeVaoBound` boolean tracker!
        glBindVertexArray(_vao);
        drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProj, model, pieceColor);
    }
}

void GLRenderer::drawSingleExplodingPiece(const glm::mat4& viewProj, const Piece* piece, float boardX, float boardY, float yOffset, float originX,
                                          float originZ, float topY, float explosionProgress, const ResourceManager& resourceManager) const
{
    if (piece == nullptr || !_pieceExplosionUniforms.isValid())
        return;

    const std::size_t pieceIndex = static_cast<std::size_t>(piece->type());
    if (pieceIndex >= PIECES_HEIGHT.size())
        return;

    const glm::vec3 pieceColor  = colorForPiece(piece);
    const float     pieceHeight = PIECES_HEIGHT[pieceIndex] * 1.35f;
    const float     worldX      = originX + boardX;
    const float     worldZ      = originZ + boardY;
    const float     progress    = std::clamp(explosionProgress, 0.f, 1.f);

    glUniform1f(_pieceExplosionUniforms.progress, progress);

    const ResourceManager::PieceMeshGlData* modelMesh = resourceManager.pieceMeshFor(piece->type());
    if (modelMesh != nullptr && modelMesh->isValid())
    {
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + yOffset, worldZ})
                              * glm::scale(glm::mat4{1.f}, glm::vec3{pieceHeight, pieceHeight, pieceHeight});

        drawIndexedMesh(_pieceExplosionUniforms.mvp, _pieceExplosionUniforms.model, _pieceExplosionUniforms.color,
                        viewProj, model, pieceColor, modelMesh->vao, modelMesh->indexCount);
    }
    else
    {
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + pieceHeight * 0.5f + yOffset, worldZ})
                              * glm::scale(glm::mat4{1.f}, glm::vec3{PIECE_BASE_WIDTH, pieceHeight, PIECE_BASE_WIDTH});

        glBindVertexArray(_vao);
        drawCube(_pieceExplosionUniforms.mvp, _pieceExplosionUniforms.model, _pieceExplosionUniforms.color, viewProj, model, pieceColor);
    }
}

void GLRenderer::drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager,
                            const AnimatedPiecePositions& animatedPiecePositions, const ExplodingPiecePositions& explodingPiecePositions) const
{
    if (!_boardUniforms.isValid())
        return;

    // Calculate the board anchors once
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardTopY    = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f;

    // A clean, readable loop!
    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (currentCase.hasPiece())
            {
                const Piece* piece = currentCase.getPiece();

                float boardX = static_cast<float>(x);
                float boardY = static_cast<float>(y);
                float yOffset = 0.f;

                const auto animatedPosition = animatedPiecePositions.find(piece);
                if (animatedPosition != animatedPiecePositions.end())
                {
                    boardX   = animatedPosition->second.x;
                    boardY   = animatedPosition->second.y;
                    yOffset  = animatedPosition->second.z;
                }

                drawSinglePiece(viewProjection, piece, boardX, boardY, yOffset, boardOriginX, boardOriginZ, boardTopY, resourceManager);
            }
        }
    }

    if (!_pieceExplosionReady || explodingPiecePositions.empty())
        return;

    _pieceExplosionShader.use();
    glUniform3f(_pieceExplosionUniforms.lightDir, -0.35f, 1.f, 0.25f);
    glUniform1f(_pieceExplosionUniforms.ambient, 0.25f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    for (const auto& [piece, explosion] : explodingPiecePositions)
    {
        drawSingleExplodingPiece(viewProjection,
                                 piece,
                                 explosion.position.x,
                                 explosion.position.y,
                                 explosion.position.z,
                                 boardOriginX,
                                 boardOriginZ,
                                 boardTopY,
                                 explosion.progress,
                                 resourceManager);
    }

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void GLRenderer::drawSkybox(const glm::mat4& view, const glm::mat4& projection, const settings& gameSettings, const ResourceManager& resourceManager) const
{
    if (!_skyboxReady || !_skyboxUniforms.isValid() || _vao == 0)
        return;

    const glm::vec3 topColor    = gameSettings.getSkyboxTopColorVec3();
    const glm::vec3 bottomColor = gameSettings.getSkyboxBottomColorVec3();
    const unsigned int cubemapTexture = resourceManager.skyboxCubemap();
    const bool hasCubemapTexture = cubemapTexture != 0;

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    const glm::mat4 skyboxView = glm::mat4{glm::mat3{view}};
    const glm::mat4 skyboxVp   = projection * skyboxView;

    _skyboxShader.use();
    glUniformMatrix4fv(_skyboxUniforms.vp, 1, GL_FALSE, glm::value_ptr(skyboxVp));
    glUniform3f(_skyboxUniforms.topColor, topColor.x, topColor.y, topColor.z);
    glUniform3f(_skyboxUniforms.bottomColor, bottomColor.x, bottomColor.y, bottomColor.z);
    glUniform1i(_skyboxUniforms.cubemap, 0);
    glUniform1i(_skyboxUniforms.useCubemap, hasCubemapTexture ? 1 : 0);

    if (hasCubemapTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    }

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    if (hasCubemapTexture)
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

} // namespace Render3D
