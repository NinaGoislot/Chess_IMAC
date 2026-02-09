#include "Pawn.hpp"
#include <vector>
#include "utilities/Direction.hpp"

Pawn::Pawn()
    : Piece()
{
    setAllowedMoves();
}

Pawn::~Pawn() = default;

void Pawn::setAllowedMoves()
{
    Vector2D move1(0, 1);

    _allowedMoves.push_back(move1);
}