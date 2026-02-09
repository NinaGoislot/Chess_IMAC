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
    Move move1(dir::up(), 1);

    _allowedMoves.push_back(move1);
}