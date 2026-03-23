#pragma once
#include "Piece.hpp"

class King : public Piece {
public:
    King();
    ~King() override = default;

    explicit King(ImTextureID texture);
    King(PieceColor color, ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};