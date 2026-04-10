#pragma once

#include <imgui.h>
#include "Board/Board.hpp"
#include "Game/settings.hpp"
#include "3D/BoardRenderer.hpp"
#include "3D/CameraController.hpp"
#include "3D/ResourceManager.hpp"


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
    bool initialize();
    void render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height, float deltaTimeSeconds);
    ImTextureID colorTexture() const;

private:
    // initialization and cleanup functions
    void initializeIfNeeded();
    bool prepareRenderState(int width, int height);
    void ensureFramebufferSize(int width, int height);
    void destroyFramebuffer();
    void destroyGlResources();

    // OpenGL resources
    unsigned int _fbo           = 0;
    unsigned int _colorTexture  = 0;
    unsigned int _depthStencil  = 0;
    int          _framebufferW  = 0;
    int          _framebufferH  = 0;
    bool         _initialized   = false;

    Render3D::BoardRenderer   _boardRenderer;
    Render3D::CameraController _cameraController;
    Render3D::ResourceManager _resourceManager;
};
