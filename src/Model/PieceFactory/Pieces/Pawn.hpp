#pragma once
#include "Model/PieceFactory/Piece.hpp"

class Pawn : public Piece {
public:
    // Constructors
    Pawn();
    ~Pawn() override = default;
    Pawn(const Pawn&) = delete;
    Pawn& operator=(const Pawn&) = delete;
    Pawn(Pawn&&) = delete;
    Pawn& operator=(Pawn&&) = delete;

    explicit Pawn(ImTextureID texture);
    Pawn(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
