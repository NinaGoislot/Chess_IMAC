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
#include "modelLoader.hpp"

namespace {

constexpr std::size_t kPieceTypeCount = 6u;

constexpr std::array<const char*, kPieceTypeCount> kPieceModelNames = {
    "pawn",
    "rook",
    "knight",
    "bishop",
    "queen",
    "king",
};

constexpr std::array<const char*, 6> kSkyboxFaceNames = {
    "right",
    "left",
    "top",
    "bottom",
    "front",
    "back",
};
constexpr std::array<const char*, 5> kSkyboxFileExtensions = {
    ".png",
    ".jpg",
    ".jpeg",
    ".bmp",
    ".tga",
};
constexpr std::array<const char*, 5> kSkyboxRoots = {
    "assets/textures/skybox/day",
    "../assets/textures/skybox/day",
    "../../assets/textures/skybox/day",
    "bin/assets/textures/skybox/day",
    "../../../assets/textures/skybox/day",
};

GLenum textureFormatForChannels(int channels)
{
    switch (channels)
    {
    case 1:
        return GL_RED;
    case 3:
        return GL_RGB;
    case 4:
        return GL_RGBA;
    default:
        return GL_RGB;
    }
}

std::size_t pieceTypeIndex(PieceType type)
{
    return static_cast<std::size_t>(type);
}

std::string pieceModelName(PieceType type)
{
    const std::size_t index = pieceTypeIndex(type);
    if (index < kPieceModelNames.size())
        return kPieceModelNames[index];

    return kPieceModelNames[0];
}

std::vector<std::string> modelCandidates(PieceType type)
{
    const std::string                modelName = pieceModelName(type);
    const std::array<std::string, 5> roots     = {
        "assets/models",
        "../assets/models",
        "../../assets/models",
        "bin/assets/models",
        "../../../assets/models",
    };

    std::vector<std::string> candidates;
    candidates.reserve(roots.size());

    for (const std::string& root : roots)
        candidates.push_back(root + "/" + modelName + ".glb");

    return candidates;
}

} // namespace

namespace Render3D {

ResourceManager::~ResourceManager()
{
    destroy();
}

bool ResourceManager::initialize()
{
    if (_initialized)
        return true;

    initializePieceModels();
    const bool skyboxLoaded = loadSkyboxCubemap();

    _initialized = true;
    return skyboxLoaded;
}

const ResourceManager::PieceMeshGlData* ResourceManager::pieceMeshFor(PieceType type) const
{
    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size())
        return nullptr;

    return &_pieceMeshes[index];
}

bool ResourceManager::uploadPieceMesh(PieceType type, const ModelMeshData& meshData)
{
    if (meshData.vertices.empty() || meshData.indices.empty())
        return false;

    const std::size_t index = pieceTypeIndex(type);
    if (index >= _pieceMeshes.size())
        return false;

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
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(meshData.vertices.size() * sizeof(Vertex)),
        meshData.vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(meshData.indices.size() * sizeof(uint32_t)),
        meshData.indices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    mesh.indexCount = static_cast<int>(meshData.indices.size());

    glBindVertexArray(0);
    return mesh.isValid();
}

void ResourceManager::initializePieceModels()
{
    for (std::size_t i = 0u; i < kPieceTypeCount; ++i)
    {
        const PieceType type = static_cast<PieceType>(i);

        ModelMeshData meshData;
        bool          loaded = false;
        std::string   loadedFrom;
        std::string   loadError;

        for (const std::string& candidate : modelCandidates(type))
        {
            if (!std::filesystem::exists(candidate))
                continue;

            MeshLoadResult loadResult = loadGLBMesh(candidate);
            if (loadResult.success)
            {
                meshData   = std::move(loadResult.mesh);
                loaded     = true;
                loadedFrom = candidate;
                break;
            }

            loadError = std::move(loadResult.error);
        }

        if (!loaded)
        {
            std::cout << "No GLB model found for piece type '" << pieceModelName(type)
                      << "'. Falling back to cube rendering for this piece.";
            if (!loadError.empty())
                std::cout << " Last loader error: " << loadError;
            std::cout << "\n";
            continue;
        }

        if (!uploadPieceMesh(type, meshData))
        {
            std::cout << "Failed to upload GLB mesh for piece type '" << pieceModelName(type)
                      << "' from: " << loadedFrom << "\n";
            continue;
        }

        std::cout << "Loaded GLB model for piece type '" << pieceModelName(type)
                  << "' from: " << loadedFrom << "\n";
    }
}

bool ResourceManager::loadSkyboxCubemap()
{
    std::array<std::string, kSkyboxFaceNames.size()> selectedFacePaths{};
    bool                                             foundValidRoot = false;

    for (const char* root : kSkyboxRoots)
    {
        std::array<std::string, kSkyboxFaceNames.size()> candidatePaths{};
        bool                                             hasAllFaces = true;

        for (std::size_t i = 0; i < kSkyboxFaceNames.size(); ++i)
        {
            bool faceFound = false;
            for (const char* extension : kSkyboxFileExtensions)
            {
                const std::string candidatePath = std::string{root} + "/" + kSkyboxFaceNames[i] + extension;
                if (std::filesystem::exists(candidatePath))
                {
                    candidatePaths[i] = candidatePath;
                    faceFound         = true;
                    break;
                }
            }

            if (!faceFound)
            {
                hasAllFaces = false;
                break;
            }
        }

        if (hasAllFaces)
        {
            selectedFacePaths = candidatePaths;
            foundValidRoot    = true;
            break;
        }
    }

    if (!foundValidRoot)
    {
        if (_skyboxCubemap != 0)
        {
            glDeleteTextures(1, &_skyboxCubemap);
            _skyboxCubemap = 0;
        }

        std::cout << "Skybox textures not found. Expected right/left/top/bottom/front/back in assets/textures/skybox/day. "
                     "Using gradient skybox colors only.\n";
        return true;
    }

    if (_skyboxCubemap != 0)
    {
        glDeleteTextures(1, &_skyboxCubemap);
        _skyboxCubemap = 0;
    }

    glGenTextures(1, &_skyboxCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _skyboxCubemap);

    for (std::size_t i = 0; i < selectedFacePaths.size(); ++i)
    {
        int            width    = 0;
        int            height   = 0;
        int            channels = 0;
        unsigned char* data     = stbi_load(selectedFacePaths[i].c_str(), &width, &height, &channels, 0);

        if (data == nullptr)
        {
            std::cout << "Failed to load skybox face texture: " << selectedFacePaths[i] << "\n";
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glDeleteTextures(1, &_skyboxCubemap);
            _skyboxCubemap = 0;
            return false;
        }

        const GLenum format         = textureFormatForChannels(channels);
        GLenum       internalFormat = GL_RGB8;
        if (format == GL_RGBA)
            internalFormat = GL_RGBA8;
        else if (format == GL_RED)
            internalFormat = GL_R8;

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i),
            0,
            static_cast<GLint>(internalFormat),
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );

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
