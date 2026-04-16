#include "TextureManager.hpp"
#include <array>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <glad/glad.h>

#include "Render/TextureData.hpp"
#include "TextureLoader.hpp"


namespace {
constexpr std::array<std::pair<PieceColor, std::string_view>, 2> PieceColors = {{
    {PieceColor::White, std::string_view{"white"}},
    {PieceColor::Black, std::string_view{"black"}},
}};

constexpr std::array<std::pair<PieceType, std::string_view>, 6> PieceTypes = {{
    {PieceType::Pawn, std::string_view{"pawn"}},
    {PieceType::Rook, std::string_view{"rook"}},
    {PieceType::Knight, std::string_view{"knight"}},
    {PieceType::Bishop, std::string_view{"bishop"}},
    {PieceType::Queen, std::string_view{"queen"}},
    {PieceType::King, std::string_view{"king"}},
}};

std::size_t colorIndex(PieceColor color)
{
    return (color == PieceColor::White) ? 0U : 1U;
}

std::size_t pieceTypeIndex(PieceType type)
{
    switch (type)
    {
    case PieceType::Pawn:
        return 0U;
    case PieceType::Rook:
        return 1U;
    case PieceType::Knight:
        return 2U;
    case PieceType::Bishop:
        return 3U;
    case PieceType::Queen:
        return 4U;
    case PieceType::King:
        return 5U;
    }

    return 0U;
}

ImTextureID loadFirstAvailableTexture(const std::vector<std::string>& candidates, const std::string& label)
{
    for (const std::string& path : candidates)
    {
        ImTextureID texture = LoadTexture(path.c_str(), false);
        if (texture != nullptr)
        {
            std::cout << "Loaded " << label << " texture from: " << path << "\n";
            return texture;
        }
    }

    std::cout << "Failed to load " << label << " texture. Tried " << candidates.size()
              << " candidate paths. Current working directory: "
              << std::filesystem::current_path().string() << "\n";
    return nullptr;
}

const unsigned char* expandTexturePixels(const TextureData& data, std::vector<std::uint8_t>& scratch)
{
    const std::size_t pixelCount = static_cast<std::size_t>(data.width) * static_cast<std::size_t>(data.height);
    if (pixelCount == 0u)
        return nullptr;

    const std::size_t expectedSize = pixelCount * static_cast<std::size_t>(data.channels);
    if (data.pixels.size() < expectedSize)
        return nullptr;

    if (data.channels == 4)
        return data.pixels.data();

    scratch.assign(pixelCount * 4u, 255u);

    if (data.channels == 3)
    {
        for (std::size_t i = 0u; i < pixelCount; ++i)
        {
            scratch[i * 4u + 0u] = data.pixels[i * 3u + 0u];
            scratch[i * 4u + 1u] = data.pixels[i * 3u + 1u];
            scratch[i * 4u + 2u] = data.pixels[i * 3u + 2u];
        }
        return scratch.data();
    }

    if (data.channels == 2)
    {
        for (std::size_t i = 0u; i < pixelCount; ++i)
        {
            const std::uint8_t luminance = data.pixels[i * 2u + 0u];
            scratch[i * 4u + 0u] = luminance;
            scratch[i * 4u + 1u] = luminance;
            scratch[i * 4u + 2u] = luminance;
            scratch[i * 4u + 3u] = data.pixels[i * 2u + 1u];
        }
        return scratch.data();
    }

    if (data.channels == 1)
    {
        for (std::size_t i = 0u; i < pixelCount; ++i)
        {
            const std::uint8_t luminance = data.pixels[i];
            scratch[i * 4u + 0u] = luminance;
            scratch[i * 4u + 1u] = luminance;
            scratch[i * 4u + 2u] = luminance;
        }
        return scratch.data();
    }

    return nullptr;
}
} // namespace

void TextureManager::load(const AppConfig& config)
{
    std::vector<std::string> baseFolders;
    if (!config.assetRoot.empty())
    {
        baseFolders.insert(baseFolders.begin(), config.texture("pieces"));
    }

    for (const auto& [color, colorName] : PieceColors)
    {
        for (const auto& [type, pieceName] : PieceTypes)
        {
            std::vector<std::string> candidates;
            candidates.reserve(baseFolders.size() * 2U);

            for (const std::string& baseFolder : baseFolders)
            {
                std::string folderStylePath;
                folderStylePath.reserve(baseFolder.size() + colorName.size() * 2U + pieceName.size() + 8U);
                folderStylePath.append(baseFolder);
                folderStylePath.append("/");
                folderStylePath.append(colorName);
                folderStylePath.append("/");
                folderStylePath.append(colorName);
                folderStylePath.append("_");
                folderStylePath.append(pieceName);
                folderStylePath.append(".png");
                candidates.push_back(std::move(folderStylePath));

                // Fallback for older flat naming style (e.g. pieces/white_pawn.png).
                std::string flatStylePath;
                flatStylePath.reserve(baseFolder.size() + colorName.size() + pieceName.size() + 6U);
                flatStylePath.append(baseFolder);
                flatStylePath.append("/");
                flatStylePath.append(colorName);
                flatStylePath.append("_");
                flatStylePath.append(pieceName);
                flatStylePath.append(".png");
                candidates.push_back(std::move(flatStylePath));
            }

            const std::string label = std::string(colorName) + " " + std::string(pieceName);
            _pieceTextures[colorIndex(color)][pieceTypeIndex(type)] = loadFirstAvailableTexture(candidates, label);
        }
    }
}

ImTextureID TextureManager::getPieceTexture(PieceColor color, PieceType type) const
{
    return _pieceTextures[colorIndex(color)][pieceTypeIndex(type)];
}

unsigned int TextureManager::createTexture2D(const TextureData& data, bool generateMipmaps)
{
    if (!data.isValid())
        return 0;

    std::vector<std::uint8_t> scratch;
    const unsigned char* pixels = expandTexturePixels(data, scratch);
    if (pixels == nullptr)
        return 0;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    if (generateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}