#include "Board.hpp"
#include <algorithm>
#include <utility>
#include "Case.hpp"

namespace {
bool isPawnPromotionMove(const Piece* piece, int destinationY)
{
    if (piece == nullptr || piece->type() != PieceType::Pawn)
        return false;

    const int promotionRank = (piece->color() == PieceColor::White) ? (Board::SIZE - 1) : 0;
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

bool Board::onCaseClicked(int x, int y, const PieceColor& currentTurn)
{
    if (_pendingPromotion.has_value())
    {
        return false;
    }

    Case& clicked = getCase(x, y);

    if (_selectedCase == nullptr)
    {
        if (!clicked.hasPiece())
            return false;

        Piece* piece = clicked.getPiece();
        if (piece->color() != currentTurn)
            return false;

        _selectedCase = &clicked;
        piece->updateAllowedMoves(*this, Vector2D(static_cast<float>(x), static_cast<float>(y)));
        auto moves = piece->getAllowedMoves();

        clearHighlights();
        clicked.setActive(true);

        for (auto& move : moves)
        {
            if (!isInside(move))
                continue;

            getCase(static_cast<int>(move.getX()), static_cast<int>(move.getY())).setActive(true);
        }

        return false;
    }

    if (&clicked == _selectedCase)
    {
        clearHighlights();
        _selectedCase = nullptr;
        return false;
    }

    if (clicked.isActive())
    {
        Piece* selectedPiece = _selectedCase->getPiece();
        if (selectedPiece == nullptr)
            return false;

        MoveAttempt attempt;
        attempt.fromX       = _selectedCase->getX();
        attempt.fromY       = _selectedCase->getY();
        attempt.toX         = x;
        attempt.toY         = y;
        attempt.piece       = selectedPiece;
        attempt.currentTurn = currentTurn;

        if (_beforeMoveHook)
        {
            if (!_beforeMoveHook(attempt))
            {
                clearHighlights();
                _selectedCase = nullptr;
                return false;
            }
        }

        if (!isInside(Vector2D(static_cast<float>(attempt.toX), static_cast<float>(attempt.toY))))
        {
            clearHighlights();
            _selectedCase = nullptr;
            return false;
        }

        selectedPiece->updateAllowedMoves(*this, Vector2D(static_cast<float>(attempt.fromX), static_cast<float>(attempt.fromY)));
        const auto allowedMoves = selectedPiece->getAllowedMoves();

        const bool moveStillAllowed = std::any_of(
            allowedMoves.begin(),
            allowedMoves.end(),
            [&](const Vector2D& move) {
                return static_cast<int>(move.getX()) == attempt.toX && static_cast<int>(move.getY()) == attempt.toY;
            }
        );

        if (!moveStillAllowed)
        {
            clearHighlights();
            _selectedCase = nullptr;
            return false;
        }

        Case&  destination   = getCase(attempt.toX, attempt.toY);
        Piece* movingPiece   = _selectedCase->takePiece();
        Piece* capturedPiece = destination.takePiece();

        destination.setPiece(movingPiece);

        if (isPawnPromotionMove(movingPiece, attempt.toY))
        {
            PendingPromotionState state;
            state.from        = _selectedCase;
            state.to          = &destination;
            state.pawn        = movingPiece;
            state.captured    = capturedPiece;
            state.color       = movingPiece->color();
            _pendingPromotion = state;

            clearHighlights();
            _selectedCase = nullptr;
            return false;
        }

        clearHighlights();
        _selectedCase = nullptr;
        return true;
    }

    if (clicked.hasPiece() && clicked.getPiece()->color() == currentTurn)
    {
        clearHighlights();
        _selectedCase = nullptr;
        return onCaseClicked(x, y, currentTurn);
    }

    clearHighlights();
    _selectedCase = nullptr;
    return false;
}

bool Board::hasPendingPromotion() const
{
    return _pendingPromotion.has_value();
}

std::optional<Board::PendingPromotionInfo> Board::getPendingPromotion() const
{
    if (!_pendingPromotion.has_value() || _pendingPromotion->to == nullptr)
        return std::nullopt;

    PendingPromotionInfo info;
    info.x     = _pendingPromotion->to->getX();
    info.y     = _pendingPromotion->to->getY();
    info.color = _pendingPromotion->color;
    return info;
}

bool Board::confirmPromotion(Piece* promotedPiece)
{
    if (!_pendingPromotion.has_value() || _pendingPromotion->to == nullptr || promotedPiece == nullptr)
        return false;

    _pendingPromotion->to->setPiece(promotedPiece);
    _pendingPromotion.reset();
    return true;
}

void Board::cancelPendingPromotion()
{
    if (!_pendingPromotion.has_value())
        return;

    if (_pendingPromotion->from != nullptr)
        _pendingPromotion->from->setPiece(_pendingPromotion->pawn);

    if (_pendingPromotion->to != nullptr)
        _pendingPromotion->to->setPiece(_pendingPromotion->captured);

    _pendingPromotion.reset();
}

void Board::setBeforeMoveHook(BeforeMoveHook hook)
{
    _beforeMoveHook = std::move(hook);
}

void Board::clearBeforeMoveHook()
{
    _beforeMoveHook = nullptr;
}

void Board::clear()
{
    for (int x = 0; x < SIZE; x++)
    {
        for (int y = 0; y < SIZE; y++)
        {
            _cases[x][y].removePiece();
            _cases[x][y].setActive(false);
        }
    }

    _selectedCase = nullptr;
    _pendingPromotion.reset();
}

void Board::clearHighlights()
{
    for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
            _cases[x][y].setActive(false);
}
bool Board::isInside(Vector2D pos) const
{
    return pos.getX() >= 0 && pos.getX() < SIZE && pos.getY() >= 0 && pos.getY() < SIZE;
}

bool Board::isEmpty(Vector2D pos) const
{
    if (!isInside(pos))
        return false;

    return !getCase(static_cast<int>(pos.getX()), static_cast<int>(pos.getY())).hasPiece();
}

bool Board::isEnemy(Vector2D pos, PieceColor color) const
{
    if (!isInside(pos))
        return false;

    const Case& caseAtPos = getCase(static_cast<int>(pos.getX()), static_cast<int>(pos.getY()));
    return caseAtPos.hasPiece() && caseAtPos.getPiece()->color() != color;
}

bool Board::isSelectedCase(int x, int y) const
{
    if (_selectedCase == nullptr)
        return false;

    return _selectedCase == &getCase(x, y);
}
