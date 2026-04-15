#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

// Vertex layout used by normalized piece meshes.
struct Vertex {
    glm::vec3 position{0.f, 0.f, 0.f};
    glm::vec3 normal{0.f, 1.f, 0.f};
};

// Packed mesh data ready for GPU upload.
struct ModelMeshData {
    std::vector<Vertex>        vertices;
    std::vector<std::uint32_t> indices;
};

// Raw mesh data extracted from source files.
struct RawMeshData {
    std::vector<glm::vec3>     positions;
    std::vector<glm::vec3>     normals;
    std::vector<std::uint32_t> indices;
    glm::mat4                  worldTransform{1.f};
};

// Normalization options applied to raw meshes.
struct MeshNormalizationOptions {
    bool applyWorldTransform   = true;
    bool normalizeToUnitHeight = true;
};

// Result of raw mesh loading.
struct RawMeshLoadResult {
    bool        success = false;
    RawMeshData mesh{};
    std::string error{};
};

// Result of packed mesh generation.
struct MeshLoadResult {
    bool          success = false;
    ModelMeshData mesh{};
    std::string   error{};
};

// Load helpers
// Loads raw mesh data from a GLB file.
RawMeshLoadResult loadRawGLBMesh(const std::string& filepath);
// Applies transform and normalization options to raw mesh data.
void normalizeRawMesh(RawMeshData& mesh, const MeshNormalizationOptions& options = {});
// Converts raw mesh data into packed render-ready mesh data.
MeshLoadResult buildMeshData(const RawMeshData& rawMesh);
// High-level helper: load + normalize + build a mesh in one call.
MeshLoadResult loadGLBMesh(const std::string& filepath, const MeshNormalizationOptions& options = {});