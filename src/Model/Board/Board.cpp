#include "Board.hpp"
#include <algorithm>
#include "utilities/MoveAttempt.hpp"

namespace {
bool isPawnPromotionMove(const Piece* piece, int destinationY)
{
    if (piece == nullptr || piece->getType() != PieceType::Pawn)
        return false;

    const int promotionRank = (piece->getColor() == PieceColor::White) ? 0 : (Board::SIZE - 1);
    return destinationY == promotionRank;
}
} // namespace

Board::Board()
{
    for (int x = 0; x < SIZE; x++)
    {
        for (int y = 0; y < SIZE; y++)
        {
            _cases[x][y] = Case(x, y);
        }
    }
}

Case& Board::getCase(int x, int y)
{
    return _cases[x][y];
}
const Case& Board::getCase(int x, int y) const
{
    return _cases[x][y];
}

/**
 * Returns the legal moves for a piece at the specified position
 * @param from : the position of the piece to move
 * @param currentTurn : the color of the player whose turn it is
 * @return : a vector of legal move positions
 */

std::vector<Vector2D> Board::getLegalMovesFrom(Vector2D from, PieceColor currentTurn) const
{
    if (!isInside(from))
        return {};

    const int fromX = static_cast<int>(from.getX());
    const int fromY = static_cast<int>(from.getY());

    const Case& fromCase = getCase(fromX, fromY);
    if (!fromCase.getHasPiece())
        return {};

    Piece* movingPiece = fromCase.getPiece();
    if (movingPiece == nullptr || movingPiece->getColor() != currentTurn)
        return {};

    movingPiece->updateAllowedMoves(*this, from);

    std::vector<Vector2D> insideBoardMoves;
    for (const Vector2D& move : movingPiece->getAllowedMoves())
    {
        if (isInside(move))
            insideBoardMoves.push_back(move);
    }

    return insideBoardMoves;
}

bool Board::canMove(Vector2D from, Vector2D to, PieceColor currentTurn) const
{
    if (!isInside(from) || !isInside(to))
        return false;

    const int fromX = static_cast<int>(from.getX());
    const int fromY = static_cast<int>(from.getY());
    const int toX   = static_cast<int>(to.getX());
    const int toY   = static_cast<int>(to.getY());

    if (fromX == toX && fromY == toY)
        return false;

    const auto legalMoves = getLegalMovesFrom(from, currentTurn);

    return std::any_of(
        legalMoves.begin(),
        legalMoves.end(),
        [&](const Vector2D& move) {
            return static_cast<int>(move.getX()) == toX && static_cast<int>(move.getY()) == toY;
        }
    );
}

/**
 * Attempts to move a piece from one position to another
 * @param from : the position of the piece to move
 * @param to : the position to move the piece to
 * @param currentTurn : the color of the player whose turn it is
 * @return : the result of the move attempt
 */
Board::MoveResult Board::tryMove(Vector2D from, Vector2D to, PieceColor currentTurn)
{
    MoveResult result;

    if (!canMove(from, to, currentTurn))
        return result;

    const int fromX = static_cast<int>(from.getX());
    const int fromY = static_cast<int>(from.getY());
    const int toX   = static_cast<int>(to.getX());
    const int toY   = static_cast<int>(to.getY());

    Case& fromCase = getCase(fromX, fromY);
    Case& toCase   = getCase(toX, toY);

    Piece* movingPiece      = fromCase.takePiece();
    Piece* previouslyAtDest = toCase.takePiece();

    toCase.setPiece(movingPiece);

    result.moved         = true;
    result.isPromotion   = isPawnPromotionMove(movingPiece, toY);
    result.from          = from;
    result.to            = to;
    result.movedPiece    = movingPiece;
    result.capturedPiece = previouslyAtDest;

    return result;
}

Board::MoveResult Board::tryMove(const MoveAttempt& attempt)
{
    const Vector2D from(static_cast<float>(attempt.fromX), static_cast<float>(attempt.fromY));
    const Vector2D to(static_cast<float>(attempt.toX), static_cast<float>(attempt.toY));
    return tryMove(from, to, attempt.currentTurn);
}

void Board::clear()
{
    for (int x = 0; x < SIZE; x++)
    {
        for (int y = 0; y < SIZE; y++)
        {
            _cases[x][y].removePiece();
        }
    }
}

bool Board::isInside(Vector2D pos) const
{
    return pos.getX() >= 0 && pos.getX() < SIZE && pos.getY() >= 0 && pos.getY() < SIZE;
}

bool Board::isEmpty(Vector2D pos) const
{
    if (!isInside(pos))
        return false;

    return !getCase(static_cast<int>(pos.getX()), static_cast<int>(pos.getY())).getHasPiece();
}

bool Board::isEnemy(Vector2D pos, PieceColor color) const
{
    if (!isInside(pos))
        return false;

    const Case& caseAtPos = getCase(static_cast<int>(pos.getX()), static_cast<int>(pos.getY()));
    return caseAtPos.getHasPiece() && caseAtPos.getPiece()->getColor() != color;
}
