#pragma once

#include <functional>
#include "Game/Pieces/Piece.hpp"

// Tracks and advances the active player turn.
class TurnManager {
public:
    // Constructors
    TurnManager() = default;

    // Getters
    PieceColor getCurrent() const { return _current; }

    // Setters
    void setCurrent(PieceColor start) { _current = start; }

    // Advances to the opposite player's turn.
    void nextTurn();
    // Applies validated move turn transition and triggers turn callbacks.
    void advanceValidatedMove(const std::function<void(PieceColor)>& onTurnEnd, const std::function<void(PieceColor)>& onTurnStart);

private:
    // Current side to play.
    PieceColor _current = PieceColor::White;
};
