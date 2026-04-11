#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Board;
struct settings;

namespace Render3D {

// -------- EXPLANATION --------
// CameraController is the "Cameraman".

class CameraController {
public:
    CameraController() = default;

    void reset();
    glm::vec3 updateTarget(const Board& board, const settings& gameSettings, float deltaTimeSeconds);
    glm::mat4 calculateViewProjection(const settings& gameSettings, float aspect, glm::mat4* outView = nullptr, glm::mat4* outProjection = nullptr) const;

private:
    static glm::vec3 calculatePieceTarget(const Board& board);

    glm::vec3 _cameraTarget{0.f, 0.2f, 0.f};
    bool      _cameraTargetInitialized = false;
};

} // namespace Render3D
