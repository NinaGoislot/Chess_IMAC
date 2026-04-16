#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Board;
class Piece;
struct settings;
struct SelectionState;

namespace Render3D {

// Computes camera target, view, and projection for 3D rendering

class CameraController {
public:
    // Constructors
    CameraController() = default;

    // Lifecycle
    void reset();
    // Updates camera target from board/game state and returns current target
    glm::vec3 updateTarget(const Board& board, const settings& gameSettings, const SelectionState& selection, float deltaTimeSeconds);
    // Computes view-projection matrix and optionally returns both components
    glm::mat4 calculateViewProjection(const settings& gameSettings, float aspect, glm::mat4* outView = nullptr, glm::mat4* outProjection = nullptr) const;
    // Stores last camera matrices for picking
    void storeMatrices(const glm::mat4& view, const glm::mat4& projection);
    // Clears cached matrices when rendering is unavailable
    void clearMatrices();
    // Performs board tile picking from viewport coordinates
    bool pickBoardTile(const settings& gameSettings, float localX, float localY, float viewportWidth, float viewportHeight, int* outX, int* outY) const;

private:
    // Updates tracked piece from the currently selected tile, when available
    void updateTrackedPieceFromSelection(const Board& board, const SelectionState& selection);
    // Finds world-space target for a tracked piece. Returns false if piece is no longer on board
    static bool tryGetPieceTarget(const Board& board, const Piece* trackedPiece, glm::vec3& outTarget);

    // Parameters
    glm::vec3    _cameraTarget{0.f, 0.2f, 0.f};
    bool         _cameraTargetInitialized = false;
    const Piece* _trackedPiece            = nullptr;
    glm::mat4    _lastView{1.f};
    glm::mat4    _lastProjection{1.f};
    bool         _hasCameraMatrices = false;
};

} // namespace Render3D
