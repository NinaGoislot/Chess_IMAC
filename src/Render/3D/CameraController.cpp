#include "CameraController.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "Board/Board.hpp"
#include "Game/settings.hpp"

namespace {

constexpr float CAMERA_TARGET_Y        = 0.2f;
constexpr float PERSPECTIVE_FOV_DEGREES = 45.f;
constexpr float CAMERA_SMOOTHING_RATE   = 8.f;
constexpr float MAX_DELTA_TIME_SECONDS   = 0.1f;
constexpr float POV_EYE_HEIGHT_OFFSET    = 0.55f;
constexpr float POV_LOOK_DISTANCE       = 3.f;

} // namespace

namespace Render3D {

void CameraController::reset()
{
    _cameraTarget            = glm::vec3{0.f, CAMERA_TARGET_Y, 0.f};
    _cameraTargetInitialized = false;
}

glm::vec3 CameraController::calculatePieceTarget(const Board& board)
{
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;

    const Case& cornerCase = board.getCase(0, 0);
    if (cornerCase.getHasPiece())
        return glm::vec3{boardOriginX, CAMERA_TARGET_Y, boardOriginZ};

    return glm::vec3{0.f, CAMERA_TARGET_Y, 0.f};
}

glm::vec3 CameraController::updateTarget(const Board& board, const settings& gameSettings, float deltaTimeSeconds)
{
    glm::vec3 target{0.f, CAMERA_TARGET_Y, 0.f};
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
        const float clampedDeltaTime = std::clamp(deltaTimeSeconds, 0.f, MAX_DELTA_TIME_SECONDS);
        const float lerpFactor       = 1.f - std::exp(-CAMERA_SMOOTHING_RATE * clampedDeltaTime);
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
    const glm::mat4 projection = glm::perspective(glm::radians(PERSPECTIVE_FOV_DEGREES), aspect, 0.1f, 100.f);

    const float yawRadians   = glm::radians(gameSettings.cameraYawDegrees);
    const float pitchRadians = glm::radians(gameSettings.cameraPitchDegrees);

    glm::mat4 view{1.f};
    if (gameSettings.cameraPieceTarget)
    {
        const glm::vec3 eye = _cameraTarget + glm::vec3{0.f, POV_EYE_HEIGHT_OFFSET, 0.f};
        const glm::vec3 forward{
            std::cos(pitchRadians) * std::cos(yawRadians),
            std::sin(pitchRadians),
            std::cos(pitchRadians) * std::sin(yawRadians),
        };

        const glm::vec3 lookTarget = eye + forward * POV_LOOK_DISTANCE;
        view                       = glm::lookAt(eye, lookTarget, glm::vec3{0.f, 1.f, 0.f});
    }
    else
    {
        const glm::vec3 eye{
            _cameraTarget.x + gameSettings.cameraDistance * std::cos(pitchRadians) * std::cos(yawRadians),
            _cameraTarget.y + gameSettings.cameraDistance * std::sin(pitchRadians),
            _cameraTarget.z + gameSettings.cameraDistance * std::cos(pitchRadians) * std::sin(yawRadians),
        };

        view = glm::lookAt(eye, _cameraTarget, glm::vec3{0.f, 1.f, 0.f});
    }

    if (outView != nullptr)
        *outView = view;

    if (outProjection != nullptr)
        *outProjection = projection;

    return projection * view;
}

} // namespace Render3D

