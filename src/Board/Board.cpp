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

        clicked.setActive(true);

        Piece* piece = clicked.getPiece();
        auto   moves = piece->getAllowedMoves();

        for (auto& move : moves)
        {
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
        _selectedCase->removePiece();

        clearHighlights();
        _selectedCase = nullptr;
    }
}

void Board::clearHighlights()
{
    for (int x=0;x<SIZE;x++)
        for (int y=0;y<SIZE;y++)
            _cases[x][y].setActive(false);
}
