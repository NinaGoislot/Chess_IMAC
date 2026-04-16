#pragma once
#include <imgui.h>
#include <array>

#include "Model/PieceFactory/Piece.hpp"
#include "utilities/AppConfig.hpp"

// Loads and serves piece textures for 2D rendering.
class TextureManager
{
public:
    // Init function: loads all piece textures from configured asset paths.
    void load(const AppConfig& config = {});
    // Getters
    ImTextureID getPieceTexture(PieceColor color, PieceType type) const;

private:
    // Fixed counts used to size texture tables.
    static constexpr std::size_t ColorCount     = 2;
    static constexpr std::size_t PieceTypeCount = 6;
    // Indexed texture table: [color][pieceType].
    std::array<std::array<ImTextureID, PieceTypeCount>, ColorCount> _pieceTextures{};
};
