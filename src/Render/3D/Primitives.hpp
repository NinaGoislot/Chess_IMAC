#pragma once
#include <array>
#include <cstddef>

// Static primitive geometry used by the 3D renderer.
namespace Primitives {
inline constexpr std::size_t CUBE_VERTEX_FLOAT_COUNT = static_cast<std::size_t>(36u) * 6u;

// Returns position xyz + normal xyz for a unit cube.
const std::array<float, CUBE_VERTEX_FLOAT_COUNT>& cubeVertices();
} // namespace Primitives