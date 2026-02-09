#pragma once
#include "Piece.hpp"

class Pawn : public Piece {
public:
    Pawn();
    ~Pawn() override;

private:
    void setAllowedMoves() override;
};