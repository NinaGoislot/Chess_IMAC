#pragma once

#include <functional>
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
    void advanceValidatedMove(const std::function<void(PieceColor)>& onTurnEnd, const std::function<void(PieceColor)>& onTurnStart);

private:
    // Parameters
    PieceColor _current = PieceColor::White;
};
