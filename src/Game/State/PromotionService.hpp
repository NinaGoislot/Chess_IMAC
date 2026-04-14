#pragma once

#include <array>
#include <optional>
#include "Board/Board.hpp"
#include "Game/Player.hpp"

// Owns deferred pawn promotion state and its resolution.
class PromotionService {
public:
    // Promotion tile and side shown in UI while waiting for a choice.
    struct PendingPromotionInfo {
        int        x     = 0;
        int        y     = 0;
        PieceColor color = PieceColor::White;
    };

    // Getters
    bool                              getHasPending() const;
    std::optional<PendingPromotionInfo> getPendingInfo() const;

    // Lifecycle
    void                              clear();

    // Promotion flow
    bool start(const Board::MoveResult& result, PieceColor currentTurn);
    bool resolve(Board& board, std::array<Player, 2>& players, PieceType type);
    void cancel(Board& board);

private:
    // Internal data kept between pawn move and promotion choice.
    struct PendingPromotionState {
        Vector2D   from{};
        Vector2D   to{};
        Piece*     pawn          = nullptr; // non-owning, owned by Player
        Piece*     capturedPiece = nullptr; // non-owning, owned by Player
        PieceColor color         = PieceColor::White;
    };

    // Removes captured piece from its owner set.
    static void consumeCapturedPiece(std::array<Player, 2>& players, Piece* capturedPiece);

    // Pending promotion, present only while awaiting player choice.
    std::optional<PendingPromotionState> _pending;
};
