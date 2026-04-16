#include "ResourceManager.hpp"
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "ModelLoader.hpp"
#include "Render/TextureData.hpp"
#include "Render/TextureManager.hpp"

namespace {

constexpr std::size_t PIECE_TYPE_COUNT = 6u;

constexpr std::array<const char*, PIECE_TYPE_COUNT> PIECE_MODEL_NAMES = {
    "pawn", "rook", "knight", "bishop", "queen", "king",
};

constexpr std::array<const char*, 6> SKYBOX_FACE_NAMES = {
    "right", "left", "top", "bottom", "front", "back",
};

// We keep extensions so you can mix and match jpg/png easily
constexpr std::array<const char*, 5> SKYBOX_FILE_EXTENSIONS = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tga",
};

constexpr std::array<const char*, 5> BOARD_TEXTURE_FILE_EXTENSIONS = {
    ".png", ".jpg", ".jpeg", ".bmp", ".tga",
};

constexpr const char* BOARD_EDGE_TEXTURE_BASENAME = "edges";

GLenum textureFormatForChannels(int channels)
{
    switch (channels) {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default: return GL_RGB;
    }
}

std::size_t pieceTypeIndex(PieceType type)
{
    return static_cast<std::size_t>(type);
}

std::string pieceModelName(PieceType type)
{
    const std::size_t index = pieceTypeIndex(type);
    if (index < PIECE_MODEL_NAMES.size())
        return PIECE_MODEL_NAMES[index];
    return PIECE_MODEL_NAMES[0];
}

std::string joinPath(const std::string& directory, const std::string& filename)
{
    return (std::filesystem::path(directory) / filename).string();
}

// Notice: modelCandidates() is completely DELETED. We don't need it anymore!

} // namespace

namespace Render3D {

ResourceManager::~ResourceManager()
{
    destroy();
}

bool ResourceManager::initialize(const AssetPaths& assetPaths)
{
    if (_initialized)
        return true;

    initializePieceModels(assetPaths.models);
    const bool skyboxLoaded = loadSkyboxCubemap(assetPaths.skybox);
    initializeBoardTextures(assetPaths.board);

    initChaosModel(assetPaths.models, "kirby");

    _initialized = true;
    return skyboxLoaded;
}

void ResourceManager::initChaosModel(const std::string& modelsDirectory, const std::string& modelName)
{
    const std::string modelPath = joinPath(modelsDirectory, modelName + ".glb");

    if (!std::filesystem::exists(modelPath))
    {
        std::cout << "No GLB model found at '" << modelPath << "'. Chaos pieces will use the default cube.\n";
        return;
    }

    MeshLoadResult loadResult = loadGLBMesh(modelPath);
    if (!loadResult.success)
    {
        std::cout << "Failed to load GLB at '" << modelPath << "'. Error: " << loadResult.error << "\n";
        return;
    }

    if (!uploadMesh(_kirbyMesh, loadResult.mesh))
    {
        std::cout << "Failed to upload GLB mesh for '" << modelPath << "' to the GPU.\n";
        return;
    }

    const auto& submeshTextures = loadResult.submeshBaseColorTextures;
    const auto& submeshFactors  = loadResult.submeshBaseColorFactors;
    if (!submeshTextures.empty())
    {
        const std::size_t submeshCount = (_kirbyMesh.submeshes.size() < submeshTextures.size())
                                          ? _kirbyMesh.submeshes.size()
                                          : submeshTextures.size();
        for (std::size_t submeshIndex = 0u; submeshIndex < submeshCount; ++submeshIndex)
        {
            if (submeshIndex < submeshFactors.size())
                _kirbyMesh.submeshes[submeshIndex].baseColorFactor = submeshFactors[submeshIndex];

            if (!submeshTextures[submeshIndex].has_value() || !submeshTextures[submeshIndex]->isValid())
                continue;

            const std::string textureId = "kirby.submesh" + std::to_string(submeshIndex) + ".baseColor";
            if (loadTexture2D(textureId, *submeshTextures[submeshIndex], true))
            {
                _kirbyMesh.submeshes[submeshIndex].textureId = getTexture2D(textureId);
            }
        }
    }
    else if (loadResult.baseColorTexture.has_value() && loadResult.baseColorTexture->isValid())
    {
        const std::string textureId = "kirby.baseColor";
        if (loadTexture2D(textureId, *loadResult.baseColorTexture, true))
        {
            _kirbyMesh.textureId = getTexture2D(textureId);
            if (!_kirbyMesh.submeshes.empty())
                _kirbyMesh.submeshes.front().textureId = _kirbyMesh.textureId;
        }
    }
    else if (!submeshFactors.empty())
    {
        const std::size_t submeshCount = (_kirbyMesh.submeshes.size() < submeshFactors.size())
                                          ? _kirbyMesh.submeshes.size()
                                          : submeshFactors.size();
        for (std::size_t submeshIndex = 0u; submeshIndex < submeshCount; ++submeshIndex)
        {
            _kirbyMesh.submeshes[submeshIndex].baseColorFactor = submeshFactors[submeshIndex];
        }
    }

    std::cout << "Loaded Chaos piece GLB model from: " << modelPath << "\n";
}


const ResourceManager::PieceMeshGlData* ResourceManager::getPieceMeshFor(PieceType type) const
{
    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size()) return nullptr;
    return &_pieceMeshes[index];
}

const ResourceManager::PieceMeshGlData* ResourceManager::getKirbyMesh() const
{
    if (_kirbyMesh.isValid())
        return &_kirbyMesh;
    return nullptr;
}

unsigned int ResourceManager::getTexture2D(const std::string& textureId) const
{
    const auto textureIt = _textures2D.find(textureId);
    if (textureIt == _textures2D.end())
        return 0;

    return textureIt->second;
}

bool ResourceManager::loadTexture2D(const std::string& textureId, const std::vector<std::string>& candidatePaths, bool optional)
{
    destroyTexture2D(textureId);

    for (const std::string& candidatePath : candidatePaths)
    {
        if (!std::filesystem::exists(candidatePath))
            continue;

        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* data = stbi_load(candidatePath.c_str(), &width, &height, &channels, 4);
        if (data == nullptr)
            continue;

        unsigned int textureIdGl = 0;
        glGenTextures(1, &textureIdGl);
        glBindTexture(GL_TEXTURE_2D, textureIdGl);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        _textures2D[textureId] = textureIdGl;
        std::cout << "Loaded texture '" << textureId << "' from: " << candidatePath << "\n";
        return true;
    }

    if (!optional)
    {
        std::cout << "Failed to load required texture '" << textureId << "'.\n";
        return false;
    }

    std::cout << "Texture '" << textureId << "' not found. Continuing without it.\n";
    return true;
}

bool ResourceManager::loadTexture2D(const std::string& textureId, const TextureData& data, bool generateMipmaps)
{
    destroyTexture2D(textureId);

    const unsigned int textureIdGl = TextureManager::createTexture2D(data, generateMipmaps);
    if (textureIdGl == 0)
    {
        std::cout << "Failed to build texture '" << textureId << "' from embedded data.\n";
        return false;
    }

    _textures2D[textureId] = textureIdGl;
    std::cout << "Loaded texture '" << textureId << "' from embedded data.\n";
    return true;
}

void ResourceManager::initializeBoardTextures(const std::string& boardTexturesDirectory)
{
    std::vector<std::string> boardEdgeCandidates;
    boardEdgeCandidates.reserve(BOARD_TEXTURE_FILE_EXTENSIONS.size());

    for (const char* extension : BOARD_TEXTURE_FILE_EXTENSIONS)
    {
        boardEdgeCandidates.push_back(joinPath(boardTexturesDirectory, std::string(BOARD_EDGE_TEXTURE_BASENAME) + extension));
    }

    if (!loadTexture2D(std::string(BoardEdgeTextureId), boardEdgeCandidates, true))
    {
        std::cout << "Board edge texture setup failed. Board edge will use flat color only.\n";
    }
}

bool ResourceManager::uploadPieceMesh(PieceType type, const ModelMeshData& meshData)
{
    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size()) return false;

    return uploadMesh(_pieceMeshes[index], meshData);
}

bool ResourceManager::uploadMesh(PieceMeshGlData& mesh, const ModelMeshData& meshData)
{
    if (meshData.vertices.empty() || meshData.indices.empty())
        return false;

    destroyMesh(mesh);
    mesh = PieceMeshGlData{};

    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ebo);

    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(meshData.vertices.size() * sizeof(Vertex)), meshData.vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(meshData.indices.size() * sizeof(uint32_t)), meshData.indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    mesh.indexCount = static_cast<int>(meshData.indices.size());
    mesh.submeshes.clear();

    if (meshData.submeshes.empty())
    {
        mesh.submeshes.push_back(PieceMeshGlData::SubMeshGlData{0u, static_cast<uint32_t>(mesh.indexCount), 0u});
    }
    else
    {
        mesh.submeshes.reserve(meshData.submeshes.size());
        for (const SubMeshGL& submesh : meshData.submeshes)
        {
            mesh.submeshes.push_back(PieceMeshGlData::SubMeshGlData{submesh.indexOffset, submesh.indexCount, 0u});
        }
    }

    glBindVertexArray(0);
    return mesh.isValid();
}

void ResourceManager::initializePieceModels(const std::string& modelsDirectory)
{
    for (std::size_t i = 0u; i < PIECE_TYPE_COUNT; ++i)
    {
        const PieceType type = static_cast<PieceType>(i);
        const std::string modelPath = joinPath(modelsDirectory, pieceModelName(type) + ".glb");

        if (!std::filesystem::exists(modelPath))
        {
            std::cout << "No GLB model found at '" << modelPath << "'. Falling back to cube.\n";
            continue;
        }

        MeshLoadResult loadResult = loadGLBMesh(modelPath);
        if (!loadResult.success)
        {
            std::cout << "Failed to load GLB at '" << modelPath << "'. Error: " << loadResult.error << "\n";
            continue;
        }

        if (!uploadPieceMesh(type, loadResult.mesh))
        {
            std::cout << "Failed to upload GLB mesh for '" << modelPath << "' to the GPU.\n";
            continue;
        }

        PieceMeshGlData& pieceMesh = _pieceMeshes[pieceTypeIndex(type)];
        const auto& submeshTextures = loadResult.submeshBaseColorTextures;
        const auto& submeshFactors  = loadResult.submeshBaseColorFactors;
        if (!submeshTextures.empty())
        {
            const std::size_t submeshCount = (pieceMesh.submeshes.size() < submeshTextures.size())
                                              ? pieceMesh.submeshes.size()
                                              : submeshTextures.size();
            for (std::size_t submeshIndex = 0u; submeshIndex < submeshCount; ++submeshIndex)
            {
                if (submeshIndex < submeshFactors.size())
                    pieceMesh.submeshes[submeshIndex].baseColorFactor = submeshFactors[submeshIndex];

                if (!submeshTextures[submeshIndex].has_value() || !submeshTextures[submeshIndex]->isValid())
                    continue;

                const std::string textureId = "piece." + pieceModelName(type) + ".submesh" + std::to_string(submeshIndex) + ".baseColor";
                if (loadTexture2D(textureId, *submeshTextures[submeshIndex], true))
                {
                    pieceMesh.submeshes[submeshIndex].textureId = getTexture2D(textureId);
                }
            }
        }
        else if (loadResult.baseColorTexture.has_value() && loadResult.baseColorTexture->isValid())
        {
            const std::string textureId = "piece." + pieceModelName(type) + ".baseColor";
            if (loadTexture2D(textureId, *loadResult.baseColorTexture, true))
            {
                pieceMesh.textureId = getTexture2D(textureId);
                if (!pieceMesh.submeshes.empty())
                    pieceMesh.submeshes.front().textureId = pieceMesh.textureId;
            }
        }
        else if (!submeshFactors.empty())
        {
            const std::size_t submeshCount = (pieceMesh.submeshes.size() < submeshFactors.size())
                                              ? pieceMesh.submeshes.size()
                                              : submeshFactors.size();
            for (std::size_t submeshIndex = 0u; submeshIndex < submeshCount; ++submeshIndex)
            {
                pieceMesh.submeshes[submeshIndex].baseColorFactor = submeshFactors[submeshIndex];
            }
        }

        std::cout << "Loaded GLB model from: " << modelPath << "\n";
    }
}

bool ResourceManager::loadSkyboxCubemap(const std::string& skyboxDirectory)
{
    std::array<std::string, SKYBOX_FACE_NAMES.size()> selectedFacePaths{};
    bool hasAllFaces = true;

    // Only loop through faces and extensions now. No more root guessing!
    for (std::size_t i = 0; i < SKYBOX_FACE_NAMES.size(); ++i)
    {
        bool faceFound = false;
        for (const char* extension : SKYBOX_FILE_EXTENSIONS)
        {
            const std::string candidatePath = joinPath(skyboxDirectory, std::string(SKYBOX_FACE_NAMES[i]) + extension);
            if (std::filesystem::exists(candidatePath))
            {
                selectedFacePaths[i] = candidatePath;
                faceFound = true;
                break;
            }
        }

        if (!faceFound)
        {
            hasAllFaces = false;
            break;
        }
    }

    if (!hasAllFaces)
    {
        // Cleanup if we had a previous skybox, then exit cleanly
        destroySkybox();
        std::cout << "Skybox textures not found in " << skyboxDirectory << ". Using gradient skybox colors only.\n";
        return true; 
    }

    destroySkybox(); // clear any existing texture safely

    glGenTextures(1, &_skyboxCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxCubemap);

    for (std::size_t i = 0; i < selectedFacePaths.size(); ++i)
    {
        int width = 0, height = 0, channels = 0;
        unsigned char* data = stbi_load(selectedFacePaths[i].c_str(), &width, &height, &channels, 0);

        if (data == nullptr)
        {
            std::cout << "Failed to load skybox face texture: " << selectedFacePaths[i] << "\n";
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            destroySkybox();
            return false;
        }

        const GLenum format = textureFormatForChannels(channels);
        GLenum internalFormat = (format == GL_RGBA) ? GL_RGBA8 : ((format == GL_RED) ? GL_R8 : GL_RGB8);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, static_cast<GLint>(internalFormat), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return true;
}

// [destroyPieceMeshes, destroySkybox, and destroy remain exactly the same]

void ResourceManager::destroyPieceMeshes()
{
    for (PieceMeshGlData& mesh : _pieceMeshes)
    {
        destroyMesh(mesh);
    }
}

void ResourceManager::destroyKirbyMesh()
{
    destroyMesh(_kirbyMesh);
}

void ResourceManager::destroyMesh(PieceMeshGlData& mesh)
{
    glDeleteBuffers(1, &mesh.ebo);
    mesh.ebo = 0;
    glDeleteBuffers(1, &mesh.vbo);
    mesh.vbo = 0;
    glDeleteVertexArrays(1, &mesh.vao);
    mesh.vao = 0;
    mesh.indexCount = 0;
    mesh.textureId = 0;
    mesh.submeshes.clear();
}

void ResourceManager::destroySkybox()
{
    if (_skyboxCubemap != 0)
    {
        glDeleteTextures(1, &_skyboxCubemap);
        _skyboxCubemap = 0;
    }
}

void ResourceManager::destroyTexture2D(const std::string& textureId)
{
    const auto textureIt = _textures2D.find(textureId);
    if (textureIt == _textures2D.end())
        return;

    if (textureIt->second != 0)
    {
        glDeleteTextures(1, &textureIt->second);
    }

    _textures2D.erase(textureIt);
}

void ResourceManager::destroyAllTextures2D()
{
    for (auto& [textureId, textureGlId] : _textures2D)
    {
        (void)textureId;
        if (textureGlId != 0)
            glDeleteTextures(1, &textureGlId);
    }

    _textures2D.clear();
}

void ResourceManager::destroy()
{
    destroyPieceMeshes();
    destroyKirbyMesh();
    destroySkybox();
    destroyAllTextures2D();
    _initialized = false;
}

} // namespace Render3D
