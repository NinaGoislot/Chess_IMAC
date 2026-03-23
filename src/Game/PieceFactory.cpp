#include "PieceFactory.hpp"
#include "Pieces/Pawn.hpp"
#include "Pieces/Rook.hpp"
#include "Pieces/Knight.hpp"
#include "Pieces/Bishop.hpp"
#include "Pieces/King.hpp"
#include "Pieces/Queen.hpp"


std::unique_ptr<Piece> PieceFactory::create(PieceType type, PieceColor color, ImTextureID texture)
{
    switch (type)
    {
    case PieceType::Pawn:
        return std::make_unique<Pawn>(color, texture);
    case PieceType::Rook:
        return std::make_unique<Rook>(color, texture);
        break;
    case PieceType::Knight:
        return std::make_unique<Knight>(color, texture);
        break;
    case PieceType::Bishop:
        return std::make_unique<Bishop>(color, texture);
        break;
    case PieceType::Queen:
        return std::make_unique<Queen>(color, texture);
        break;
    case PieceType::King:
        return std::make_unique<King>(color, texture);
        break;
    default:
        return nullptr;
    }
}
