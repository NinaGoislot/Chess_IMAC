#include "TextureManager.hpp"
#include "TextureLoader.hpp"
#include <array>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {
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
        ImTextureID texture = LoadTexture(path.c_str());
        if (texture != nullptr)
        {
            std::cout << "Loaded " << label << " texture from: " << path << "\n";
            return texture;
        }
    }

    std::cout << "Failed to load " << label
              << " texture from all candidates. Current working directory: "
              << std::filesystem::current_path().string() << "\n";
    return nullptr;
}

std::vector<std::string> textureBaseFolders()
{
    std::vector<std::string> folders;
    folders.reserve(4U);

    folders.push_back("../assets/textures/pieces");
    folders.push_back("assets/textures/pieces");
    folders.push_back("../../assets/textures/pieces");
    folders.push_back("textures/pieces");
    return folders;
}
} // namespace

void TextureManager::load()
{
    const std::array<std::pair<PieceColor, std::string>, ColorCount> colors = {
        std::pair{PieceColor::White, std::string{"white"}},
        std::pair{PieceColor::Black, std::string{"black"}},
    };

    const std::array<std::pair<PieceType, std::string>, PieceTypeCount> pieceTypes = {
        std::pair{PieceType::Pawn, std::string{"pawn"}},
        std::pair{PieceType::Rook, std::string{"rook"}},
        std::pair{PieceType::Knight, std::string{"knight"}},
        std::pair{PieceType::Bishop, std::string{"bishop"}},
        std::pair{PieceType::Queen, std::string{"queen"}},
        std::pair{PieceType::King, std::string{"king"}},
    };

    const std::vector<std::string> baseFolders = textureBaseFolders();

    for (const auto& [color, colorName] : colors)
    {
        for (const auto& [type, pieceName] : pieceTypes)
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

            std::string label;
            label.reserve(colorName.size() + pieceName.size() + 1U);
            label.append(colorName);
            label.append(" ");
            label.append(pieceName);
            _pieceTextures[colorIndex(color)][pieceTypeIndex(type)] = loadFirstAvailableTexture(candidates, label);
        }
    }
}

ImTextureID TextureManager::getPieceTexture(PieceColor color, PieceType type) const
{
    return _pieceTextures[colorIndex(color)][pieceTypeIndex(type)];
}