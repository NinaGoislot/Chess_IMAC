#pragma once

#include <unordered_map>
#include <glm/vec3.hpp>

class Piece;

namespace Render3D {

struct ExplodingPieceState
{
    glm::vec3 position{0.f, 0.f, 0.f};
    float     progress = 0.f;
};

using ExplodingPiecePositions = std::unordered_map<const Piece*, ExplodingPieceState>;

} // namespace Render3D
