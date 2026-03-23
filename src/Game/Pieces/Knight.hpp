#pragma once
#include "Piece.hpp"

class Knight : public Piece {
public:
    Knight();
    ~Knight() override = default;

    explicit Knight(ImTextureID texture);
    Knight(PieceColor color, ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void setAllowedMoves(const Board& board, Vector2D position) override;
};