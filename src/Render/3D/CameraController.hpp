#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Board;
struct settings;

namespace Render3D {

// Computes camera target, view, and projection for 3D rendering.

class CameraController {
public:
    // Constructors
    CameraController() = default;

    // Lifecycle
    void reset();

    // Update function
    glm::vec3 updateTarget(const Board& board, const settings& gameSettings, float deltaTimeSeconds);

    // Getters
    glm::mat4 calculateViewProjection(const settings& gameSettings, float aspect, glm::mat4* outView = nullptr, glm::mat4* outProjection = nullptr) const;

private:
    // Helpers
    // Finds a piece-focused target position when camera targeting is enabled.
    static glm::vec3 calculatePieceTarget(const Board& board);

    // Parameters
    // Smoothed camera target in world space.
    glm::vec3 _cameraTarget{0.f, 0.2f, 0.f};
    // Indicates whether target interpolation was initialized.
    bool _cameraTargetInitialized = false;
};

} // namespace Render3D
