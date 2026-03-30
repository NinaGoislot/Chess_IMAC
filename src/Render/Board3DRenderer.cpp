#include "Board3DRenderer.hpp"

#include "3D/modelLoader.hpp"
#include "3D/Primitives.hpp"
#include "Board/Board.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

namespace {

constexpr std::size_t kPieceTypeCount = 6u;

std::size_t pieceTypeIndex(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn:
        return 0u;
    case PieceType::Rook:
        return 1u;
    case PieceType::Knight:
        return 2u;
    case PieceType::Bishop:
        return 3u;
    case PieceType::Queen:
        return 4u;
    case PieceType::King:
        return 5u;
    }

    return 0u;
}

std::string pieceModelName(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn:
        return "pawn";
    case PieceType::Rook:
        return "rook";
    case PieceType::Knight:
        return "knight";
    case PieceType::Bishop:
        return "bishop";
    case PieceType::Queen:
        return "queen";
    case PieceType::King:
        return "king";
    }

    return "pawn";
}

std::vector<std::string> modelCandidates(PieceType type)
{
    const std::string modelName = pieceModelName(type);
    const std::array<std::string, 5> roots = {
        "assets/models",
        "../assets/models",
        "../../assets/models",
        "bin/assets/models",
        "../../../assets/models",
    };

    std::vector<std::string> candidates;
    candidates.reserve(roots.size());

    for (const std::string& root : roots)
        candidates.push_back(root + "/" + modelName + ".glb");

    return candidates;
}

glm::vec3 imToVec3(const ImVec4& color)
{
    return glm::vec3{color.x, color.y, color.z};
}

glm::vec3 colorForPiece(const Piece* piece)
{
    if (piece == nullptr)
        return glm::vec3{0.f, 0.f, 0.f};

    if (piece->color() == PieceColor::White)
        return glm::vec3{0.9f, 0.9f, 0.9f};

    return glm::vec3{0.15f, 0.15f, 0.2f};
}

float heightForPiece(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn:
        return 0.6f;
    case PieceType::Knight:
        return 0.8f;
    case PieceType::Bishop:
        return 0.9f;
    case PieceType::Rook:
        return 0.95f;
    case PieceType::Queen:
        return 1.1f;
    case PieceType::King:
        return 1.2f;
    }

    return 0.8f;
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

Board3DRenderer::~Board3DRenderer()
{
    destroyGlResources();
}

bool Board3DRenderer::prepareRenderState(int width, int height, PieceColor currentTurn)
{
    if (width <= 0 || height <= 0)
        return false;

    initializeIfNeeded();
    if (!_initialized)
        return false;

    ensureFramebufferSize(width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _framebufferW, _framebufferH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.08f, 0.08f, 0.10f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentTurn == PieceColor::White)
    {
        _activeShader = &_whiteTurnShader;
        _activeUniforms = &_whiteUniforms;
    }
    else
    {
        _activeShader = &_blackTurnShader;
        _activeUniforms = &_blackUniforms;
    }

    _activeShader->use();

    if (_activeUniforms == nullptr || !_activeUniforms->isValid())
        return false;

    glBindVertexArray(_vao);

    return true;
}

glm::vec3 Board3DRenderer::calculatePieceTarget(const Board& board) const
{
    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (currentCase.hasPiece())
            {
                const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
                const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
                return glm::vec3{boardOriginX + static_cast<float>(x), 0.2f, boardOriginZ + static_cast<float>(y)};
            }
        }
    }

    return glm::vec3{0.f, 0.2f, 0.f};
}

glm::mat4 Board3DRenderer::calculateCameraViewProjection(const settings& gameSettings, float aspect,const glm::vec3& target) const
{
    const glm::mat4 projection = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.f);

    const float yawRadians   = glm::radians(gameSettings.cameraYawDegrees);
    const float pitchRadians = glm::radians(gameSettings.cameraPitchDegrees);

        const glm::vec3 eye{
        target.x + gameSettings.cameraDistance * std::cos(pitchRadians) * std::cos(yawRadians),
        target.y + gameSettings.cameraDistance * std::sin(pitchRadians),
        target.z + gameSettings.cameraDistance * std::cos(pitchRadians) * std::sin(yawRadians),
    };

    const glm::mat4 view = glm::lookAt(eye, target, glm::vec3{0.f, 1.f, 0.f});
    return projection * view;
}



void Board3DRenderer::setupStaticLighting() const
{
    if (_activeUniforms == nullptr)
        return;

    glUniform3f(_activeUniforms->lightDir, -0.35f, 1.f, 0.25f);
    glUniform1f(_activeUniforms->ambient, 0.30f);
}

void Board3DRenderer::drawBoardTiles(const glm::mat4& viewProjection, const settings& gameSettings) const
{
    if (_activeUniforms == nullptr)
        return;

    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const glm::vec3 whiteTileColor = imToVec3(gameSettings.getWhite());
    const glm::vec3 blackTileColor = imToVec3(gameSettings.getBlack());

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const bool isWhiteTile = ((x + y) % 2) == 0;
            const glm::vec3 tileColor = isWhiteTile ? whiteTileColor : blackTileColor;

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, -0.05f, worldZ})
                                  * glm::scale(glm::mat4{1.f}, glm::vec3{0.94f, gameSettings.boardThickness, 0.94f});

            drawCube(_activeUniforms->mvp, _activeUniforms->model, _activeUniforms->color, viewProjection, model, tileColor);
        }
    }
}

void Board3DRenderer::drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings) const
{
    if (_activeUniforms == nullptr)
        return;

    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardTopY    = -0.05f + gameSettings.boardThickness * 0.5f;
    const float pieceLiftY   = 0.01f;
    bool        cubeVaoBound = true;

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (!currentCase.hasPiece())
                continue;

            const Piece* piece = currentCase.getPiece();
            const glm::vec3 pieceColor = colorForPiece(piece);
            const float pieceHeight = heightForPiece(piece->type());

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 pieceModel = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, boardTopY + pieceLiftY + pieceHeight * 0.5f, worldZ})
                                       * glm::scale(glm::mat4{1.f}, glm::vec3{0.45f, pieceHeight, 0.45f});

            const PieceMeshGlData* modelMesh = pieceMeshFor(piece->type());
            if (modelMesh != nullptr && modelMesh->isValid())
            {
                cubeVaoBound = false;
                drawIndexedMesh(_activeUniforms->mvp, _activeUniforms->model, _activeUniforms->color, viewProjection, pieceModel, pieceColor, modelMesh->vao, modelMesh->indexCount);
            }
            else
            {
                if (!cubeVaoBound)
                {
                    glBindVertexArray(_vao);
                    cubeVaoBound = true;
                }
                drawCube(_activeUniforms->mvp, _activeUniforms->model, _activeUniforms->color, viewProjection, pieceModel, pieceColor);
            }
        }
    }
}

void Board3DRenderer::render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height)
{
    if (!prepareRenderState(width, height, currentTurn))
        return;

    const float aspect = static_cast<float>(_framebufferW) / static_cast<float>(_framebufferH);
    
    glm::vec3 target{0.f, 0.2f, 0.f};
    if(gameSettings.cameraPieceTarget) {
        target = calculatePieceTarget(board);
    }

    const glm::mat4 viewProjection = calculateCameraViewProjection(gameSettings, aspect, target);

    setupStaticLighting();
    drawBoardTiles(viewProjection, gameSettings);

    if (gameSettings.drawPieces3D)
        drawPieces(viewProjection, board, gameSettings);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImTextureID Board3DRenderer::colorTexture() const
{
    return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(_colorTexture));
}

Board3DRenderer::UniformLocations Board3DRenderer::queryUniformLocations(const Shader& shader)
{
    UniformLocations locations;
    locations.mvp      = shader.getUniform("uMVP");
    locations.model    = shader.getUniform("uModel");
    locations.color    = shader.getUniform("uColor");
    locations.lightDir = shader.getUniform("uLightDirection");
    locations.ambient  = shader.getUniform("uAmbientStrength");
    return locations;
}

const Board3DRenderer::PieceMeshGlData* Board3DRenderer::pieceMeshFor(PieceType type) const
{
    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size())
        return nullptr;

    return &_pieceMeshes[index];
}

bool Board3DRenderer::uploadPieceMesh(PieceType type, const ModelMeshData& meshData)
{
    if (meshData.vertices.empty() || meshData.indices.empty())
        return false;

    PieceMeshGlData& mesh = _pieceMeshes[pieceTypeIndex(type)];

    if (mesh.ebo != 0)
        glDeleteBuffers(1, &mesh.ebo);
    if (mesh.vbo != 0)
        glDeleteBuffers(1, &mesh.vbo);
    if (mesh.vao != 0)
        glDeleteVertexArrays(1, &mesh.vao);

    mesh = PieceMeshGlData{};

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(meshData.vertices.size() * sizeof(float)),
        meshData.vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(meshData.indices.size() * sizeof(uint32_t)),
        meshData.indices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

    mesh.indexCount = static_cast<int>(meshData.indices.size());

    glBindVertexArray(0);
    return mesh.isValid();
}

void Board3DRenderer::initializePieceModels()
{
    for (std::size_t i = 0u; i < kPieceTypeCount; ++i)
    {
        const PieceType type = static_cast<PieceType>(i);

        ModelMeshData meshData;
        bool          loaded = false;
        std::string   loadedFrom;

        for (const std::string& candidate : modelCandidates(type))
        {
            if (!std::filesystem::exists(candidate))
                continue;

            if (loadGLBMesh(candidate, meshData))
            {
                loaded = true;
                loadedFrom = candidate;
                break;
            }
        }

        if (!loaded)
        {
            std::cout << "No GLB model found for piece type '" << pieceModelName(type)
                      << "'. Falling back to cube rendering for this piece.\n";
            continue;
        }

        if (!uploadPieceMesh(type, meshData))
        {
            std::cout << "Failed to upload GLB mesh for piece type '" << pieceModelName(type)
                      << "' from: " << loadedFrom << "\n";
            continue;
        }

        std::cout << "Loaded GLB model for piece type '" << pieceModelName(type)
                  << "' from: " << loadedFrom << "\n";
    }
}

void Board3DRenderer::initializeIfNeeded()
{
    if (_initialized)
        return;

#ifdef SHADER_DIR
    const std::string shaderDir = SHADER_DIR;
#else
    const std::string shaderDir = "shaders";
#endif

    const bool whiteLoaded = _whiteTurnShader.load(shaderDir + "/board.vs.glsl", shaderDir + "/board_white_turn.fs.glsl");
    const bool blackLoaded = _blackTurnShader.load(shaderDir + "/board.vs.glsl", shaderDir + "/board_black_turn.fs.glsl");
    if (!whiteLoaded || !blackLoaded)
    {
        std::cout << "Failed to load board shaders from: " << shaderDir << "\n";
        return;
    }

    _whiteUniforms = queryUniformLocations(_whiteTurnShader);
    _blackUniforms = queryUniformLocations(_blackTurnShader);
    if (!_whiteUniforms.isValid() || !_blackUniforms.isValid())
    {
        std::cout << "Failed to find one or more shader uniforms in board shaders.\n";
        return;
    }

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

    initializePieceModels();

    glBindVertexArray(0);

    _initialized = true;
}

void Board3DRenderer::ensureFramebufferSize(int width, int height)
{
    if (_fbo != 0 && _framebufferW == width && _framebufferH == height)
        return;

    destroyFramebuffer();
    _framebufferW = width;
    _framebufferH = height;

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_colorTexture);
    glBindTexture(GL_TEXTURE_2D, _colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _framebufferW, _framebufferH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTexture, 0);

    glGenRenderbuffers(1, &_depthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _framebufferW, _framebufferH);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencil);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "3D board framebuffer is not complete.\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Board3DRenderer::destroyFramebuffer()
{
    if (_depthStencil != 0)
    {
        glDeleteRenderbuffers(1, &_depthStencil);
        _depthStencil = 0;
    }

    if (_colorTexture != 0)
    {
        glDeleteTextures(1, &_colorTexture);
        _colorTexture = 0;
    }

    if (_fbo != 0)
    {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
}

void Board3DRenderer::destroyPieceMeshes()
{
    for (PieceMeshGlData& mesh : _pieceMeshes)
    {
        if (mesh.ebo != 0)
        {
            glDeleteBuffers(1, &mesh.ebo);
            mesh.ebo = 0;
        }

        if (mesh.vbo != 0)
        {
            glDeleteBuffers(1, &mesh.vbo);
            mesh.vbo = 0;
        }

        if (mesh.vao != 0)
        {
            glDeleteVertexArrays(1, &mesh.vao);
            mesh.vao = 0;
        }

        mesh.indexCount = 0;
    }
}

void Board3DRenderer::destroyGlResources()
{
    destroyFramebuffer();
    destroyPieceMeshes();

    if (_vbo != 0)
    {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }

    if (_vao != 0)
    {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }

    _initialized = false;
}
