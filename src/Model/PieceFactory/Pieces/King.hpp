#pragma once
#include "Model/PieceFactory/Piece.hpp"

class King : public Piece {
public:
    // Constructors
    King();
    ~King() override = default;
    King(const King&) = delete;
    King& operator=(const King&) = delete;
    King(King&&) = delete;
    King& operator=(King&&) = delete;

    explicit King(ImTextureID texture);
    King(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
