#include "ModelLoader.hpp"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

namespace {

constexpr float EPSILON = 1e-6f;

std::size_t toSize(const int value)
{
    return static_cast<std::size_t>(value);
}

template<typename Container>
bool isValidIndex(const int index, const Container& container)
{
    return index >= 0 && toSize(index) < container.size();
}

struct AccessorView {
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

    const int componentSize  = tinygltf::GetComponentSizeInBytes(accessor.componentType);
    const int componentCount = tinygltf::GetNumComponentsInType(accessor.type);
    if (componentSize <= 0 || componentCount <= 0)
    {
        error = "Accessor has an invalid component description.";
        return false;
    }

    const std::size_t elementSize    = static_cast<std::size_t>(componentSize) * static_cast<std::size_t>(componentCount);
    const int         declaredStride = accessor.ByteStride(view);
    const std::size_t stride         = (declaredStride > 0) ? static_cast<std::size_t>(declaredStride) : elementSize;

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
    const std::size_t lastOffset   = start + stride * (elementCount - 1u);
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

template <typename T>
bool readAccessor(const tinygltf::Model& model,
                  int accessorIndex,
                  std::vector<T>& out,
                  std::string& error)
{
    if (!isValidIndex(accessorIndex, model.accessors))
    {
        error = "Invalid accessor index.";
        return false;
    }

    const tinygltf::Accessor& accessor =
        model.accessors[toSize(accessorIndex)];

    if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
    {
        error = "Only FLOAT accessors supported in this version.";
        return false;
    }

    // IMPORTANT: check byte compatibility, not semantic type
    const std::size_t elementSize = sizeof(T);

    AccessorView view;
    if (!readAccessor(model, accessor, view, error))
        return false;

    if (view.elementSize != elementSize)
    {
        error = "Accessor element size does not match target type.";
        return false;
    }

    out.resize(view.count);

    for (std::size_t i = 0; i < view.count; ++i)
    {
        std::memcpy(&out[i],
                    view.data + i * view.stride,
                    sizeof(T));
    }

    return true;
}

template<typename Scalar>
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

bool extractBaseColorTexture(const tinygltf::Model& model, const tinygltf::Primitive& primitive, TextureData& out)
{
    if (!isValidIndex(primitive.material, model.materials))
        return false;

    const tinygltf::Material&    material    = model.materials[toSize(primitive.material)];
    const tinygltf::TextureInfo& baseTexture = material.pbrMetallicRoughness.baseColorTexture;
    if (!isValidIndex(baseTexture.index, model.textures))
        return false;

    const tinygltf::Texture& texture = model.textures[toSize(baseTexture.index)];
    if (!isValidIndex(texture.source, model.images))
        return false;

    const tinygltf::Image& image = model.images[toSize(texture.source)];
    if (image.image.empty() || image.width <= 0 || image.height <= 0 || image.component <= 0)
        return false;

    out.width    = image.width;
    out.height   = image.height;
    out.channels = image.component;
    out.pixels.assign(image.image.begin(), image.image.end());
    out.label = !image.name.empty() ? image.name : std::string{"baseColor"};
    return true;
}

glm::vec3 extractBaseColorFactor(const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
    if (!isValidIndex(primitive.material, model.materials))
        return glm::vec3{1.f, 1.f, 1.f};

    const tinygltf::Material& material = model.materials[toSize(primitive.material)];
    const std::vector<double>& factor = material.pbrMetallicRoughness.baseColorFactor;
    if (factor.size() < 3u)
        return glm::vec3{1.f, 1.f, 1.f};

    return glm::vec3{static_cast<float>(factor[0]), static_cast<float>(factor[1]), static_cast<float>(factor[2])};
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

        const glm::vec3 edge1        = p1 - p0;
        const glm::vec3 edge2        = p2 - p0;
        const glm::vec3 crossProduct = glm::cross(edge1, edge2);

        const float twiceArea = glm::length(crossProduct);
        if (twiceArea <= EPSILON)
            continue;

        const glm::vec3 faceNormal     = crossProduct / twiceArea;
        const float     area           = 0.5f * twiceArea;
        const glm::vec3 weightedNormal = faceNormal * area;

        normals[i0] += weightedNormal;
        normals[i1] += weightedNormal;
        normals[i2] += weightedNormal;
    }

    for (glm::vec3& normal : normals)
    {
        const float len = glm::length(normal);
        if (len > EPSILON)
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
        rotation      = glm::quat{w, x, y, z};
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

struct PrimitiveSelection {
    const tinygltf::Primitive* primitive = nullptr;
    glm::mat4                  worldTransform{1.f};
};

bool isRenderablePrimitive(const tinygltf::Primitive& primitive)
{
    if (primitive.mode != TINYGLTF_MODE_TRIANGLES)
        return false;

    return primitive.attributes.contains("POSITION");
}

void collectPrimitives(const tinygltf::Model& model, const int nodeIndex, const glm::mat4& parentTransform, std::vector<PrimitiveSelection>& out)
{
    if (!isValidIndex(nodeIndex, model.nodes))
        return;

    const tinygltf::Node& node           = model.nodes[toSize(nodeIndex)];
    const glm::mat4       worldTransform = parentTransform * nodeLocalTransform(node);

    if (isValidIndex(node.mesh, model.meshes))
    {
        const tinygltf::Mesh& mesh = model.meshes[toSize(node.mesh)];
        for (const tinygltf::Primitive& primitive : mesh.primitives)
        {
            if (!isRenderablePrimitive(primitive))
                continue;

            out.push_back(PrimitiveSelection{&primitive, worldTransform});
        }
    }

    for (const int childIndex : node.children)
    {
        collectPrimitives(model, childIndex, worldTransform, out);
    }
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

// bool findPrimitiveWithWorldTransform(const tinygltf::Model& model, PrimitiveSelection& out)
// {
//     const std::vector<int> roots = rootNodesForTraversal(model);
//     for (const int root : roots)
//     {
//         if (visitNodeForPrimitive(model, root, glm::mat4{1.f}, out))
//             return true;
//     }

//     return false;
// }

std::vector<PrimitiveSelection> findAllPrimitives(const tinygltf::Model& model)
{
    std::vector<PrimitiveSelection> selections;

    const std::vector<int> roots = rootNodesForTraversal(model);
    for (const int root : roots)
    {
        collectPrimitives(model, root, glm::mat4{1.f}, selections);
    }

    return selections;
}

void applyTransformToPositions(std::vector<glm::vec3>& positions, const glm::mat4& transform)
{
    for (glm::vec3& position : positions)
    {
        const glm::vec4 transformed = transform * glm::vec4{position, 1.f};
        position                    = glm::vec3{transformed};
    }
}

void applyTransformToNormals(std::vector<glm::vec3>& normals, const glm::mat4& transform)
{
    if (normals.empty())
        return;

    const glm::mat3 linear = glm::mat3(transform);
    if (std::abs(glm::determinant(linear)) <= EPSILON)
        return;

    const glm::mat3 normalMatrix = glm::transpose(glm::inverse(linear));

    for (glm::vec3& normal : normals)
    {
        normal          = normalMatrix * normal;
        const float len = glm::length(normal);
        if (len > EPSILON)
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
    if (height > EPSILON)
    {
        normalizingScale = 1.f / height;
    }
    else
    {
        const float extentX = maxP.x - minP.x;
        const float extentZ = maxP.z - minP.z;
        const float extent  = std::max(extentX, extentZ);
        if (extent > EPSILON)
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

    std::vector<PrimitiveSelection> selections = findAllPrimitives(model);

    if (selections.empty())
    {
        result.error = "No TRIANGLES mesh primitive with POSITION attribute was found.";
        return result;
    }

    for (const PrimitiveSelection& selection : selections)
    {
        const tinygltf::Primitive& primitive = *selection.primitive;

        const auto vertexOffset = static_cast<std::uint32_t>(result.mesh.positions.size());

        std::vector<glm::vec3> positions;

        const auto positionIt = primitive.attributes.find("POSITION");
        if (positionIt == primitive.attributes.end())
        {
            result.error = "Primitive is missing POSITION attribute.";
            return result;
        }

        std::string accessError;

        if (!readAccessor(model, positionIt->second, positions, accessError))
        {
            result.error = "Failed to read POSITION accessor: " + accessError;
            return result;
        }

        if (positions.empty())
        {
            result.error = "Primitive has no vertices.";
            return result;
        }

        std::vector<glm::vec3> normals;

        const auto normalIt = primitive.attributes.find("NORMAL");
        if (normalIt != primitive.attributes.end())
        {
            if (readAccessor(model, normalIt->second, normals, accessError))
            {
                if (normals.size() != positions.size())
                    normals.clear();
            }
        }

        std::vector<glm::vec2> uvs;

        const auto uvIt = primitive.attributes.find("TEXCOORD_0");
        if (uvIt != primitive.attributes.end())
        {
            if (readAccessor(model, uvIt->second, uvs, accessError))
            {
                if (uvs.size() != positions.size())
                    uvs.clear();
            }
        }

        applyTransformToPositions(positions, selection.worldTransform);
        applyTransformToNormals(normals, selection.worldTransform);

        result.mesh.positions.insert(result.mesh.positions.end(), positions.begin(), positions.end());
        result.mesh.normals.insert(result.mesh.normals.end(), normals.begin(), normals.end());
        result.mesh.uvs.insert(result.mesh.uvs.end(), uvs.begin(), uvs.end());

        std::vector<std::uint32_t> indices;

        if (primitive.indices >= 0)
        {
            if (!readIndicesAccessor(model, primitive.indices, indices, accessError))
            {
                result.error = "Failed to read index accessor: " + accessError;
                return result;
            }

            for (auto& idx : indices)
            {
                idx += vertexOffset;
            }
        }
        else
        {
            indices.resize(positions.size());
            for (std::size_t i = 0; i < positions.size(); ++i)
            {
                indices[i] = static_cast<uint32_t>(i) + vertexOffset;
            }
        }

        if (indices.empty())
        {
            result.error = "Primitive has no indices.";
            return result;
        }

        const std::uint32_t indexOffset = static_cast<std::uint32_t>(result.mesh.indices.size());
        const std::uint32_t indexCount  = static_cast<std::uint32_t>(indices.size());

        result.mesh.indices.insert(result.mesh.indices.end(), indices.begin(), indices.end());

        SubMeshData submesh;
        submesh.indexOffset = indexOffset;
        submesh.indexCount  = indexCount;
        submesh.baseColorFactor = extractBaseColorFactor(model, primitive);

        TextureData baseColorTexture;
        if (extractBaseColorTexture(model, primitive, baseColorTexture))
        {
            submesh.baseColorTexture = baseColorTexture;
            if (!result.baseColorTexture.has_value())
                result.baseColorTexture = baseColorTexture;
        }

        result.mesh.submeshes.push_back(std::move(submesh));
    }
    result.success = true;
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

    if (!rawMesh.submeshes.empty())
    {
        std::uint32_t expectedOffset = 0u;
        for (const SubMeshData& submesh : rawMesh.submeshes)
        {
            if (submesh.indexCount == 0u)
            {
                result.error = "Submesh has no indices.";
                return result;
            }
            if (submesh.indexOffset != expectedOffset)
            {
                result.error = "Submesh index offsets are not contiguous.";
                return result;
            }
            if (submesh.indexOffset + submesh.indexCount > rawMesh.indices.size())
            {
                result.error = "Submesh index range exceeds index buffer.";
                return result;
            }
            expectedOffset += submesh.indexCount;
        }

        if (expectedOffset != rawMesh.indices.size())
        {
            result.error = "Submesh index data does not match index buffer.";
            return result;
        }
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

    const bool hasUvs = rawMesh.uvs.size() == rawMesh.positions.size();

    result.mesh.vertices.resize(rawMesh.positions.size());
    for (std::size_t i = 0u; i < rawMesh.positions.size(); ++i)
    {
        result.mesh.vertices[i].position = rawMesh.positions[i];
        result.mesh.vertices[i].normal   = normals[i];
        result.mesh.vertices[i].uv       = hasUvs ? rawMesh.uvs[i] : glm::vec2{0.f, 0.f};
    }

    result.mesh.indices = rawMesh.indices;
    result.mesh.submeshes.clear();
    result.submeshBaseColorTextures.clear();
    result.submeshBaseColorFactors.clear();

    if (rawMesh.submeshes.empty())
    {
        SubMeshGL submesh;
        submesh.indexOffset = 0u;
        submesh.indexCount  = static_cast<uint32_t>(rawMesh.indices.size());
        result.mesh.submeshes.push_back(submesh);
        result.submeshBaseColorTextures.push_back(std::nullopt);
        result.submeshBaseColorFactors.push_back(glm::vec3{1.f, 1.f, 1.f});
    }
    else
    {
        result.mesh.submeshes.reserve(rawMesh.submeshes.size());
        result.submeshBaseColorTextures.reserve(rawMesh.submeshes.size());
        result.submeshBaseColorFactors.reserve(rawMesh.submeshes.size());

        for (const SubMeshData& submeshData : rawMesh.submeshes)
        {
            SubMeshGL submesh;
            submesh.indexOffset = submeshData.indexOffset;
            submesh.indexCount  = submeshData.indexCount;
            result.mesh.submeshes.push_back(submesh);
            result.submeshBaseColorTextures.push_back(submeshData.baseColorTexture);
            result.submeshBaseColorFactors.push_back(submeshData.baseColorFactor);
        }

        if (!result.submeshBaseColorTextures.empty() && result.submeshBaseColorTextures.front().has_value())
            result.baseColorTexture = result.submeshBaseColorTextures.front();
    }

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
    MeshLoadResult result = buildMeshData(rawResult.mesh);
    if (!result.success)
        return result;

    return result;
}
