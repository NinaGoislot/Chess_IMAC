#pragma once

#include <glm/vec3.hpp>
#include <unordered_map>

class Piece;

namespace Render3D {

// Runtime state for one exploding piece
struct ExplodingPieceState {
    glm::vec3 position{0.f, 0.f, 0.f};
    float     progress = 0.f;
};

// Active explosion states indexed by piece
using ExplodingPiecePositions = std::unordered_map<const Piece*, ExplodingPieceState>;

} // namespace Render3D
