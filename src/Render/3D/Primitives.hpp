#pragma once
#include <array>
#include <cstddef>

namespace Primitives {
    inline constexpr std::size_t kCubeVertexFloatCount = static_cast<std::size_t>(36u) * 6u;

    // Returns Position xyz + normal xyz for a 1x1x1 cube
    const std::array<float, kCubeVertexFloatCount>& cubeVertices();
}