#pragma once

#include "Game/Pieces/Piece.hpp"

class TurnManager {
public:
    TurnManager() = default;

    PieceColor current() const { return _current; }
    void       reset(PieceColor start) { _current = start; }
    void       advance();

private:
    PieceColor _current = PieceColor::White;
};
