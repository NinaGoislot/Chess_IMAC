#include "TextureManager.hpp"
#include <array>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
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
} // namespace

void TextureManager::load(const AppConfig& config)
{
    std::vector<std::string> baseFolders;
    if (!config.assetPath.empty())
    {
        baseFolders.insert(baseFolders.begin(), config.assetPath + "/textures/pieces");
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