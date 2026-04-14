#include "Game/State/MoveExecutor.hpp"

Board::MoveResult MoveExecutor::execute(Board& board, const MoveAttempt& attempt) const
{
    const Vector2D from(static_cast<float>(attempt.fromX), static_cast<float>(attempt.fromY));
    const Vector2D to(static_cast<float>(attempt.toX), static_cast<float>(attempt.toY));
    return board.tryMove(from, to, attempt.currentTurn);
}
