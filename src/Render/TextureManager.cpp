#include "TextureManager.hpp"
#include "TextureLoader.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

void TextureManager::load()
{
    const std::vector<std::string> whitePawnCandidates = {
        "../assets/textures/pieces/white_pawn.png",
        "assets/textures/pieces/white_pawn.png",
        "bin/assets/textures/pieces/white_pawn.png",
        "textures/pieces/white_pawn.png",
    };

    whitePawn = nullptr;
    for (const std::string& path : whitePawnCandidates)
    {
        whitePawn = LoadTexture(path.c_str());
        if (whitePawn != nullptr)
        {
            std::cout << "Loaded white pawn texture from: " << path << "\n";
            break;
        }
    }

    if (whitePawn == nullptr)
    {
        std::cout << "Failed to load white pawn texture from all candidates. Current working directory: "
                  << std::filesystem::current_path().string() << "\n";
    }

    const std::vector<std::string> blackPawnCandidates = {
        "../assets/textures/pieces/black_pawn.png",
        "assets/textures/pieces/black_pawn.png",
        "bin/assets/textures/pieces/black_pawn.png",
        "textures/pieces/black_pawn.png",
    };

    blackPawn = nullptr;
    for (const std::string& path : blackPawnCandidates)
    {
        blackPawn = LoadTexture(path.c_str());
        if (blackPawn != nullptr)
        {
            std::cout << "Loaded black pawn texture from: " << path << "\n";
            break;
        }
    }

    if (blackPawn == nullptr)
    {
        std::cout << "Failed to load black pawn texture from all candidates. Current working directory: "
                  << std::filesystem::current_path().string() << "\n";
    }

    printf("White pawn texture: %p\n", whitePawn);
}

ImTextureID TextureManager::getPieceTexture(PieceColor color, PieceType type) const
{
    // Until all piece textures are loaded, map every type to the pawn texture of the requested color.
    (void)type;
    return (color == PieceColor::White) ? whitePawn : blackPawn;
}