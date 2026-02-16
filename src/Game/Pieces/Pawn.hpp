#pragma once
#include "Piece.hpp"

class Pawn : public Piece {
public:
    Pawn();
    ~Pawn() override;

    void draw(const ImTextureID &texture, const settings& gameSettings) override;

private:
    void setAllowedMoves() override;
};