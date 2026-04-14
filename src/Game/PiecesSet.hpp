#pragma once

#include <array>
#include <memory>
#include <unordered_set>
#include <vector>
#include "Pieces/Piece.hpp"


class TextureManager;

// Owns all pieces for one side and tracks which ones are still alive.
class PiecesSet {
public:
    // Constructors
    explicit PiecesSet(PieceColor ownerColor = PieceColor::White, const TextureManager* textures = nullptr);

    static const std::array<PieceType, 8>& backRankOrder();

    // Getters
    PieceColor getOwnerColor() const { return _ownerColor; }

    // Clears all pieces and alive tracking.
    void clear();

    const std::vector<std::unique_ptr<Piece>>& getAllPieces() const { return _pieces; }

    // Adds one piece of given type and returns raw pointer.
    Piece*      addPiece(PieceType type);
    // Marks a piece as captured/dead.
    void        pieceEaten(const Piece* piece);
    bool        getIsAlive(const Piece* piece) const;
    std::size_t getAliveCount() const;

private:
    // Builds the standard full chess piece pool.
    void                   buildFullSet();
    // Factory helper to allocate one piece with texture.
    std::unique_ptr<Piece> createOnePiece(PieceType type, ImTextureID texture) const;

    // Side that owns all pieces in this set.
    PieceColor            _ownerColor;
    // Optional texture provider used when creating pieces.
    const TextureManager* _textures;

    // The ONLY place where pieces are actually owned in memory
    std::vector<std::unique_ptr<Piece>> _pieces;

    // Fast lookup for currently alive pieces.
    std::unordered_set<const Piece*> _alivePieces;
};

