#pragma once

#include "Game/Pieces/Piece.hpp"

class TurnManager {
public:
    // Constructors
    TurnManager() = default;

    // Getters
    PieceColor getCurrent() const { return _current; }

    // Setters
    void setCurrent(PieceColor start) { _current = start; }

    // Functions
    void nextTurn();

private:
    // Parameters
    PieceColor _current = PieceColor::White;
};
