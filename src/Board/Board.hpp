#pragma once

#include <array>
#include <vector>
#include "Case.hpp"

// Owns board state only. Pieces are owned by Player and referenced here by raw pointers.
class Board {
public:
    static constexpr int SIZE = 8;

    // Result of one validated move application on board state.
    struct MoveResult {
        bool     moved         = false;
        bool     isPromotion   = false;
        Vector2D from{};
        Vector2D to{};
        Piece*   movedPiece    = nullptr; // non-owning
        Piece*   capturedPiece = nullptr; // non-owning
    };

    // Constructors
    Board();

    // Getters
    Case&                               getCase(int x, int y);
    const Case&                         getCase(int x, int y) const;
    // Computes legal target squares for one piece at source position.
    std::vector<Vector2D>               getLegalMovesFrom(Vector2D from, PieceColor currentTurn) const;
    // Checks if a move is currently legal according to chess piece rules.
    bool                                canMove(Vector2D from, Vector2D to, PieceColor currentTurn) const;
    // Applies one legal move and returns what happened on board.
    MoveResult                          tryMove(Vector2D from, Vector2D to, PieceColor currentTurn);
    // Resets board content and transient state.
    void                                clear();

    bool isInside(Vector2D pos) const;
    bool isEmpty(Vector2D pos) const;
    bool isEnemy(Vector2D pos, PieceColor color) const;

private:
    // 8x8 grid of board cases.
    std::array<std::array<Case, SIZE>, SIZE> _cases;
};
