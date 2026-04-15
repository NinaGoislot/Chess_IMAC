#pragma once

#include <array>
#include <memory>
#include <unordered_set>
#include <vector>
#include "Pieces/Piece.hpp"

class TextureManager;

// Owns all pieces for one side/player
class PiecesSet {
public:
    // Constructors
    explicit PiecesSet(PieceColor ownerColor = PieceColor::White, const TextureManager* textures = nullptr);

    static const std::array<PieceType, 8>& backRankOrder();

    // Getters
    PieceColor                                 getOwnerColor() const { return _ownerColor; }
    const std::vector<std::unique_ptr<Piece>>& getAllPieces() const { return _pieces; }
    bool                                       getIsAlive(const Piece* piece) const;
    std::size_t                                getAliveCount() const;

    // Clear
    void clear();

    // Adds
    Piece* addPiece(PieceType type);

    // Actions
    void pieceEaten(const Piece* piece);

private:
    void                   buildFullSet();
    std::unique_ptr<Piece> createOnePiece(PieceType type, ImTextureID texture) const;

    // Parameters
    PieceColor                          _ownerColor;
    const TextureManager*               _textures;
    std::vector<std::unique_ptr<Piece>> _pieces;
    std::unordered_set<const Piece*>    _alivePieces;
};
