#pragma once

#include <vector>
#include "Model/Board/Board.hpp"

// Stateless chess rule checks
class GameRules {
public:
    bool                  isMoveValid(const Board& board, Vector2D from, Vector2D to, PieceColor currentTurn, bool hasPendingPromotion) const;
    bool                  canSelect(const Board& board, Vector2D tile, PieceColor currentTurn, bool hasPendingPromotion) const;
    std::vector<Vector2D> getLegalMovesFrom(const Board& board, Vector2D from, PieceColor currentTurn, bool hasPendingPromotion) const;
    bool                  isPromotion(const Board::MoveResult& result) const;
};
