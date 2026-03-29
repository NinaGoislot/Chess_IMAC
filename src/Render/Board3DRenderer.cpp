#include "Board3DRenderer.hpp"

#include "3D/Primitives.hpp"
#include "Board/Board.hpp"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>

#include <glad/glad.h>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

namespace {

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

    _activeShader = (currentTurn == PieceColor::White) ? &_whiteTurnShader : &_blackTurnShader;
    _activeShader->use();
    refreshUniformLocations();

    if (_mvpLoc < 0 || _modelLoc < 0 || _colorLoc < 0 || _lightDirLoc < 0 || _ambientLoc < 0)
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
    glUniform3f(_lightDirLoc, -0.35f, 1.f, 0.25f);
    glUniform1f(_ambientLoc, 0.30f);
}

void Board3DRenderer::drawBoardTiles(const glm::mat4& viewProjection, const settings& gameSettings) const
{
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const bool isWhiteTile = ((x + y) % 2) == 0;
            const glm::vec3 tileColor = isWhiteTile ? imToVec3(gameSettings.getWhite()) : imToVec3(gameSettings.getBlack());

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, -0.05f, worldZ})
                                  * glm::scale(glm::mat4{1.f}, glm::vec3{0.94f, gameSettings.boardThickness, 0.94f});

            drawCube(_mvpLoc, _modelLoc, _colorLoc, viewProjection, model, tileColor);
        }
    }
}

void Board3DRenderer::drawPieces(const glm::mat4& viewProjection, const Board& board, const settings& gameSettings) const
{
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardTopY    = -0.05f + gameSettings.boardThickness * 0.5f;
    const float pieceLiftY   = 0.01f;

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (!currentCase.hasPiece())
                continue;

            const Piece* piece = currentCase.getPiece();
            const float pieceHeight = heightForPiece(piece->type());

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 pieceModel = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, boardTopY + pieceLiftY + pieceHeight * 0.5f, worldZ})
                                       * glm::scale(glm::mat4{1.f}, glm::vec3{0.45f, pieceHeight, 0.45f});

            drawCube(_mvpLoc, _modelLoc, _colorLoc, viewProjection, pieceModel, colorForPiece(piece));
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

void Board3DRenderer::refreshUniformLocations()
{
    if (_activeShader == nullptr)
        return;

    _mvpLoc      = _activeShader->getUniform("uMVP");
    _modelLoc    = _activeShader->getUniform("uModel");
    _colorLoc    = _activeShader->getUniform("uColor");
    _lightDirLoc = _activeShader->getUniform("uLightDirection");
    _ambientLoc  = _activeShader->getUniform("uAmbientStrength");
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

void Board3DRenderer::destroyGlResources()
{
    destroyFramebuffer();

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
