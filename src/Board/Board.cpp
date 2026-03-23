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

Case&       Board::getCase(int x, int y) {
    return _cases[x][y];
}
const Case& Board::getCase(int x, int y) const {
    return _cases[x][y];
}

bool Board::isInside(Vector2D pos) const {
    return pos.getX() >= 0 && pos.getX() < SIZE && pos.getY() >= 0 && pos.getY() < SIZE;
}

bool Board::isEmpty(Vector2D pos) const {
    if (!isInside(pos))
        return false;

    return !getCase(pos.getX(), pos.getY()).hasPiece();
}

bool Board::isEnemy(Vector2D pos, PieceColor color) const {
    if (!isInside(pos))
        return false;

    const Case& caseAtPos = getCase(pos.getX(), pos.getY());
    return caseAtPos.hasPiece() && caseAtPos.getPiece()->color() != color;
}

