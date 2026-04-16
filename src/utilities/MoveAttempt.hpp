#pragma once

#include "Model/PieceFactory/Piece.hpp"
struct MoveAttempt {
    // Source tile coordinates
    int fromX = 0;
    int fromY = 0;
    // Destination tile coordinates
    int toX = 0;
    int toY = 0;
    // Piece currently attempting the move
    Piece* piece = nullptr;
    // Turn owner at the time of validation
    PieceColor currentTurn = PieceColor::White;
};
