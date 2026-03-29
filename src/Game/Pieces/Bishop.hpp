#pragma once
#include "Piece.hpp"

class Bishop : public Piece {
public:
    Bishop();
    ~Bishop() override = default;

    explicit Bishop(ImTextureID texture);
    Bishop(PieceColor color, ImTextureID texture);

    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};