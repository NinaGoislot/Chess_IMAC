#pragma once
#include "Piece.hpp"

// Pawn piece implementation with pawn-specific movement rules.
class Pawn : public Piece {
public:
    // Constructors
    Pawn();
    ~Pawn() override = default;

    explicit Pawn(ImTextureID texture);
    Pawn(PieceColor color, ImTextureID texture);

    // Render function: draws pawn sprite for 2D mode.
    void draw(const settings& gameSettings) override;

private:
    // Updates legal moves according to pawn rules and board state.
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
