// #pragma once

// #include <array>
// #include <memory>
// #include <vector>
// #include <unordered_set>
// #include "Pieces/Piece.hpp"

// class TextureManager;

// class PiecesSet {
// public:
//     explicit PiecesSet(PieceColor ownerColor = PieceColor::White, const TextureManager* textures = nullptr);
//     static const std::array<PieceType, 8>& backRankOrder();

//     PieceColor getOwnerColor() const { return _ownerColor; }

//     std::unique_ptr<Piece> takePiece(const PieceType& type);
//     void addPiece(const Piece& piece);
//     bool isAlive(const Piece& piece) const;
//     void pieceEaten(const Piece& piece);
//     void clear();
//     std::size_t aliveCount() const;

// private:
//     void buildFullSet();
//     std::unique_ptr<Piece> createOnePiece(PieceType type, ImTextureID texture) const;

//     PieceColor _ownerColor;
//     const TextureManager* _textures;
//     std::vector<std::unique_ptr<Piece>> _piecePool;
//     std::unordered_set<const Piece*> _alivePieces;
// };

#pragma once

#include <array>
#include <memory>
#include <unordered_set>
#include <vector>
#include "Pieces/Piece.hpp"


class TextureManager;

class PiecesSet {
public:
    explicit PiecesSet(PieceColor ownerColor = PieceColor::White, const TextureManager* textures = nullptr);

    static const std::array<PieceType, 8>& backRankOrder();

    PieceColor getOwnerColor() const { return _ownerColor; }

    void clear();

    const std::vector<std::unique_ptr<Piece>>& getAllPieces() const { return _pieces; }

    Piece*      addPiece(PieceType type);
    void        addPiece(const Piece& piece);
    void        pieceEaten(const Piece* piece);
    bool        isAlive(const Piece* piece) const;
    std::size_t aliveCount() const;

private:
    void                   buildFullSet();
    std::unique_ptr<Piece> createOnePiece(PieceType type, ImTextureID texture) const;

    PieceColor            _ownerColor;
    const TextureManager* _textures;

    // The ONLY place where pieces are actually owned in memory
    std::vector<std::unique_ptr<Piece>> _pieces;

    // Quick lookup for which pieces are still on the board
    std::unordered_set<const Piece*> _alivePieces;
};