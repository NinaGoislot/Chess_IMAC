#pragma once

#include <string>
#include "Pieces/Piece.hpp"
#include "PiecesSet.hpp"

class Player {
public:
    // Constructors
    Player();
    Player(PieceColor color, const std::string& name);

    // Getters
    PieceColor         color() const { return _color; }
    const std::string& getName() const { return _name; }

    // Setters
    void setName(const std::string& name) { _name = name; }

    // Functions
    void        addPiece(const Piece& piece);
    void        removePiece(const Piece& piece);
    bool        owns(const Piece& piece) const;
    void        resetPieces();
    std::size_t aliveCount() const;

private:
    // Parameters
    PieceColor  _color;
    std::string _name;
    PiecesSet   _piecesSet;
};