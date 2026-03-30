#include "modelLoader.hpp"

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace {

bool readAccessor(const tinygltf::Model& model, const tinygltf::Accessor& accessor, const unsigned char*& data, std::size_t& stride, std::size_t& elementSize)
{
    if (accessor.bufferView < 0 || accessor.bufferView >= static_cast<int>(model.bufferViews.size()))
        return false;

    const tinygltf::BufferView& view = model.bufferViews[static_cast<std::size_t>(accessor.bufferView)];
    if (view.buffer < 0 || view.buffer >= static_cast<int>(model.buffers.size()))
        return false;

    const tinygltf::Buffer& buffer = model.buffers[static_cast<std::size_t>(view.buffer)];

    const int componentSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);
    const int componentCount = tinygltf::GetNumComponentsInType(accessor.type);
    if (componentSize <= 0 || componentCount <= 0)
        return false;

    elementSize = static_cast<std::size_t>(componentSize) * static_cast<std::size_t>(componentCount);

    const int declaredStride = accessor.ByteStride(view);
    stride = (declaredStride > 0) ? static_cast<std::size_t>(declaredStride) : elementSize;

    const std::size_t start = static_cast<std::size_t>(view.byteOffset) + static_cast<std::size_t>(accessor.byteOffset);
    if (start > buffer.data.size())
        return false;

    if (accessor.count <= 0)
        return false;

    const std::size_t lastOffset = start + stride * (static_cast<std::size_t>(accessor.count) - 1u);
    if (lastOffset + elementSize > buffer.data.size())
        return false;

    data = buffer.data.data() + start;
    return true;
}

bool readVec3FloatAccessor(const tinygltf::Model& model, int accessorIndex, std::vector<float>& out)
{
    if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size()))
        return false;

    const tinygltf::Accessor& accessor = model.accessors[static_cast<std::size_t>(accessorIndex)];
    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || accessor.type != TINYGLTF_TYPE_VEC3)
        return false;

    const unsigned char* data = nullptr;
    std::size_t          stride = 0u;
    std::size_t          elementSize = 0u;
    if (!readAccessor(model, accessor, data, stride, elementSize))
        return false;

    out.resize(static_cast<std::size_t>(accessor.count) * 3u);
    for (std::size_t i = 0u; i < static_cast<std::size_t>(accessor.count); ++i)
    {
        const float* element = reinterpret_cast<const float*>(data + i * stride);
        out[i * 3u + 0u] = element[0];
        out[i * 3u + 1u] = element[1];
        out[i * 3u + 2u] = element[2];
    }

    return true;
}

bool readIndicesAccessor(const tinygltf::Model& model, int accessorIndex, std::vector<uint32_t>& out)
{
    if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size()))
        return false;

    const tinygltf::Accessor& accessor = model.accessors[static_cast<std::size_t>(accessorIndex)];
    if (accessor.type != TINYGLTF_TYPE_SCALAR)
        return false;

    const unsigned char* data = nullptr;
    std::size_t          stride = 0u;
    std::size_t          elementSize = 0u;
    if (!readAccessor(model, accessor, data, stride, elementSize))
        return false;

    out.resize(static_cast<std::size_t>(accessor.count));

    for (std::size_t i = 0u; i < static_cast<std::size_t>(accessor.count); ++i)
    {
        const unsigned char* element = data + i * stride;
        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            out[i] = static_cast<uint32_t>(*reinterpret_cast<const uint8_t*>(element));
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            out[i] = static_cast<uint32_t>(*reinterpret_cast<const uint16_t*>(element));
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            out[i] = *reinterpret_cast<const uint32_t*>(element);
            break;
        default:
            return false;
        }
    }

    return true;
}

void generateVertexNormals(const std::vector<float>& positions, const std::vector<uint32_t>& indices, std::vector<float>& normals)
{
    const std::size_t vertexCount = positions.size() / 3u;
    normals.assign(vertexCount * 3u, 0.f);

    if (indices.size() < 3u)
    {
        for (std::size_t i = 0u; i < vertexCount; ++i)
            normals[i * 3u + 1u] = 1.f;
        return;
    }

    for (std::size_t i = 0u; i + 2u < indices.size(); i += 3u)
    {
        const uint32_t i0 = indices[i + 0u];
        const uint32_t i1 = indices[i + 1u];
        const uint32_t i2 = indices[i + 2u];

        if (i0 >= vertexCount || i1 >= vertexCount || i2 >= vertexCount)
            continue;

        const float x0 = positions[static_cast<std::size_t>(i0) * 3u + 0u];
        const float y0 = positions[static_cast<std::size_t>(i0) * 3u + 1u];
        const float z0 = positions[static_cast<std::size_t>(i0) * 3u + 2u];

        const float x1 = positions[static_cast<std::size_t>(i1) * 3u + 0u];
        const float y1 = positions[static_cast<std::size_t>(i1) * 3u + 1u];
        const float z1 = positions[static_cast<std::size_t>(i1) * 3u + 2u];

        const float x2 = positions[static_cast<std::size_t>(i2) * 3u + 0u];
        const float y2 = positions[static_cast<std::size_t>(i2) * 3u + 1u];
        const float z2 = positions[static_cast<std::size_t>(i2) * 3u + 2u];

        const float e1x = x1 - x0;
        const float e1y = y1 - y0;
        const float e1z = z1 - z0;

        const float e2x = x2 - x0;
        const float e2y = y2 - y0;
        const float e2z = z2 - z0;

        const float nx = e1y * e2z - e1z * e2y;
        const float ny = e1z * e2x - e1x * e2z;
        const float nz = e1x * e2y - e1y * e2x;

        normals[static_cast<std::size_t>(i0) * 3u + 0u] += nx;
        normals[static_cast<std::size_t>(i0) * 3u + 1u] += ny;
        normals[static_cast<std::size_t>(i0) * 3u + 2u] += nz;

        normals[static_cast<std::size_t>(i1) * 3u + 0u] += nx;
        normals[static_cast<std::size_t>(i1) * 3u + 1u] += ny;
        normals[static_cast<std::size_t>(i1) * 3u + 2u] += nz;

        normals[static_cast<std::size_t>(i2) * 3u + 0u] += nx;
        normals[static_cast<std::size_t>(i2) * 3u + 1u] += ny;
        normals[static_cast<std::size_t>(i2) * 3u + 2u] += nz;
    }

    for (std::size_t i = 0u; i < vertexCount; ++i)
    {
        float nx = normals[i * 3u + 0u];
        float ny = normals[i * 3u + 1u];
        float nz = normals[i * 3u + 2u];

        const float len = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (len > 1e-6f)
        {
            nx /= len;
            ny /= len;
            nz /= len;
        }
        else
        {
            nx = 0.f;
            ny = 1.f;
            nz = 0.f;
        }

        normals[i * 3u + 0u] = nx;
        normals[i * 3u + 1u] = ny;
        normals[i * 3u + 2u] = nz;
    }
}

glm::mat4 nodeLocalTransform(const tinygltf::Node& node)
{
    if (node.matrix.size() == 16u)
    {
        glm::mat4 m{1.f};
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                m[col][row] = static_cast<float>(node.matrix[static_cast<std::size_t>(col * 4 + row)]);
            }
        }
        return m;
    }

    glm::vec3 translation{0.f, 0.f, 0.f};
    if (node.translation.size() == 3u)
    {
        translation.x = static_cast<float>(node.translation[0]);
        translation.y = static_cast<float>(node.translation[1]);
        translation.z = static_cast<float>(node.translation[2]);
    }

    glm::quat rotation{1.f, 0.f, 0.f, 0.f};
    if (node.rotation.size() == 4u)
    {
        const float x = static_cast<float>(node.rotation[0]);
        const float y = static_cast<float>(node.rotation[1]);
        const float z = static_cast<float>(node.rotation[2]);
        const float w = static_cast<float>(node.rotation[3]);
        rotation = glm::quat{w, x, y, z};
    }

    glm::vec3 scale{1.f, 1.f, 1.f};
    if (node.scale.size() == 3u)
    {
        scale.x = static_cast<float>(node.scale[0]);
        scale.y = static_cast<float>(node.scale[1]);
        scale.z = static_cast<float>(node.scale[2]);
    }

    return glm::translate(glm::mat4{1.f}, translation)
         * glm::mat4_cast(rotation)
         * glm::scale(glm::mat4{1.f}, scale);
}

struct PrimitiveSelection
{
    const tinygltf::Primitive* primitive = nullptr;
    glm::mat4                  worldTransform{1.f};
};

bool visitNodeForPrimitive(const tinygltf::Model& model, int nodeIndex, const glm::mat4& parentTransform, PrimitiveSelection& out)
{
    if (nodeIndex < 0 || nodeIndex >= static_cast<int>(model.nodes.size()))
        return false;

    const tinygltf::Node& node = model.nodes[static_cast<std::size_t>(nodeIndex)];
    const glm::mat4       worldTransform = parentTransform * nodeLocalTransform(node);

    if (node.mesh >= 0 && node.mesh < static_cast<int>(model.meshes.size()))
    {
        const tinygltf::Mesh& mesh = model.meshes[static_cast<std::size_t>(node.mesh)];
        for (const tinygltf::Primitive& primitive : mesh.primitives)
        {
            if (primitive.attributes.contains("POSITION"))
            {
                out.primitive = &primitive;
                out.worldTransform = worldTransform;
                return true;
            }
        }
    }

    for (int childIndex : node.children)
    {
        if (visitNodeForPrimitive(model, childIndex, worldTransform, out))
            return true;
    }

    return false;
}

std::vector<int> rootNodesForTraversal(const tinygltf::Model& model)
{
    std::vector<int> roots;

    int sceneIndex = model.defaultScene;
    if (sceneIndex < 0 && !model.scenes.empty())
        sceneIndex = 0;

    if (sceneIndex >= 0 && sceneIndex < static_cast<int>(model.scenes.size()))
    {
        roots = model.scenes[static_cast<std::size_t>(sceneIndex)].nodes;
        return roots;
    }

    std::vector<bool> hasParent(model.nodes.size(), false);
    for (const tinygltf::Node& node : model.nodes)
    {
        for (int child : node.children)
        {
            if (child >= 0 && child < static_cast<int>(hasParent.size()))
                hasParent[static_cast<std::size_t>(child)] = true;
        }
    }

    for (std::size_t i = 0u; i < hasParent.size(); ++i)
    {
        if (!hasParent[i])
            roots.push_back(static_cast<int>(i));
    }

    if (roots.empty())
    {
        roots.reserve(model.nodes.size());
        for (std::size_t i = 0u; i < model.nodes.size(); ++i)
            roots.push_back(static_cast<int>(i));
    }

    return roots;
}

bool findPrimitiveWithWorldTransform(const tinygltf::Model& model, PrimitiveSelection& out)
{
    const std::vector<int> roots = rootNodesForTraversal(model);
    for (int root : roots)
    {
        if (visitNodeForPrimitive(model, root, glm::mat4{1.f}, out))
            return true;
    }

    return false;
}

void applyTransformToPositions(std::vector<float>& positions, const glm::mat4& transform)
{
    for (std::size_t i = 0u; i < positions.size(); i += 3u)
    {
        const glm::vec4 p = transform * glm::vec4{positions[i + 0u], positions[i + 1u], positions[i + 2u], 1.f};
        positions[i + 0u] = p.x;
        positions[i + 1u] = p.y;
        positions[i + 2u] = p.z;
    }
}

void applyTransformToNormals(std::vector<float>& normals, const glm::mat4& transform)
{
    const glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));

    for (std::size_t i = 0u; i < normals.size(); i += 3u)
    {
        glm::vec3 n = normalMatrix * glm::vec3{normals[i + 0u], normals[i + 1u], normals[i + 2u]};
        const float len = glm::length(n);
        if (len > 1e-6f)
            n /= len;
        else
            n = glm::vec3{0.f, 1.f, 0.f};

        normals[i + 0u] = n.x;
        normals[i + 1u] = n.y;
        normals[i + 2u] = n.z;
    }
}

} // namespace

bool loadGLBMesh(const std::string& filepath, ModelMeshData& outMesh)
{
    outMesh.vertices.clear();
    outMesh.indices.clear();

    tinygltf::Model    model;
    tinygltf::TinyGLTF loader;
    std::string        err;
    std::string        warn;

    const bool loaded = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    if (!warn.empty())
        std::cout << "GLB warning (" << filepath << "): " << warn << "\n";
    if (!err.empty())
        std::cout << "GLB error (" << filepath << "): " << err << "\n";
    if (!loaded)
        return false;

    PrimitiveSelection selection;
    if (!findPrimitiveWithWorldTransform(model, selection) || selection.primitive == nullptr)
        return false;

    const tinygltf::Primitive& primitiveToUse = *selection.primitive;
    if (primitiveToUse.mode != TINYGLTF_MODE_TRIANGLES)
    {
        std::cout << "Unsupported primitive mode in " << filepath << ". Only triangle meshes are supported.\n";
        return false;
    }

    const auto posIt = primitiveToUse.attributes.find("POSITION");
    if (posIt == primitiveToUse.attributes.end())
        return false;

    std::vector<float> positions;
    if (!readVec3FloatAccessor(model, posIt->second, positions))
        return false;

    const std::size_t vertexCount = positions.size() / 3u;
    if (vertexCount == 0u)
        return false;

    std::vector<float> normals;
    const auto         normalIt = primitiveToUse.attributes.find("NORMAL");
    const bool         hasNormals = (normalIt != primitiveToUse.attributes.end())
                                  && readVec3FloatAccessor(model, normalIt->second, normals)
                                  && normals.size() == positions.size();

    if (primitiveToUse.indices >= 0)
    {
        if (!readIndicesAccessor(model, primitiveToUse.indices, outMesh.indices))
            return false;
    }
    else
    {
        outMesh.indices.resize(vertexCount);
        for (std::size_t i = 0u; i < vertexCount; ++i)
            outMesh.indices[i] = static_cast<uint32_t>(i);
    }

    if (!hasNormals)
        generateVertexNormals(positions, outMesh.indices, normals);

    applyTransformToPositions(positions, selection.worldTransform);
    applyTransformToNormals(normals, selection.worldTransform);

    outMesh.vertices.resize(vertexCount * 6u);
    for (std::size_t i = 0u; i < vertexCount; ++i)
    {
        const std::size_t dst = i * 6u;
        const std::size_t src = i * 3u;

        outMesh.vertices[dst + 0u] = positions[src + 0u];
        outMesh.vertices[dst + 1u] = positions[src + 1u];
        outMesh.vertices[dst + 2u] = positions[src + 2u];
        outMesh.vertices[dst + 3u] = normals[src + 0u];
        outMesh.vertices[dst + 4u] = normals[src + 1u];
        outMesh.vertices[dst + 5u] = normals[src + 2u];
    }

    return !outMesh.vertices.empty() && !outMesh.indices.empty();
}
