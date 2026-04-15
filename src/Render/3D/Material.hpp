#pragma once

#include <glm/vec3.hpp>

namespace Render3D {

// Simple material data for board/piece rendering.
struct Material {
    glm::vec3 color{1.f, 1.f, 1.f};
    unsigned int textureId = 0;
    float textureScale = 1.f;
    bool useTexture = false;

    bool hasTexture() const { return useTexture && textureId != 0; }
};

} // namespace Render3D
