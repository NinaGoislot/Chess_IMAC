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

void Board::onCaseClicked(int x, int y)
{
    Case& clicked = getCase(x, y);

    if (_selectedCase == nullptr)
    {
        if (!clicked.hasPiece())
            return;

        _selectedCase = &clicked;
        Piece* piece  = clicked.getPiece();
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

        return;
    }

    if (&clicked == _selectedCase)
    {
        clearHighlights();
        _selectedCase = nullptr;
        return;
    }

    if (clicked.isActive())
    {
        clicked.setPiece(_selectedCase->takePiece());

        clearHighlights();
        _selectedCase = nullptr;
    }
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
