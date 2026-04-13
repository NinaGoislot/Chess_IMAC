#pragma once

#include <imgui.h>
#include <glm/mat4x4.hpp>

#include "utilities/AppConfig.hpp"
#include "Board/Board.hpp"
#include "Game/settings.hpp"
#include "3D/GLRenderer.hpp"
#include "3D/CameraController.hpp"
#include "3D/PieceAnimator.hpp"
#include "3D/ResourceManager.hpp"

// -------- EXPLANATION --------
// Scene3D is the "Director". It manages the framebuffer, the camera, and the rendering of the board.
// Scene3D does not actually draw anything itself

class Scene3D
{
public:
    // constructor and destructor
    Scene3D() = default;
    ~Scene3D();

    // non-copyable
    Scene3D(const Scene3D&)            = delete;
    Scene3D& operator=(const Scene3D&) = delete;

    // main render function and texture
    bool initialize(const AppConfig& config);
    void render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height, float deltaTimeSeconds);
    bool pickBoardTile(const settings& gameSettings, float localX, float localY, float viewportWidth, float viewportHeight, int* outX, int* outY) const;
    ImTextureID colorTexture() const;

private:
    // initialization and cleanup functions
    void initializeIfNeeded(const AppConfig& config);
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

    Render3D::GLRenderer   _boardRenderer;
    Render3D::CameraController _cameraController;
    Render3D::PieceAnimator _pieceAnimator;
    Render3D::ResourceManager _resourceManager;

    glm::mat4 _lastView{1.f};
    glm::mat4 _lastProjection{1.f};
    bool      _hasCameraMatrices = false;
};
