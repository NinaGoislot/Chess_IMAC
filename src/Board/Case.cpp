#include "Case.hpp"
#include <iostream>
#include "Game/Pieces/Piece.hpp"

Case::Case(int x, int y)
    : _x(x), _y(y), _isActive(false) {
    };

int Case::getX() const
{
    return _x;
};

int Case::getY() const
{
    return _y;
};

bool Case::isActive() const
{
    return _isActive;
}

void Case::setActive(bool active)
{
    _isActive = active;
}

bool Case::hasPiece() const
{
    return _piece != nullptr;
}

Piece* Case::getPiece() const
{
    return _piece;
}

void Case::setPiece(Piece* p)
{
    _piece = p;
}

Piece* Case::takePiece()
{
    Piece* movedPiece = _piece;
    _piece           = nullptr;
    return movedPiece;
}

void Case::removePiece()
{
    _piece = nullptr;
}

void Case::onClick()
{
    std::cout << "Clicked " << _x << "," << _y << "\n";
}