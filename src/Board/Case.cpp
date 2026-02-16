#include "Case.hpp"
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

bool Case::hasPiece() const {
    return _piece ? true : false;
}
Piece* Case::getPiece() const {
    return _piece.get();
}

void Case::setPiece(std::unique_ptr<Piece> p) {
    _piece = std::move(p);
}

void Case::removePiece() {
    return _piece.reset();
}