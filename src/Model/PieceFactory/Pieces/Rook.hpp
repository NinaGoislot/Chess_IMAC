#pragma once
#include "Model/PieceFactory/Piece.hpp"

class Rook : public Piece {
public:
    // Constructors
    Rook();
    ~Rook() override = default;

    explicit Rook(ImTextureID texture);
    Rook(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
