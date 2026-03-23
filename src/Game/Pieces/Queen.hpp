#pragma once
#include "Piece.hpp"

class Queen : public Piece {
public:
    Queen();
    ~Queen() override = default;

    explicit Queen(ImTextureID texture);
    Queen(PieceColor color, ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};