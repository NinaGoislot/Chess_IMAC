#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct ModelMeshData
{
    std::vector<float>    vertices;
    std::vector<uint32_t> indices;
};

bool loadGLBMesh(const std::string& filepath, ModelMeshData& outMesh);