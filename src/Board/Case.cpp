#include "Case.hpp"
#include <iostream>
#include "Game/Pieces/Piece.hpp"

Case::Case(int x, int y) : _x(x), _y(y) {};

int Case::getX() const
{
    return _x;
};

int Case::getY() const
{
    return _y;
};

bool Case::isActive() const {
    return _isActive;
}

void Case::setActive(bool active) {
    _isActive = active;
}

bool Case::hasPiece() const
{
    return _piece != nullptr;
}

Piece* Case::getPiece() const
{
    return _piece.get();
}

void Case::setPiece(std::unique_ptr<Piece> p)
{
    _piece = std::move(p);
}

std::unique_ptr<Piece> Case::takePiece()
{
    return std::move(_piece);
}

void Case::removePiece()
{
    _piece.reset();
}

void Case::onClick()
{
    std::cout << "Clicked " << _x << "," << _y << "\n";
}