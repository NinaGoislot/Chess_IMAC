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

// Notice: modelCandidates() is completely DELETED. We don't need it anymore!

} // namespace

namespace Render3D {

ResourceManager::~ResourceManager()
{
    destroy();
}

// 1. Accept the path here
bool ResourceManager::initialize(const std::string& assetBasePath)
{
    if (_initialized)
        return true;

    // 2. Pass the path down to the loaders
    initializePieceModels(assetBasePath);
    const bool skyboxLoaded = loadSkyboxCubemap(assetBasePath);

    _initialized = true;
    return skyboxLoaded;
}

void ResourceManager::initChaosModel(const std::string& assetBasePath, const std::string& modelName)
{
    const std::string modelPath = assetBasePath + "/models/" + modelName + ".glb";

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

    if (!uploadPieceMesh(PieceType::Queen, loadResult.mesh)) // Using Queen as a placeholder type for Chaos
    {
        std::cout << "Failed to upload GLB mesh for '" << modelPath << "' to the GPU.\n";
        return;
    }

    std::cout << "Loaded Chaos piece GLB model from: " << modelPath << "\n";
}


const ResourceManager::PieceMeshGlData* ResourceManager::getPieceMeshFor(PieceType type) const
{
    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size()) return nullptr;
    return &_pieceMeshes[index];
}

bool ResourceManager::uploadPieceMesh(PieceType type, const ModelMeshData& meshData)
{
    // [This function remains exactly the same as your original code]
    if (meshData.vertices.empty() || meshData.indices.empty()) return false;
    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size()) return false;

    PieceMeshGlData& mesh = _pieceMeshes[index];

    glDeleteBuffers(1, &mesh.ebo);
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteVertexArrays(1, &mesh.vao);

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

    mesh.indexCount = static_cast<int>(meshData.indices.size());
    glBindVertexArray(0);
    
    return mesh.isValid();
}

void ResourceManager::initializePieceModels(const std::string& assetBasePath)
{
    // We construct the definitive path directly
    const std::string modelsDir = assetBasePath + "/models/";

    for (std::size_t i = 0u; i < PIECE_TYPE_COUNT; ++i)
    {
        const PieceType type = static_cast<PieceType>(i);
        const std::string modelPath = modelsDir + pieceModelName(type) + ".glb";

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

        std::cout << "Loaded GLB model from: " << modelPath << "\n";
    }
}

bool ResourceManager::loadSkyboxCubemap(const std::string& assetBasePath)
{
    const std::string skyboxDir = assetBasePath + "/textures/skybox/day/";
    std::array<std::string, SKYBOX_FACE_NAMES.size()> selectedFacePaths{};
    bool hasAllFaces = true;

    // Only loop through faces and extensions now. No more root guessing!
    for (std::size_t i = 0; i < SKYBOX_FACE_NAMES.size(); ++i)
    {
        bool faceFound = false;
        for (const char* extension : SKYBOX_FILE_EXTENSIONS)
        {
            const std::string candidatePath = skyboxDir + SKYBOX_FACE_NAMES[i] + extension;
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
        std::cout << "Skybox textures not found in " << skyboxDir << ". Using gradient skybox colors only.\n";
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
        glDeleteBuffers(1, &mesh.ebo);
        mesh.ebo = 0;
        glDeleteBuffers(1, &mesh.vbo);
        mesh.vbo = 0;
        glDeleteVertexArrays(1, &mesh.vao);
        mesh.vao = 0;
        mesh.indexCount = 0;
    }
}

void ResourceManager::destroySkybox()
{
    if (_skyboxCubemap != 0)
    {
        glDeleteTextures(1, &_skyboxCubemap);
        _skyboxCubemap = 0;
    }
}

void ResourceManager::destroy()
{
    destroyPieceMeshes();
    destroySkybox();
    _initialized = false;
}

} // namespace Render3D
