#include "Game/State/GameRules.hpp"

bool GameRules::isMoveValid(const Board& board, Vector2D from, Vector2D to, PieceColor currentTurn, bool hasPendingPromotion) const
{
    // Promotion must be resolved before any new move can start
    if (hasPendingPromotion)
        return false;

    if (!board.isInside(from) || !board.isInside(to))
        return false;

    const int fromX = static_cast<int>(from.getX());
    const int fromY = static_cast<int>(from.getY());

    const Case& fromCase = board.getCase(fromX, fromY);
    if (!fromCase.getHasPiece())
        return false;

    const Piece* piece = fromCase.getPiece();
    if (piece == nullptr || piece->getColor() != currentTurn)
        return false;

    return board.canMove(from, to, currentTurn);
}

bool GameRules::canSelect(const Board& board, Vector2D tile, PieceColor currentTurn, bool hasPendingPromotion) const
{
    if (hasPendingPromotion)
        return false;

    if (!board.isInside(tile) || board.isEmpty(tile))
        return false;

    const int x = static_cast<int>(tile.getX());
    const int y = static_cast<int>(tile.getY());

    const Piece* piece = board.getCase(x, y).getPiece();
    return piece != nullptr && piece->getColor() == currentTurn;
}

std::vector<Vector2D> GameRules::getLegalMovesFrom(const Board& board, Vector2D from, PieceColor currentTurn, bool hasPendingPromotion) const
{
    if (hasPendingPromotion)
        return {};

    return board.getLegalMovesFrom(from, currentTurn);
}

bool GameRules::isPromotion(const Board::MoveResult& result) const
{
    return result.isPromotion;
}
