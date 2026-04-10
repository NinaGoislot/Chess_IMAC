#include "modelLoader.hpp"

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {

constexpr float kEpsilon = 1e-6f;

std::size_t toSize(const int value)
{
    return static_cast<std::size_t>(value);
}

template <typename Container>
bool isValidIndex(const int index, const Container& container)
{
    return index >= 0 && toSize(index) < container.size();
}

struct AccessorView
{
    const unsigned char* data        = nullptr;
    std::size_t          stride      = 0u;
    std::size_t          elementSize = 0u;
    std::size_t          count       = 0u;
};

bool readAccessor(const tinygltf::Model& model, const tinygltf::Accessor& accessor, AccessorView& out, std::string& error)
{
    if (!isValidIndex(accessor.bufferView, model.bufferViews))
    {
        error = "Accessor references an invalid buffer view.";
        return false;
    }

    const tinygltf::BufferView& view = model.bufferViews[toSize(accessor.bufferView)];
    if (!isValidIndex(view.buffer, model.buffers))
    {
        error = "Buffer view references an invalid buffer.";
        return false;
    }

    const tinygltf::Buffer& buffer = model.buffers[toSize(view.buffer)];

    const int componentSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);
    const int componentCount = tinygltf::GetNumComponentsInType(accessor.type);
    if (componentSize <= 0 || componentCount <= 0)
    {
        error = "Accessor has an invalid component description.";
        return false;
    }

    const std::size_t elementSize = static_cast<std::size_t>(componentSize) * static_cast<std::size_t>(componentCount);
    const int         declaredStride = accessor.ByteStride(view);
    const std::size_t stride = (declaredStride > 0) ? static_cast<std::size_t>(declaredStride) : elementSize;

    const std::size_t start = static_cast<std::size_t>(view.byteOffset) + static_cast<std::size_t>(accessor.byteOffset);
    if (start > buffer.data.size())
    {
        error = "Accessor start offset is outside of the source buffer.";
        return false;
    }

    if (accessor.count == 0u)
    {
        error = "Accessor has zero elements.";
        return false;
    }

    const std::size_t elementCount = accessor.count;
    const std::size_t lastOffset = start + stride * (elementCount - 1u);
    if (lastOffset + elementSize > buffer.data.size())
    {
        error = "Accessor range exceeds source buffer size.";
        return false;
    }

    out.data        = buffer.data.data() + start;
    out.stride      = stride;
    out.elementSize = elementSize;
    out.count       = elementCount;
    return true;
}

bool readVec3FloatAccessor(const tinygltf::Model& model, const int accessorIndex, std::vector<glm::vec3>& out, std::string& error)
{
    if (!isValidIndex(accessorIndex, model.accessors))
    {
        error = "Invalid accessor index for VEC3 data.";
        return false;
    }

    const tinygltf::Accessor& accessor = model.accessors[toSize(accessorIndex)];
    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || accessor.type != TINYGLTF_TYPE_VEC3)
    {
        error = "Accessor must be FLOAT VEC3.";
        return false;
    }

    AccessorView view;
    if (!readAccessor(model, accessor, view, error))
        return false;

    out.resize(view.count);
    for (std::size_t i = 0u; i < view.count; ++i)
    {
        float values[3]{};
        std::memcpy(values, view.data + i * view.stride, sizeof(values));
        out[i] = glm::vec3{values[0], values[1], values[2]};
    }

    return true;
}

template <typename Scalar>
Scalar readScalarUnaligned(const unsigned char* data)
{
    Scalar value{};
    std::memcpy(&value, data, sizeof(Scalar));
    return value;
}

bool readIndicesAccessor(const tinygltf::Model& model, const int accessorIndex, std::vector<std::uint32_t>& out, std::string& error)
{
    if (!isValidIndex(accessorIndex, model.accessors))
    {
        error = "Invalid accessor index for index data.";
        return false;
    }

    const tinygltf::Accessor& accessor = model.accessors[toSize(accessorIndex)];
    if (accessor.type != TINYGLTF_TYPE_SCALAR)
    {
        error = "Index accessor must be SCALAR.";
        return false;
    }

    AccessorView view;
    if (!readAccessor(model, accessor, view, error))
        return false;

    out.resize(view.count);
    for (std::size_t i = 0u; i < view.count; ++i)
    {
        const unsigned char* element = view.data + i * view.stride;

        switch (accessor.componentType)
        {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
            out[i] = static_cast<std::uint32_t>(readScalarUnaligned<std::uint8_t>(element));
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
            out[i] = static_cast<std::uint32_t>(readScalarUnaligned<std::uint16_t>(element));
            break;
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
            out[i] = readScalarUnaligned<std::uint32_t>(element);
            break;
        default:
            error = "Unsupported index component type.";
            return false;
        }
    }

    return true;
}

std::vector<glm::vec3> generateAreaWeightedVertexNormals(const std::vector<glm::vec3>& positions, const std::vector<std::uint32_t>& indices)
{
    std::vector<glm::vec3> normals(positions.size(), glm::vec3{0.f, 0.f, 0.f});

    if (indices.size() < 3u)
    {
        for (glm::vec3& normal : normals)
            normal = glm::vec3{0.f, 1.f, 0.f};
        return normals;
    }

    for (std::size_t i = 0u; i + 2u < indices.size(); i += 3u)
    {
        const std::uint32_t i0 = indices[i + 0u];
        const std::uint32_t i1 = indices[i + 1u];
        const std::uint32_t i2 = indices[i + 2u];

        if (i0 >= positions.size() || i1 >= positions.size() || i2 >= positions.size())
            continue;

        const glm::vec3& p0 = positions[i0];
        const glm::vec3& p1 = positions[i1];
        const glm::vec3& p2 = positions[i2];

        const glm::vec3 edge1 = p1 - p0;
        const glm::vec3 edge2 = p2 - p0;
        const glm::vec3 crossProduct = glm::cross(edge1, edge2);

        const float twiceArea = glm::length(crossProduct);
        if (twiceArea <= kEpsilon)
            continue;

        const glm::vec3 faceNormal = crossProduct / twiceArea;
        const float     area = 0.5f * twiceArea;
        const glm::vec3 weightedNormal = faceNormal * area;

        normals[i0] += weightedNormal;
        normals[i1] += weightedNormal;
        normals[i2] += weightedNormal;
    }

    for (glm::vec3& normal : normals)
    {
        const float len = glm::length(normal);
        if (len > kEpsilon)
            normal /= len;
        else
            normal = glm::vec3{0.f, 1.f, 0.f};
    }

    return normals;
}

glm::mat4 nodeLocalTransform(const tinygltf::Node& node)
{
    if (node.matrix.size() == 16u)
    {
        return glm::mat4{glm::make_mat4(node.matrix.data())};
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

bool visitNodeForPrimitive(const tinygltf::Model& model, const int nodeIndex, const glm::mat4& parentTransform, PrimitiveSelection& out)
{
    if (!isValidIndex(nodeIndex, model.nodes))
        return false;

    const tinygltf::Node& node = model.nodes[toSize(nodeIndex)];
    const glm::mat4       worldTransform = parentTransform * nodeLocalTransform(node);

    if (isValidIndex(node.mesh, model.meshes))
    {
        const tinygltf::Mesh& mesh = model.meshes[toSize(node.mesh)];
        for (const tinygltf::Primitive& primitive : mesh.primitives)
        {
            if (primitive.attributes.contains("POSITION"))
            {
                out.primitive      = &primitive;
                out.worldTransform = worldTransform;
                return true;
            }
        }
    }

    for (const int childIndex : node.children)
    {
        if (visitNodeForPrimitive(model, childIndex, worldTransform, out))
            return true;
    }

    return false;
}

std::vector<int> rootNodesForTraversal(const tinygltf::Model& model)
{
    int sceneIndex = model.defaultScene;
    if (sceneIndex < 0 && !model.scenes.empty())
        sceneIndex = 0;

    if (isValidIndex(sceneIndex, model.scenes))
        return model.scenes[toSize(sceneIndex)].nodes;

    std::vector<bool> hasParent(model.nodes.size(), false);
    for (const tinygltf::Node& node : model.nodes)
    {
        for (const int child : node.children)
        {
            if (child >= 0 && toSize(child) < hasParent.size())
                hasParent[toSize(child)] = true;
        }
    }

    std::vector<int> roots;
    roots.reserve(model.nodes.size());
    for (std::size_t i = 0u; i < hasParent.size(); ++i)
    {
        if (!hasParent[i])
            roots.push_back(static_cast<int>(i));
    }

    if (!roots.empty())
        return roots;

    roots.clear();
    roots.reserve(model.nodes.size());
    for (std::size_t i = 0u; i < model.nodes.size(); ++i)
        roots.push_back(static_cast<int>(i));

    return roots;
}

bool findPrimitiveWithWorldTransform(const tinygltf::Model& model, PrimitiveSelection& out)
{
    const std::vector<int> roots = rootNodesForTraversal(model);
    for (const int root : roots)
    {
        if (visitNodeForPrimitive(model, root, glm::mat4{1.f}, out))
            return true;
    }

    return false;
}

void applyTransformToPositions(std::vector<glm::vec3>& positions, const glm::mat4& transform)
{
    for (glm::vec3& position : positions)
    {
        const glm::vec4 transformed = transform * glm::vec4{position, 1.f};
        position = glm::vec3{transformed};
    }
}

void applyTransformToNormals(std::vector<glm::vec3>& normals, const glm::mat4& transform)
{
    if (normals.empty())
        return;

    const glm::mat3 linear = glm::mat3(transform);
    if (std::abs(glm::determinant(linear)) <= kEpsilon)
        return;

    const glm::mat3 normalMatrix = glm::transpose(glm::inverse(linear));

    for (glm::vec3& normal : normals)
    {
        normal = normalMatrix * normal;
        const float len = glm::length(normal);
        if (len > kEpsilon)
            normal /= len;
        else
            normal = glm::vec3{0.f, 1.f, 0.f};
    }
}

void normalizePositionsToGroundedUnitHeight(std::vector<glm::vec3>& positions)
{
    if (positions.empty())
        return;

    glm::vec3 minP = positions.front();
    glm::vec3 maxP = positions.front();

    for (const glm::vec3& p : positions)
    {
        minP = glm::min(minP, p);
        maxP = glm::max(maxP, p);
    }

    const float centerX = 0.5f * (minP.x + maxP.x);
    const float centerZ = 0.5f * (minP.z + maxP.z);
    const float height  = maxP.y - minP.y;

    float normalizingScale = 1.f;
    if (height > kEpsilon)
    {
        normalizingScale = 1.f / height;
    }
    else
    {
        const float extentX = maxP.x - minP.x;
        const float extentZ = maxP.z - minP.z;
        const float extent  = std::max(extentX, extentZ);
        if (extent > kEpsilon)
            normalizingScale = 1.f / extent;
    }

    for (glm::vec3& p : positions)
    {
        p.x = (p.x - centerX) * normalizingScale;
        p.y = (p.y - minP.y) * normalizingScale;
        p.z = (p.z - centerZ) * normalizingScale;
    }
}

} // namespace

RawMeshLoadResult loadRawGLBMesh(const std::string& filepath)
{
    RawMeshLoadResult result;

    tinygltf::Model    model;
    tinygltf::TinyGLTF loader;
    std::string        err;
    std::string        warn;

    const bool loaded = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    if (!loaded)
    {
        result.error = !err.empty() ? err : std::string{"Failed to load GLB file."};
        if (!warn.empty())
            result.error += " Warning: " + warn;
        return result;
    }

    PrimitiveSelection selection;
    if (!findPrimitiveWithWorldTransform(model, selection) || selection.primitive == nullptr)
    {
        result.error = "No mesh primitive with POSITION attribute was found.";
        return result;
    }

    const tinygltf::Primitive& primitive = *selection.primitive;
    if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
    {
        result.error = "Unsupported primitive mode. Only TRIANGLES are supported.";
        return result;
    }

    const auto positionIt = primitive.attributes.find("POSITION");
    if (positionIt == primitive.attributes.end())
    {
        result.error = "Primitive is missing POSITION attribute.";
        return result;
    }

    std::string accessError;
    if (!readVec3FloatAccessor(model, positionIt->second, result.mesh.positions, accessError))
    {
        result.error = "Failed to read POSITION accessor: " + accessError;
        return result;
    }

    if (result.mesh.positions.empty())
    {
        result.error = "Mesh has no vertices.";
        return result;
    }

    const auto normalIt = primitive.attributes.find("NORMAL");
    if (normalIt != primitive.attributes.end())
    {
        std::vector<glm::vec3> normals;
        if (readVec3FloatAccessor(model, normalIt->second, normals, accessError))
        {
            if (normals.size() == result.mesh.positions.size())
                result.mesh.normals = std::move(normals);
        }
    }

    if (primitive.indices >= 0)
    {
        if (!readIndicesAccessor(model, primitive.indices, result.mesh.indices, accessError))
        {
            result.error = "Failed to read index accessor: " + accessError;
            return result;
        }
    }
    else
    {
        result.mesh.indices.resize(result.mesh.positions.size());
        for (std::size_t i = 0u; i < result.mesh.indices.size(); ++i)
            result.mesh.indices[i] = static_cast<std::uint32_t>(i);
    }

    if (result.mesh.indices.empty())
    {
        result.error = "Mesh has no indices.";
        return result;
    }

    result.mesh.worldTransform = selection.worldTransform;
    result.success             = true;
    return result;
}

void normalizeRawMesh(RawMeshData& mesh, const MeshNormalizationOptions& options)
{
    if (mesh.positions.empty())
        return;

    if (options.applyWorldTransform)
    {
        applyTransformToPositions(mesh.positions, mesh.worldTransform);
        applyTransformToNormals(mesh.normals, mesh.worldTransform);
        mesh.worldTransform = glm::mat4{1.f};
    }

    if (options.normalizeToUnitHeight)
        normalizePositionsToGroundedUnitHeight(mesh.positions);
}

MeshLoadResult buildMeshData(const RawMeshData& rawMesh)
{
    MeshLoadResult result;

    if (rawMesh.positions.empty())
    {
        result.error = "Cannot build mesh data from empty positions.";
        return result;
    }

    if (rawMesh.indices.empty())
    {
        result.error = "Cannot build mesh data from empty indices.";
        return result;
    }

    for (const std::uint32_t index : rawMesh.indices)
    {
        if (index >= rawMesh.positions.size())
        {
            result.error = "Index buffer references vertices out of range.";
            return result;
        }
    }

    std::vector<glm::vec3> normals = rawMesh.normals;
    if (normals.size() != rawMesh.positions.size())
        normals = generateAreaWeightedVertexNormals(rawMesh.positions, rawMesh.indices);

    result.mesh.vertices.resize(rawMesh.positions.size());
    for (std::size_t i = 0u; i < rawMesh.positions.size(); ++i)
    {
        result.mesh.vertices[i].position = rawMesh.positions[i];
        result.mesh.vertices[i].normal   = normals[i];
    }

    result.mesh.indices = rawMesh.indices;
    result.success      = !result.mesh.vertices.empty() && !result.mesh.indices.empty();
    if (!result.success)
        result.error = "Mesh conversion produced an empty result.";

    return result;
}

MeshLoadResult loadGLBMesh(const std::string& filepath, const MeshNormalizationOptions& options)
{
    RawMeshLoadResult rawResult = loadRawGLBMesh(filepath);
    if (!rawResult.success)
        return MeshLoadResult{false, {}, rawResult.error};

    normalizeRawMesh(rawResult.mesh, options);
    return buildMeshData(rawResult.mesh);
}
