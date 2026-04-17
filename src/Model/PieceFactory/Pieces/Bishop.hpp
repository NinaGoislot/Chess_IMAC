#pragma once
#include "Model/PieceFactory/Piece.hpp"

class Bishop : public Piece {
public:
    // Constructors
    Bishop();
    ~Bishop() override = default;
    Bishop(const Bishop&) = delete;
    Bishop& operator=(const Bishop&) = delete;
    Bishop(Bishop&&) = delete;
    Bishop& operator=(Bishop&&) = delete;

    explicit Bishop(ImTextureID texture);
    Bishop(PieceColor color, ImTextureID texture);

    // Render
    void draw(const settings& gameSettings) override;

private:
    void updateAllowedMoves(const Board& board, Vector2D position) override;
};
