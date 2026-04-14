#pragma once

#include <imgui.h>
#include <glm/mat4x4.hpp>
#include <optional>
#include <utility>
#include "3D/CameraController.hpp"
#include "3D/GLRenderer.hpp"
#include "3D/PieceAnimator.hpp"
#include "3D/ResourceManager.hpp"
#include "Board/Board.hpp"
#include "Game/State/SelectionState.hpp"
#include "Game/settings.hpp"
#include "utilities/AppConfig.hpp"

// Coordinates full 3D rendering flow: framebuffer, camera, resources, and draw calls.

class Scene3D {
public:
    // Constructors
    Scene3D() = default;
    ~Scene3D();

    // non-copyable
    Scene3D(const Scene3D&)            = delete;
    Scene3D& operator=(const Scene3D&) = delete;

    // Init function: loads shaders/models and prepares renderer subsystems.
    bool        initialize(const AppConfig& config);
    // Render function: draws one 3D frame of the board and pieces.
    void        render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height, float deltaTimeSeconds,
                       std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection);
    // Performs board tile picking from viewport coordinates.
    bool        pickBoardTile(const settings& gameSettings, float localX, float localY, float viewportWidth, float viewportHeight, int* outX, int* outY) const;
    ImTextureID getColorTexture() const;

private:
    // Internal init/cleanup helpers.
    void initializeIfNeeded(const AppConfig& config);
    bool prepareRenderState(int width, int height);
    void ensureFramebufferSize(int width, int height);
    void destroyFramebuffer();
    void destroyGlResources();

    // OpenGL framebuffer resources.
    unsigned int _fbo          = 0;
    unsigned int _colorTexture = 0;
    unsigned int _depthStencil = 0;
    int          _framebufferW = 0;
    int          _framebufferH = 0;
    bool         _initialized  = false;

    // 3D rendering subsystems.
    Render3D::GLRenderer       _boardRenderer;
    Render3D::CameraController _cameraController;
    Render3D::PieceAnimator _pieceAnimator;
    Render3D::ResourceManager  _resourceManager;

    // Last camera matrices used for picking and post-processing.
    glm::mat4 _lastView{1.f};
    glm::mat4 _lastProjection{1.f};
    bool      _hasCameraMatrices = false;
};
