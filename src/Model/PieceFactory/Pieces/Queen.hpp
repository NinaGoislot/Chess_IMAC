#pragma once
#include "Model/PieceFactory/Piece.hpp"

class Queen : public Piece {
public:
    // Constructors
    Queen();
    ~Queen() override = default;

    explicit Queen(ImTextureID texture);
    Queen(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
