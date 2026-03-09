#pragma once
#include "Piece.hpp"

class Pawn : public Piece {
public:
    Pawn(ImTextureID texture);
    ~Pawn() override;

    void draw(const settings& gameSettings) override;

private:
    void setAllowedMoves() override;
};