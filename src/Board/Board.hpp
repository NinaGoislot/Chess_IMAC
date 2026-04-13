#pragma once

#include <array>
#include <functional>
#include <optional>
#include "Case.hpp"
#include "utilities/MoveAttempt.hpp"

// Owns the 8x8 chessboard, selection state, and promotion/move validation flow.
class Board {
public:
    static constexpr int SIZE = 8;

    using BeforeMoveHook = std::function<bool(MoveAttempt&)>;

    // Snapshot of a pending pawn promotion target.
    struct PendingPromotionInfo {
        int        x     = 0;
        int        y     = 0;
        PieceColor color = PieceColor::White;
    };

    // Constructors
    Board();

    // Getters
    Case&                               getCase(int x, int y);
    const Case&                         getCase(int x, int y) const;
    // Handles board click and tries to execute move logic.
    bool                                onCaseClicked(int x, int y, const PieceColor& currentTurn);
    bool                                getHasPendingPromotion() const;
    std::optional<PendingPromotionInfo> getPendingPromotion() const;
    // Confirms promotion choice with created piece.
    bool                                confirmPromotion(Piece* promotedPiece);
    // Cancels current pending promotion and restores previous state.
    void                                cancelPendingPromotion();
    // Setters
    void                                setBeforeMoveHook(BeforeMoveHook hook);
    // Clears the previously installed before-move hook.
    void                                clearBeforeMoveHook();
    // Resets board content and transient state.
    void                                clear();

    bool isInside(Vector2D pos) const;
    bool isEmpty(Vector2D pos) const;
    bool isEnemy(Vector2D pos, PieceColor color) const;
    bool isSelectedCase(int x, int y) const;

private:
    // Internal state kept while waiting for promotion confirmation.
    struct PendingPromotionState {
        Case*      from     = nullptr;
        Case*      to       = nullptr;
        Piece*     pawn     = nullptr;
        Piece*     captured = nullptr;
        PieceColor color    = PieceColor::White;
    };

    // Currently selected tile, if any.
    Case*                                    _selectedCase = nullptr;
    // 8x8 grid of board cases.
    std::array<std::array<Case, SIZE>, SIZE> _cases;
    // Deferred promotion state, active only after pawn reaches last rank.
    std::optional<PendingPromotionState>     _pendingPromotion;
    // Optional callback executed before finalizing a move.
    BeforeMoveHook                           _beforeMoveHook;

    // Clears UI highlights from all cases.
    void clearHighlights();
};
