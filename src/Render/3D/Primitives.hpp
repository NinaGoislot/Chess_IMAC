#pragma once
#include <array>
#include <cstddef>

// Provides static primitive geometry buffers used by the 3D renderer.
namespace Primitives {
    inline constexpr std::size_t CUBE_VERTEX_FLOAT_COUNT = static_cast<std::size_t>(36u) * 6u;

    // Returns Position xyz + normal xyz for a 1x1x1 cube
    const std::array<float, CUBE_VERTEX_FLOAT_COUNT>& cubeVertices();
}