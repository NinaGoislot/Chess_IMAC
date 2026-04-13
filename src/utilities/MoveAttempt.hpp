#pragma once

#include "Game/Pieces/Piece.hpp"

// Carries full move context so hooks and chaos rules can inspect or modify it.
struct MoveAttempt {
    // Source tile coordinates.
    int        fromX       = 0;
    int        fromY       = 0;
    // Destination tile coordinates.
    int        toX         = 0;
    int        toY         = 0;
    // Piece currently attempting the move.
    Piece*     piece       = nullptr;
    // Turn owner at the time of validation.
    PieceColor currentTurn = PieceColor::White;
};
