#pragma once

#include <array>
#include <optional>
#include "Board/Board.hpp"
#include "Game/Player.hpp"

class PromotionService {
public:
    // Promotion tile and side shown in UI while waiting for a choice
    struct PendingPromotionInfo {
        int        x     = 0;
        int        y     = 0;
        PieceColor color = PieceColor::White;
    };

    // Getters
    bool                                getHasPending() const;
    std::optional<PendingPromotionInfo> getPendingInfo() const;

    // Lifecycle
    void clear();

    // Promotion flow
    bool start(const Board::MoveResult& result, PieceColor currentTurn);
    bool resolve(Board& board, std::array<Player, 2>& players, PieceType type);
    void cancel(Board& board);

private:
    struct PendingPromotionState {
        Vector2D   from{};
        Vector2D   to{};
        Piece*     pawn          = nullptr; // owned by Player
        Piece*     capturedPiece = nullptr; // owned by Player
        PieceColor color         = PieceColor::White;
    };
    static void                          consumeCapturedPiece(std::array<Player, 2>& players, Piece* capturedPiece);
    std::optional<PendingPromotionState> _pending;
};
