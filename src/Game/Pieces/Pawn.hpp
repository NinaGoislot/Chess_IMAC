#pragma once
#include "Piece.hpp"

class Pawn : public Piece {
public:
    explicit Pawn(ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void setAllowedMoves() override;
};