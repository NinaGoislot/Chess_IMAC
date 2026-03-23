#pragma once
#include <imgui.h>
#include <array>
#include "Game/Pieces/Piece.hpp"

class TextureManager
{
public:
    void load();
    ImTextureID getPieceTexture(PieceColor color, PieceType type) const;

private:
    static constexpr std::size_t ColorCount     = 2;
    static constexpr std::size_t PieceTypeCount = 6;
    std::array<std::array<ImTextureID, PieceTypeCount>, ColorCount> _pieceTextures{};
};