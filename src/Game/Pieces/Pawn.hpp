#pragma once
#include "Piece.hpp"

class Pawn : public Piece {
public:
    Pawn();
    ~Pawn() override = default;

    explicit Pawn(ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void setAllowedMoves() override;
};