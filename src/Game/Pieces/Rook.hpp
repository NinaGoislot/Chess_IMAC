#pragma once
#include "Piece.hpp"

// Rook piece implementation with horizontal and vertical sliding moves.
class Rook : public Piece {
public:
    // Constructors
    Rook();
    ~Rook() override = default;

    explicit Rook(ImTextureID texture);
    Rook(PieceColor color, ImTextureID texture);

    // Render function: draws rook sprite for 2D mode.
    void draw(const settings& gameSettings) override;

private:
    // Updates legal moves according to rook rules and board state.
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
