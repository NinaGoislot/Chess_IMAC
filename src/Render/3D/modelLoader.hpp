#pragma once

#include <cstdint>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 position{0.f, 0.f, 0.f};
    glm::vec3 normal{0.f, 1.f, 0.f};
};

struct ModelMeshData
{
    std::vector<Vertex>        vertices;
    std::vector<std::uint32_t> indices;
};

struct RawMeshData
{
    std::vector<glm::vec3>     positions;
    std::vector<glm::vec3>     normals;
    std::vector<std::uint32_t> indices;
    glm::mat4                  worldTransform{1.f};
};

struct MeshNormalizationOptions
{
    bool applyWorldTransform  = true;
    bool normalizeToUnitHeight = true;
};

struct RawMeshLoadResult
{
    bool        success = false;
    RawMeshData mesh{};
    std::string error{};
};

struct MeshLoadResult
{
    bool          success = false;
    ModelMeshData mesh{};
    std::string   error{};
};

RawMeshLoadResult loadRawGLBMesh(const std::string& filepath);
void              normalizeRawMesh(RawMeshData& mesh, const MeshNormalizationOptions& options = {});
MeshLoadResult    buildMeshData(const RawMeshData& rawMesh);
MeshLoadResult    loadGLBMesh(const std::string& filepath, const MeshNormalizationOptions& options = {});