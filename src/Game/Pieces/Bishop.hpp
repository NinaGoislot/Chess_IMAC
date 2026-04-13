#pragma once
#include "Piece.hpp"

// Bishop piece implementation with diagonal sliding moves.
class Bishop : public Piece {
public:
    // Constructors
    Bishop();
    ~Bishop() override = default;

    explicit Bishop(ImTextureID texture);
    Bishop(PieceColor color, ImTextureID texture);

    // Render function: draws bishop sprite for 2D mode.
    void draw(const settings& gameSettings) override;

private:
    // Updates legal moves according to bishop rules and board state.
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
