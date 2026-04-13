#pragma once
#include "Piece.hpp"

// Knight piece implementation with L-shaped jumps.
class Knight : public Piece {
public:
    // Constructors
    Knight();
    ~Knight() override = default;

    explicit Knight(ImTextureID texture);
    Knight(PieceColor color, ImTextureID texture);

    // Render function: draws knight sprite for 2D mode.
    void draw(const settings& gameSettings) override;

private:
    // Updates legal moves according to knight rules and board state.
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
