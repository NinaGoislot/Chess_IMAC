#include "CameraController.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "Board/Board.hpp"
#include "Game/settings.hpp"

namespace {

constexpr float kCameraTargetY         = 0.2f;
constexpr float kPerspectiveFovDegrees = 45.f;
constexpr float kCameraSmoothingRate   = 8.f;
constexpr float kMaxDeltaTimeSeconds   = 0.1f;

} // namespace

namespace Render3D {

void CameraController::reset()
{
    _cameraTarget            = glm::vec3{0.f, kCameraTargetY, 0.f};
    _cameraTargetInitialized = false;
}

glm::vec3 CameraController::calculatePieceTarget(const Board& board)
{
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;

    glm::vec2 accumulator{0.f, 0.f};
    int       occupiedCount = 0;

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (currentCase.hasPiece())
            {
                accumulator.x += boardOriginX + static_cast<float>(x);
                accumulator.y += boardOriginZ + static_cast<float>(y);
                ++occupiedCount;
            }
        }
    }

    if (occupiedCount > 0)
    {
        const float inverseCount = 1.f / static_cast<float>(occupiedCount);
        return glm::vec3{accumulator.x * inverseCount, kCameraTargetY, accumulator.y * inverseCount};
    }

    return glm::vec3{0.f, kCameraTargetY, 0.f};
}

glm::vec3 CameraController::updateTarget(const Board& board, const settings& gameSettings, float deltaTimeSeconds)
{
    glm::vec3 target{0.f, kCameraTargetY, 0.f};
    if (gameSettings.cameraPieceTarget)
    {
        target = calculatePieceTarget(board);
    }

    if (!_cameraTargetInitialized)
    {
        _cameraTarget            = target;
        _cameraTargetInitialized = true;
    }

    if (gameSettings.cameraPieceTarget)
    {
        const float clampedDeltaTime = std::clamp(deltaTimeSeconds, 0.f, kMaxDeltaTimeSeconds);
        const float lerpFactor       = 1.f - std::exp(-kCameraSmoothingRate * clampedDeltaTime);
        _cameraTarget += (target - _cameraTarget) * lerpFactor;
    }
    else
    {
        _cameraTarget = target;
    }

    return _cameraTarget;
}

glm::mat4 CameraController::calculateViewProjection(const settings& gameSettings, float aspect, glm::mat4* outView, glm::mat4* outProjection) const
{
    const glm::mat4 projection = glm::perspective(glm::radians(kPerspectiveFovDegrees), aspect, 0.1f, 100.f);

    const float yawRadians   = glm::radians(gameSettings.cameraYawDegrees);
    const float pitchRadians = glm::radians(gameSettings.cameraPitchDegrees);

    const glm::vec3 eye{
        _cameraTarget.x + gameSettings.cameraDistance * std::cos(pitchRadians) * std::cos(yawRadians),
        _cameraTarget.y + gameSettings.cameraDistance * std::sin(pitchRadians),
        _cameraTarget.z + gameSettings.cameraDistance * std::cos(pitchRadians) * std::sin(yawRadians),
    };

    const glm::mat4 view = glm::lookAt(eye, _cameraTarget, glm::vec3{0.f, 1.f, 0.f});

    if (outView != nullptr)
        *outView = view;

    if (outProjection != nullptr)
        *outProjection = projection;

    return projection * view;
}

} // namespace Render3D
