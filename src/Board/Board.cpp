#include "Board.hpp"
#include "Case.hpp"

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
        clicked.setPiece(_selectedCase->takePiece());

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
