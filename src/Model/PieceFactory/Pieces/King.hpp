#pragma once
#include "Model/PieceFactory/Piece.hpp"

class King : public Piece {
public:
    // Constructors
    King();
    ~King() override = default;

    explicit King(ImTextureID texture);
    King(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
