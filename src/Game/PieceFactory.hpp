#pragma once

#include <memory>
#include <imgui.h>
#include "Pieces/Piece.hpp"

class PieceFactory {
public:
    static std::unique_ptr<Piece> create(PieceType type, PieceColor color, ImTextureID texture);
};
