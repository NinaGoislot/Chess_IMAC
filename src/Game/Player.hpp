#pragma once

#include <string>
#include "Pieces/Piece.hpp"
#include "PiecesSet.hpp"

class Player {
public:
    Player();
    Player(PieceColor color, const std::string& name);

    PieceColor color() const { return _color; }
    const std::string& getName() const { return _name; }
    void addPiece(const Piece& piece);
    void removePiece(const Piece& piece);
    bool owns(const Piece& piece) const;
    void resetPieces();
    std::size_t aliveCount() const;

private:
    PieceColor _color;
    std::string _name;
    PiecesSet   _piecesSet;
};