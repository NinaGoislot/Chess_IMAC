#pragma once
#include "Piece.hpp"

// King piece implementation with one-step king movement rules.
class King : public Piece {
public:
    // Constructors
    King();
    ~King() override = default;

    explicit King(ImTextureID texture);
    King(PieceColor color, ImTextureID texture);

    // Render function: draws king sprite for 2D mode.
    void draw(const settings& gameSettings) override;

private:
    // Updates legal moves according to king rules and board state.
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
