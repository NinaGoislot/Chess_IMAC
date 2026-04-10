#include "Board3DRenderer.hpp"
#include <glad/glad.h>
#include <cstdint>
#include <iostream>
#include <string>

Board3DRenderer::~Board3DRenderer()
{
    destroyGlResources();
}

bool Board3DRenderer::initialize()
{
    initializeIfNeeded();
    return _initialized;
}

bool Board3DRenderer::prepareRenderState(int width, int height)
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

void Board3DRenderer::render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height, float deltaTimeSeconds)
{
    if (!prepareRenderState(width, height))
        return;

    if (!_boardRenderer.beginBoardPass(currentTurn))
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    const float aspect = static_cast<float>(_framebufferW) / static_cast<float>(_framebufferH);

    _cameraController.updateTarget(board, gameSettings, deltaTimeSeconds);

    glm::mat4       view{1.f};
    glm::mat4       projection{1.f};
    const glm::mat4 viewProjection = _cameraController.calculateViewProjection(gameSettings, aspect, &view, &projection);

    _boardRenderer.setupStaticLighting();
    _boardRenderer.drawBoardTiles(viewProjection, gameSettings);

    if (gameSettings.drawPieces3D)
        _boardRenderer.drawPieces(viewProjection, board, gameSettings, _resourceManager);

    if (gameSettings.drawSkybox)
        _boardRenderer.drawSkybox(view, projection, gameSettings);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImTextureID Board3DRenderer::colorTexture() const
{
    return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(_colorTexture));
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

    (void)_resourceManager.initialize();

    if (!_boardRenderer.initialize(shaderDir))
    {
        std::cout << "Failed to initialize 3D board shaders from: " << shaderDir << "\n";
        _resourceManager.destroy();
        return;
    }

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

    _boardRenderer.destroy();
    _resourceManager.destroy();
    _cameraController.reset();

    _framebufferW          = 0;
    _framebufferH          = 0;
    _initialized           = false;
}
