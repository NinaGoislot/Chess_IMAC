#pragma once
#include <imgui.h>
#include "Game/Pieces/Piece.hpp"

class TextureManager
{
public:
    ImTextureID whitePawn{};
    ImTextureID blackPawn{};

    void load();
    ImTextureID getPieceTexture(PieceColor color, PieceType type) const;
};