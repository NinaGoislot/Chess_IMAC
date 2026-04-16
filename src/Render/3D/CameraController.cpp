#include "CameraController.hpp"
#include <algorithm>
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>
#include "Model/Board/Board.hpp"
#include "Model/Match/SelectionState.hpp"
#include "Model/settings.hpp"

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
constexpr float BOARD_CENTER_Y          = -0.05f;
constexpr float BOARD_TILE_SIZE         = 0.94f;
constexpr float EPSILON                 = 1e-5f;

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
    _lastView                 = glm::mat4{1.f};
    _lastProjection           = glm::mat4{1.f};
    _hasCameraMatrices        = false;
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

void CameraController::storeMatrices(const glm::mat4& view, const glm::mat4& projection)
{
    _lastView          = view;
    _lastProjection    = projection;
    _hasCameraMatrices = true;
}

void CameraController::clearMatrices()
{
    _hasCameraMatrices = false;
}

bool CameraController::pickBoardTile(const settings& gameSettings, float localX, float localY, float viewportWidth, float viewportHeight, int* outX, int* outY) const
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

} // namespace Render3D

