#pragma once
#include "Piece.hpp"

class Rook : public Piece {
public:
    Rook();
    ~Rook() override = default;

    explicit Rook(ImTextureID texture);
    Rook(PieceColor color, ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};