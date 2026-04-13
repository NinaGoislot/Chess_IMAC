#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

// Vertex layout used by normalized piece meshes.
struct Vertex
{
    glm::vec3 position{0.f, 0.f, 0.f};
    glm::vec3 normal{0.f, 1.f, 0.f};
};

// GPU-ready mesh data with packed vertices and indices.
struct ModelMeshData
{
    std::vector<Vertex>        vertices;
    std::vector<std::uint32_t> indices;
};

// Raw mesh data extracted from source files before normalization.
struct RawMeshData
{
    std::vector<glm::vec3>     positions;
    std::vector<glm::vec3>     normals;
    std::vector<std::uint32_t> indices;
    glm::mat4                  worldTransform{1.f};
};

// Controls how raw meshes are transformed and normalized.
struct MeshNormalizationOptions
{
    bool applyWorldTransform  = true;
    bool normalizeToUnitHeight = true;
};

// Result type for loading raw mesh data.
struct RawMeshLoadResult
{
    bool        success = false;
    RawMeshData mesh{};
    std::string error{};
};

// Result type for building render-ready mesh data.
struct MeshLoadResult
{
    bool          success = false;
    ModelMeshData mesh{};
    std::string   error{};
};

// Loads raw mesh data from a GLB file.
RawMeshLoadResult loadRawGLBMesh(const std::string& filepath);
// Applies transform and normalization options to raw mesh data.
void              normalizeRawMesh(RawMeshData& mesh, const MeshNormalizationOptions& options = {});
// Converts raw mesh data into packed render-ready mesh data.
MeshLoadResult    buildMeshData(const RawMeshData& rawMesh);
// High-level helper: load + normalize + build a mesh in one call.
MeshLoadResult    loadGLBMesh(const std::string& filepath, const MeshNormalizationOptions& options = {});