#pragma once
#include "Piece.hpp"

// Queen piece implementation with combined rook+bishop movement.
class Queen : public Piece {
public:
    // Constructors
    Queen();
    ~Queen() override = default;

    explicit Queen(ImTextureID texture);
    Queen(PieceColor color, ImTextureID texture);

    // Render function: draws queen sprite for 2D mode.
    void draw(const settings& gameSettings) override;

private:
    // Updates legal moves according to queen rules and board state.
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
