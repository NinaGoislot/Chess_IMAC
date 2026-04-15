#include "CameraController.hpp"
#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "Board/Board.hpp"
#include "Game/State/SelectionState.hpp"
#include "Game/settings.hpp"

namespace {

constexpr float CAMERA_TARGET_Y        = 0.2f;
constexpr float PERSPECTIVE_FOV_DEGREES = 45.f;
constexpr float CAMERA_SMOOTHING_RATE   = 8.f;
constexpr float MAX_DELTA_TIME_SECONDS   = 0.1f;
constexpr float POV_EYE_HEIGHT_OFFSET    = 0.55f;
constexpr float POV_LOOK_DISTANCE       = 3.f;
constexpr float ORBIT_MIN_PITCH_DEGREES = 10.f;
constexpr float ORBIT_MAX_PITCH_DEGREES = 80.f;
constexpr float POV_MIN_PITCH_DEGREES   = -75.f;
constexpr float POV_MAX_PITCH_DEGREES   = 75.f;

glm::vec3 boardCenterTarget()
{
    return glm::vec3{0.f, CAMERA_TARGET_Y, 0.f};
}

glm::vec3 boardTileToTarget(int x, int y)
{
    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    return glm::vec3{
        boardOriginX + static_cast<float>(x),
        CAMERA_TARGET_Y,
        boardOriginZ + static_cast<float>(y),
    };
}

} // namespace

namespace Render3D {

void CameraController::reset()
{
    _cameraTarget            = boardCenterTarget();
    _cameraTargetInitialized = false;
    _trackedPiece            = nullptr;
}

void CameraController::updateTrackedPieceFromSelection(const Board& board, const SelectionState& selection)
{
    if (!selection.selected.has_value())
        return;

    const int selectedX = static_cast<int>(selection.selected->getX());
    const int selectedY = static_cast<int>(selection.selected->getY());
    if (selectedX < 0 || selectedX >= Board::SIZE || selectedY < 0 || selectedY >= Board::SIZE)
        return;

    const Case& selectedCase = board.getCase(selectedX, selectedY);
    if (!selectedCase.getHasPiece())
        return;

    _trackedPiece = selectedCase.getPiece();
}

bool CameraController::tryGetPieceTarget(const Board& board, const Piece* trackedPiece, glm::vec3& outTarget)
{
    if (trackedPiece == nullptr)
        return false;

    for (int x = 0; x < Board::SIZE; ++x)
    {
        for (int y = 0; y < Board::SIZE; ++y)
        {
            const Case& boardCase = board.getCase(x, y);
            if (!boardCase.getHasPiece())
                continue;

            if (boardCase.getPiece() == trackedPiece)
            {
                outTarget = boardTileToTarget(x, y);
                return true;
            }
        }
    }

    return false;
}

glm::vec3 CameraController::updateTarget(const Board& board, const settings& gameSettings, const SelectionState& selection, float deltaTimeSeconds)
{
    glm::vec3 target = boardCenterTarget();
    if (gameSettings.cameraPieceTarget)
    {
        updateTrackedPieceFromSelection(board, selection);

        glm::vec3 trackedTarget = boardCenterTarget();
        if (_trackedPiece != nullptr && tryGetPieceTarget(board, _trackedPiece, trackedTarget))
            target = trackedTarget;
        else
            _trackedPiece = nullptr;
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

    const float minPitch = gameSettings.cameraPieceTarget ? POV_MIN_PITCH_DEGREES : ORBIT_MIN_PITCH_DEGREES;
    const float maxPitch = gameSettings.cameraPieceTarget ? POV_MAX_PITCH_DEGREES : ORBIT_MAX_PITCH_DEGREES;
    const float clampedPitchDegrees = std::clamp(gameSettings.cameraPitchDegrees, minPitch, maxPitch);

    const float yawRadians   = glm::radians(gameSettings.cameraYawDegrees);
    const float pitchRadians = glm::radians(clampedPitchDegrees);

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

