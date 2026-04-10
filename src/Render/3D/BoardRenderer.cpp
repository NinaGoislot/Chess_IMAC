#include "BoardRenderer.hpp"
#include <glad/glad.h>
#include <array>
#include <cstddef>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Primitives.hpp"
#include "ResourceManager.hpp"
#include "Board/Board.hpp"

namespace {

constexpr std::size_t kPieceTypeCount = 6u;

constexpr float kBoardTileSize  = 0.94f;
constexpr float kBoardCenterY   = -0.05f;
constexpr float kPieceLiftY     = 0.01f;
constexpr float kPieceBaseWidth = 0.45f;

constexpr std::array<float, kPieceTypeCount> kPieceHeights = {
    0.6f,
    0.95f,
    0.8f,
    0.9f,
    1.1f,
    1.2f,
};

const glm::vec3 kWhiteTurnTint{1.08f, 1.06f, 1.00f};
const glm::vec3 kBlackTurnTint{0.88f, 0.94f, 1.08f};

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

BoardRenderer::~BoardRenderer()
{
    destroy();
}

BoardRenderer::UniformLocations BoardRenderer::queryUniformLocations(const Shader& shader)
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

void BoardRenderer::initializeCubeGeometry()
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

bool BoardRenderer::initialize(const std::string& shaderDir)
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

    initializeCubeGeometry();

    const bool skyboxShaderLoaded = _skyboxShader.load(shaderDir + "/skybox.vs.glsl", shaderDir + "/skybox.fs.glsl");
    if (skyboxShaderLoaded)
    {
        _skyboxUniforms.vp           = _skyboxShader.getUniform("uVP");
        _skyboxUniforms.topColor     = _skyboxShader.getUniform("uTopColor");
        _skyboxUniforms.bottomColor  = _skyboxShader.getUniform("uBottomColor");
        _skyboxReady                 = _skyboxUniforms.isValid();
    }

    _initialized = true;
    return true;
}

void BoardRenderer::destroy()
{
    glDeleteBuffers(1, &_vbo);
    _vbo = 0;

    glDeleteVertexArrays(1, &_vao);
    _vao = 0;

    _boardUniforms  = UniformLocations{};
    _skyboxUniforms = SkyboxUniformLocations{};
    _skyboxReady    = false;
    _initialized    = false;
}

bool BoardRenderer::beginBoardPass(PieceColor currentTurn) const
{
    if (!_initialized || !_boardUniforms.isValid() || _vao == 0)
        return false;

    _boardShader.use();

    const glm::vec3 turnTint = (currentTurn == PieceColor::White) ? kWhiteTurnTint : kBlackTurnTint;
    glUniform3f(_boardUniforms.turnTint, turnTint.x, turnTint.y, turnTint.z);

    glBindVertexArray(_vao);
    return true;
}

void BoardRenderer::setupStaticLighting() const
{
    if (!_boardUniforms.isValid())
        return;

    glUniform3f(_boardUniforms.lightDir, -0.35f, 1.f, 0.25f);
    glUniform1f(_boardUniforms.ambient, 0.30f);
}

void BoardRenderer::drawBoardTiles(const glm::mat4& viewProjection, const settings& gameSettings) const
{
    if (!_boardUniforms.isValid())
        return;

    const float     tileSize       = kBoardTileSize;
    const float     boardCenterY   = kBoardCenterY;
    const float     boardOriginX   = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float     boardOriginZ   = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float     boardHalfSize  = (static_cast<float>(Board::SIZE) - 1.f) * 0.5f + tileSize * 0.5f;
    const glm::vec3 whiteTileColor = gameSettings.getWhiteVec3();
    const glm::vec3 blackTileColor = gameSettings.getBlackVec3();
    const glm::vec3 boardGapColor  = gameSettings.getBoardGapColorVec3();
    const glm::mat4 tileScale      = glm::scale(glm::mat4{1.f}, glm::vec3{tileSize, gameSettings.boardThickness, tileSize});

    const float     fullBoardSize   = boardHalfSize * 2.f;
    const float     gapLayerHeight  = gameSettings.boardThickness * 0.4f;
    const float     gapLayerTopY    = boardCenterY + gameSettings.boardThickness * 0.5f - 0.004f;
    const float     gapLayerCenterY = gapLayerTopY - gapLayerHeight * 0.5f;
    const glm::mat4 gapLayerModel   = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, gapLayerCenterY, 0.f})
                                    * glm::scale(glm::mat4{1.f}, glm::vec3{fullBoardSize, gapLayerHeight, fullBoardSize});

    drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, gapLayerModel, boardGapColor);

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const bool      isWhiteTile = ((x + y) % 2) == 0;
            const glm::vec3 tileColor   = isWhiteTile ? whiteTileColor : blackTileColor;

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, boardCenterY, worldZ}) * tileScale;

            drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, model, tileColor);
        }
    }

    const float     sideThickness       = gameSettings.boardSideThickness;
    const float     sideHeight          = gameSettings.boardThickness + gameSettings.boardSideDrop;
    const float     sideCenterY         = boardCenterY - gameSettings.boardSideDrop * 0.5f;
    const float     sideLength          = (boardHalfSize + sideThickness) * 2.f;
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

void BoardRenderer::drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager) const
{
    if (!_boardUniforms.isValid())
        return;

    const float                            boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float                            boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float                            boardTopY    = kBoardCenterY + gameSettings.boardThickness * 0.5f;
    std::array<glm::mat4, kPieceTypeCount> cubePieceScales{};
    std::array<glm::mat4, kPieceTypeCount> meshPieceScales{};

    for (std::size_t i = 0u; i < kPieceTypeCount; ++i)
    {
        const float pieceHeight = kPieceHeights[i];
        cubePieceScales[i]      = glm::scale(glm::mat4{1.f}, glm::vec3{kPieceBaseWidth, pieceHeight, kPieceBaseWidth});
        meshPieceScales[i]      = glm::scale(glm::mat4{1.f}, glm::vec3{pieceHeight, pieceHeight, pieceHeight});
    }

    bool cubeVaoBound = true;

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (!currentCase.hasPiece())
                continue;

            const Piece* piece = currentCase.getPiece();
            if (piece == nullptr)
                continue;

            const std::size_t pieceIndex = pieceTypeIndex(piece->type());
            if (pieceIndex >= kPieceTypeCount)
                continue;

            const glm::vec3 pieceColor  = colorForPiece(piece);
            const float     pieceHeight = kPieceHeights[pieceIndex];

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 cubePieceModel = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, boardTopY + kPieceLiftY + pieceHeight * 0.5f, worldZ})
                                             * cubePieceScales[pieceIndex];

            const glm::mat4 meshPieceModel = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, boardTopY + kPieceLiftY, worldZ})
                                             * meshPieceScales[pieceIndex];

            const ResourceManager::PieceMeshGlData* modelMesh = resourceManager.pieceMeshFor(piece->type());
            if (modelMesh != nullptr && modelMesh->isValid())
            {
                cubeVaoBound = false;
                drawIndexedMesh(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, meshPieceModel, pieceColor, modelMesh->vao, modelMesh->indexCount);
            }
            else
            {
                if (!cubeVaoBound)
                {
                    glBindVertexArray(_vao);
                    cubeVaoBound = true;
                }
                drawCube(_boardUniforms.mvp, _boardUniforms.model, _boardUniforms.color, viewProjection, cubePieceModel, pieceColor);
            }
        }
    }
}

void BoardRenderer::drawSkybox(const glm::mat4& view, const glm::mat4& projection, const settings& gameSettings) const
{
    if (!_skyboxReady || !_skyboxUniforms.isValid() || _vao == 0)
        return;

    const glm::vec3    topColor       = gameSettings.getSkyboxTopColorVec3();
    const glm::vec3    bottomColor    = gameSettings.getSkyboxBottomColorVec3();

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    const glm::mat4 skyboxView = glm::mat4{glm::mat3{view}};
    const glm::mat4 skyboxVp   = projection * skyboxView;

    _skyboxShader.use();
    glUniformMatrix4fv(_skyboxUniforms.vp, 1, GL_FALSE, glm::value_ptr(skyboxVp));
    glUniform3f(_skyboxUniforms.topColor, topColor.x, topColor.y, topColor.z);
    glUniform3f(_skyboxUniforms.bottomColor, bottomColor.x, bottomColor.y, bottomColor.z);

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

} // namespace Render3D
