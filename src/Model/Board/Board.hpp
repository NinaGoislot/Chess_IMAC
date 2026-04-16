#pragma once

#include <array>
#include <vector>
#include "Case.hpp"

struct MoveAttempt;

// Owns board state only. Pieces are owned by Player and referenced here by raw pointers
class Board {
public:
    static constexpr int SIZE = 8;

    // Result of one validated move application on board state.
    struct MoveResult {
        bool     moved       = false;
        bool     isPromotion = false;
        Vector2D from{};
        Vector2D to{};
        Piece*   movedPiece    = nullptr;
        Piece*   capturedPiece = nullptr;
    };

    // Constructors
    Board();

    // Getters
    Case&                 getCase(int x, int y);
    const Case&           getCase(int x, int y) const;
    std::vector<Vector2D> getLegalMovesFrom(Vector2D from, PieceColor currentTurn) const;

    // Move application
    bool       canMove(Vector2D from, Vector2D to, PieceColor currentTurn) const;
    MoveResult tryMove(Vector2D from, Vector2D to, PieceColor currentTurn);
    MoveResult tryMove(const MoveAttempt& attempt);

    // Resets board state
    void clear();

    // Helpers
    bool isInside(Vector2D pos) const;
    bool isEmpty(Vector2D pos) const;
    bool isEnemy(Vector2D pos, PieceColor color) const;

private:
    std::array<std::array<Case, SIZE>, SIZE> _cases;
};
