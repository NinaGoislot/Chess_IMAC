#pragma once
#include "Model/PieceFactory/Piece.hpp"

class Knight : public Piece {
public:
    // Constructors
    Knight();
    ~Knight() override = default;
    Knight(const Knight&) = delete;
    Knight& operator=(const Knight&) = delete;
    Knight(Knight&&) = delete;
    Knight& operator=(Knight&&) = delete;

    explicit Knight(ImTextureID texture);
    Knight(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
