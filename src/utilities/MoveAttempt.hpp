#pragma once

#include "Game/Pieces/Piece.hpp"

struct MoveAttempt {
    int        fromX       = 0;
    int        fromY       = 0;
    int        toX         = 0;
    int        toY         = 0;
    Piece*     piece       = nullptr;
    PieceColor currentTurn = PieceColor::White;
};
