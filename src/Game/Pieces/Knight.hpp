#pragma once
#include "Piece.hpp"

class Knight : public Piece {
public:
    // Constructors
    Knight();
    ~Knight() override = default;

    explicit Knight(ImTextureID texture);
    Knight(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
