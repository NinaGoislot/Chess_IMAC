#include "Primitives.hpp"

const std::array<float, Primitives::kCubeVertexFloatCount>& Primitives::cubeVertices()
{
    // Position xyz + normal xyz.
    static constexpr std::array<float, Primitives::kCubeVertexFloatCount> vertices = {
        // +X
        0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f,
        0.5f, 0.5f, -0.5f, 1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f,
        0.5f, -0.5f, -0.5f, 1.f, 0.f, 0.f,
        0.5f, 0.5f, 0.5f, 1.f, 0.f, 0.f,
        0.5f, -0.5f, 0.5f, 1.f, 0.f, 0.f,

        // -X
        -0.5f, -0.5f, 0.5f, -1.f, 0.f, 0.f,
        -0.5f, 0.5f, 0.5f, -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -0.5f, -1.f, 0.f, 0.f,
        -0.5f, -0.5f, 0.5f, -1.f, 0.f, 0.f,
        -0.5f, 0.5f, -0.5f, -1.f, 0.f, 0.f,
        -0.5f, -0.5f, -0.5f, -1.f, 0.f, 0.f,

        // +Y
        -0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
        -0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f,
        -0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,
        0.5f, 0.5f, 0.5f, 0.f, 1.f, 0.f,
        0.5f, 0.5f, -0.5f, 0.f, 1.f, 0.f,

        // -Y
        -0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f,
        -0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f,
        0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f,
        -0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f,
        0.5f, -0.5f, -0.5f, 0.f, -1.f, 0.f,
        0.5f, -0.5f, 0.5f, 0.f, -1.f, 0.f,

        // +Z
        -0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
        0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
        0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.5f, 0.f, 0.f, 1.f,
        0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f,
        -0.5f, 0.5f, 0.5f, 0.f, 0.f, 1.f,

        // -Z
        0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f,
        -0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f,
        0.5f, -0.5f, -0.5f, 0.f, 0.f, -1.f,
        -0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f,
        0.5f, 0.5f, -0.5f, 0.f, 0.f, -1.f,
    };

    return vertices;
}