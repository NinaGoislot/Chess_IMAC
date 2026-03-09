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

    printf("White pawn texture: %p\n", whitePawn);
}