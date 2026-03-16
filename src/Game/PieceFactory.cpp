#include "PieceFactory.hpp"
#include "Pieces/Pawn.hpp"

std::unique_ptr<Piece> PieceFactory::create(PieceType type, PieceColor color, ImTextureID texture)
{
    switch (type)
    {
    case PieceType::Pawn:
        return std::make_unique<Pawn>(color, texture);
    default:
        // Temporary fallback until dedicated classes exist for other piece types.
        return std::make_unique<Pawn>(color, texture);
    }
}
