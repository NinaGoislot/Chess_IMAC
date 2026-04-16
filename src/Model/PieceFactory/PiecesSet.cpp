#include "PiecesSet.hpp"
#include <array>
#include "Model/PieceFactory/Pieces/Bishop.hpp"
#include "Model/PieceFactory/Pieces/King.hpp"
#include "Model/PieceFactory/Pieces/Knight.hpp"
#include "Model/PieceFactory/Pieces/Pawn.hpp"
#include "Model/PieceFactory/Pieces/Queen.hpp"
#include "Model/PieceFactory/Pieces/Rook.hpp"
#include "Render/TextureManager.hpp"

PiecesSet::PiecesSet(PieceColor ownerColor, const TextureManager* textures)
    : _ownerColor(ownerColor)
    , _textures(textures)
{
    buildFullSet();
}

const std::array<PieceType, 8>& PiecesSet::backRankOrder()
{
    static const std::array<PieceType, 8> order = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook,
    };

    return order;
}

Piece* PiecesSet::addPiece(PieceType type)
{
    ImTextureID texture = _textures ? _textures->getPieceTexture(_ownerColor, type) : nullptr;
    _pieces.push_back(createOnePiece(type, texture));
    Piece* created = _pieces.back().get();
    _alivePieces.insert(created);
    return created;
}

void PiecesSet::pieceEaten(const Piece* piece)
{
    _alivePieces.erase(piece);
}

bool PiecesSet::getIsAlive(const Piece* piece) const
{
    return _alivePieces.contains(piece);
}

void PiecesSet::clear()
{
    buildFullSet();
}

std::size_t PiecesSet::getAliveCount() const
{
    return _alivePieces.size();
}

void PiecesSet::buildFullSet()
{
    _pieces.clear();
    _alivePieces.clear();
    _pieces.reserve(16);

    for (const PieceType type : backRankOrder())
    {
        ImTextureID texture = _textures ? _textures->getPieceTexture(_ownerColor, type) : nullptr;
        _pieces.push_back(createOnePiece(type, texture));
    }

    for (int i = 0; i < 8; ++i)
    {
        ImTextureID texture = _textures ? _textures->getPieceTexture(_ownerColor, PieceType::Pawn) : nullptr;
        _pieces.push_back(createOnePiece(PieceType::Pawn, texture));
    }

    for (const auto& piece : _pieces)
    {
        _alivePieces.insert(piece.get());
    }
}

std::unique_ptr<Piece> PiecesSet::createOnePiece(PieceType type, ImTextureID texture) const
{
    switch (type)
    {
    case PieceType::Pawn:
        return std::make_unique<Pawn>(_ownerColor, texture);
    case PieceType::Rook:
        return std::make_unique<Rook>(_ownerColor, texture);
    case PieceType::Knight:
        return std::make_unique<Knight>(_ownerColor, texture);
    case PieceType::Bishop:
        return std::make_unique<Bishop>(_ownerColor, texture);
    case PieceType::Queen:
        return std::make_unique<Queen>(_ownerColor, texture);
    case PieceType::King:
        return std::make_unique<King>(_ownerColor, texture);
    default:
        return nullptr;
    }
}

