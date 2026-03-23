#pragma once
#include "Piece.hpp"

class Pawn : public Piece {
public:
    Pawn();
    ~Pawn() override = default;

    explicit Pawn(ImTextureID texture);
    Pawn(PieceColor color, ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};