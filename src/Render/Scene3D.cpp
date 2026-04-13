#include "Scene3D.hpp"
#include <glad/glad.h>
#include <cmath>
#include <cstdint>
#include <glm/geometric.hpp>
#include <iostream>
#include <string>


namespace {

constexpr float BOARD_CENTER_Y  = -0.05f;
constexpr float BOARD_TILE_SIZE = 0.94f;
constexpr float EPSILON         = 1e-5f;

} // namespace

Scene3D::~Scene3D()
{
    destroyGlResources();
}

bool Scene3D::initialize(const AppConfig& config)
{
    initializeIfNeeded(config);
    return _initialized;
}

bool Scene3D::prepareRenderState(int width, int height)
{
    if (width <= 0 || height <= 0)
        return false;
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

    return true;
}

void Scene3D::render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition)
{
    if (!prepareRenderState(width, height))
    {
        _hasCameraMatrices = false;
        return;
    }

    if (!_boardRenderer.beginBoardPass(currentTurn))
    {
        _hasCameraMatrices = false;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    const float aspect = static_cast<float>(_framebufferW) / static_cast<float>(_framebufferH);

    _cameraController.updateTarget(board, gameSettings, deltaTimeSeconds);

    glm::mat4       view{1.f};
    glm::mat4       projection{1.f};
    const glm::mat4 viewProjection = _cameraController.calculateViewProjection(gameSettings, aspect, &view, &projection);

    _lastView          = view;
    _lastProjection    = projection;
    _hasCameraMatrices = true;

    _boardRenderer.setupStaticLighting();
    _boardRenderer.drawBoard(viewProjection, board, gameSettings, kirbyPosition);

    if (gameSettings.drawPieces3D)
        _boardRenderer.drawPieces(viewProjection, board, gameSettings, _resourceManager);

    if (gameSettings.drawSkybox)
        _boardRenderer.drawSkybox(view, projection, gameSettings);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImTextureID Scene3D::colorTexture() const
{
    return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(_colorTexture));
}

bool Scene3D::pickBoardTile(const settings& gameSettings, float localX, float localY, float viewportWidth, float viewportHeight, int* outX, int* outY) const
{
    if (outX == nullptr || outY == nullptr)
        return false;
    if (!_hasCameraMatrices)
        return false;
    if (viewportWidth <= 0.f || viewportHeight <= 0.f)
        return false;

    const float u = localX / viewportWidth;
    const float v = localY / viewportHeight;
    if (u < 0.f || u > 1.f || v < 0.f || v > 1.f)
        return false;

    const float ndcX = u * 2.f - 1.f;
    const float ndcY = 1.f - v * 2.f;

    const glm::mat4 inverseViewProjection = glm::inverse(_lastProjection * _lastView);
    glm::vec4       worldNear             = inverseViewProjection * glm::vec4{ndcX, ndcY, -1.f, 1.f};
    glm::vec4       worldFar              = inverseViewProjection * glm::vec4{ndcX, ndcY, 1.f, 1.f};

    if (std::abs(worldNear.w) < EPSILON || std::abs(worldFar.w) < EPSILON)
        return false;

    worldNear /= worldNear.w;
    worldFar /= worldFar.w;

    const glm::vec3 rayOrigin{worldNear.x, worldNear.y, worldNear.z};
    const glm::vec3 rayVector{worldFar.x - worldNear.x, worldFar.y - worldNear.y, worldFar.z - worldNear.z};
    const float     rayLength = glm::length(rayVector);
    if (rayLength < EPSILON)
        return false;

    const glm::vec3 rayDirection = rayVector / rayLength;
    if (std::abs(rayDirection.y) < EPSILON)
        return false;

    const float boardTopY = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f;
    const float t         = (boardTopY - rayOrigin.y) / rayDirection.y;
    if (t < 0.f)
        return false;

    const glm::vec3 hitPoint = rayOrigin + rayDirection * t;

    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;

    const float boardX = hitPoint.x - boardOriginX;
    const float boardY = hitPoint.z - boardOriginZ;

    const int tileX = static_cast<int>(std::floor(boardX + 0.5f));
    const int tileY = static_cast<int>(std::floor(boardY + 0.5f));

    if (tileX < 0 || tileX >= Board::SIZE || tileY < 0 || tileY >= Board::SIZE)
        return false;

    const float tileCenterX = boardOriginX + static_cast<float>(tileX);
    const float tileCenterZ = boardOriginZ + static_cast<float>(tileY);
    const float tileHalf    = BOARD_TILE_SIZE * 0.5f;

    if (std::abs(hitPoint.x - tileCenterX) > tileHalf || std::abs(hitPoint.z - tileCenterZ) > tileHalf)
        return false;

    *outX = tileX;
    *outY = tileY;
    return true;
}

void Scene3D::initializeIfNeeded(const AppConfig& config)
{
    if (_initialized)
        return;

    // 1. Give the Resource Manager its assets path! (Fixes the compile error)
    if (!_resourceManager.initialize(config.assetPath))
    {
        std::cout << "Failed to initialize resources from: " << config.assetPath << "\n";
        return;
    }

    // 2. Give the Geometry Drawer (BoardRenderer) its shaders path! (Removes the macro hack)
    if (!_boardRenderer.initialize(config.shaderPath))
    {
        std::cout << "Failed to initialize 3D board shaders from: " << config.shaderPath << "\n";
        _resourceManager.destroy();
        return;
    }

    _initialized = true;
}

void Scene3D::ensureFramebufferSize(int width, int height)
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

void Scene3D::destroyFramebuffer()
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

void Scene3D::destroyGlResources()
{
    destroyFramebuffer();

    _boardRenderer.destroy();
    _resourceManager.destroy();
    _cameraController.reset();

    _framebufferW      = 0;
    _framebufferH      = 0;
    _hasCameraMatrices = false;
    _initialized       = false;
}
