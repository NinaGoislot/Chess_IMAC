#include "Scene3D.hpp"
#include <cstdint>
#include <iostream>

namespace {

constexpr glm::vec3 TOP_LIGHT_DIRECTION{-0.22f, 1.f, 0.18f};
constexpr glm::vec3 TOP_LIGHT_COLOR{1.00f, 0.98f, 0.95f};

// About 65 degrees from board normal (about 25 degrees above board plane)
// to emphasize the active side.
constexpr glm::vec3 WHITE_SIDE_LIGHT_DIRECTION{0.f, 0.42f, 0.91f};
constexpr glm::vec3 BLACK_SIDE_LIGHT_DIRECTION{0.f, 0.42f, -0.91f};

constexpr glm::vec3 WHITE_SIDE_TINT{1.00f, 0.86f, 0.36f};
constexpr glm::vec3 BLACK_SIDE_TINT{0.74f, 0.52f, 1.00f};

Render3D::BoardLighting makeBoardLighting(PieceColor currentTurn)
{
    Render3D::BoardLighting lighting;
    lighting.topLightDirection = TOP_LIGHT_DIRECTION;
    lighting.topLightColor = TOP_LIGHT_COLOR;
    lighting.topLightStrength = 0.72f;
    lighting.ambientStrength = 0.23f;

    const bool useWhiteSideLight = (currentTurn == PieceColor::White);
    lighting.sideLightDirection = useWhiteSideLight ? WHITE_SIDE_LIGHT_DIRECTION : BLACK_SIDE_LIGHT_DIRECTION;
    lighting.sideLightColor = useWhiteSideLight ? WHITE_SIDE_TINT : BLACK_SIDE_TINT;
    lighting.sideLightStrength = 0.62f;
    return lighting;
}

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

    return _glRenderer.beginFrame(width, height);
}

void Scene3D::render(const Board& board, const settings& gameSettings, PieceColor currentTurn, int width, int height, float deltaTimeSeconds,
                     std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection)
{
    if (!prepareRenderState(width, height))
    {
        _cameraController.clearMatrices();
        return;
    }

    const float aspect = static_cast<float>(_glRenderer.framebufferWidth()) / static_cast<float>(_glRenderer.framebufferHeight());

    _cameraController.updateTarget(board, gameSettings, selection, deltaTimeSeconds);

    glm::mat4       view{1.f};
    glm::mat4       projection{1.f};
    const glm::mat4 viewProjection = _cameraController.calculateViewProjection(gameSettings, aspect, &view, &projection);

    _cameraController.storeMatrices(view, projection);

    const bool boardPassReady = _glRenderer.beginBoardPass();
    if (boardPassReady)
    {
        _glRenderer.setBoardLighting(makeBoardLighting(currentTurn));
        _chessSceneRenderer.drawBoard(_glRenderer, viewProjection, board, gameSettings, _resourceManager, currentTurn, kirbyPosition, selection);
    }

    _pieceAnimator.update(board, gameSettings, deltaTimeSeconds);

    if (gameSettings.drawPieces3D && boardPassReady)
        _chessSceneRenderer.drawPieces(_glRenderer, viewProjection, board, gameSettings, _resourceManager, currentTurn, selection, _pieceAnimator.getPositions(), _pieceAnimator.getExplosions());

    if (gameSettings.drawSkybox)
        _glRenderer.drawSkybox(view, projection, gameSettings, _resourceManager);

    _glRenderer.endFrame();
}

ImTextureID Scene3D::getColorTexture() const
{
    return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(_glRenderer.colorTextureId()));
}

bool Scene3D::pickBoardTile(const settings& gameSettings, float localX, float localY, float viewportWidth, float viewportHeight, int* outX, int* outY) const
{
    return _cameraController.pickBoardTile(gameSettings, localX, localY, viewportWidth, viewportHeight, outX, outY);
}

void Scene3D::initializeIfNeeded(const AppConfig& config)
{
    if (_initialized)
        return;

    AssetPaths assetPaths;
    assetPaths.models   = config.models();
    assetPaths.textures = config.textures();
    assetPaths.skybox   = config.skybox();
    assetPaths.board    = config.texture("board");

    // 1. Give the Resource Manager explicit asset directories.
    if (!_resourceManager.initialize(assetPaths))
    {
        std::cout << "Failed to initialize resources from configured asset paths.\n";
        return;
    }

    // 2. Initialize low-level OpenGL renderer resources.
    if (!_glRenderer.initialize(config.shaderPath))
    {
        std::cout << "Failed to initialize 3D board shaders from: " << config.shaderPath << "\n";
        _resourceManager.destroy();
        return;
    }

    _initialized = true;
}

void Scene3D::destroyGlResources()
{
    _glRenderer.destroy();
    _resourceManager.destroy();
    _cameraController.reset();
    _pieceAnimator.reset();
    _initialized       = false;
}

