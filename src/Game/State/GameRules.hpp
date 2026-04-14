#pragma once

#include <vector>
#include "Board/Board.hpp"

// Stateless chess rule checks used by MatchState orchestration.
class GameRules {
public:
    // Move validation pipeline used before executing a move.
    bool isMoveValid(const Board& board, Vector2D from, Vector2D to, PieceColor currentTurn, bool hasPendingPromotion) const;
    // Selection guard used by click/selection systems.
    bool canSelect(const Board& board, Vector2D tile, PieceColor currentTurn, bool hasPendingPromotion) const;
    // Computes legal moves for the selected tile and active player.
    std::vector<Vector2D> getLegalMovesFrom(const Board& board, Vector2D from, PieceColor currentTurn, bool hasPendingPromotion) const;
    // Detects whether a move result requires promotion resolution.
    bool isPromotion(const Board::MoveResult& result) const;
};
