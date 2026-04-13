#pragma once

#include <array>
#include <functional>
#include <optional>
#include "Case.hpp"
#include "utilities/MoveAttempt.hpp"

class Board {
public:
    static constexpr int SIZE = 8;

    using BeforeMoveHook = std::function<bool(MoveAttempt&)>;

    struct PendingPromotionInfo {
        int        x     = 0;
        int        y     = 0;
        PieceColor color = PieceColor::White;
    };

    Board();

    Case&                               getCase(int x, int y);
    const Case&                         getCase(int x, int y) const;
    bool                                onCaseClicked(int x, int y, const PieceColor& currentTurn);
    bool                                hasPendingPromotion() const;
    std::optional<PendingPromotionInfo> getPendingPromotion() const;
    bool                                confirmPromotion(Piece* promotedPiece);
    void                                cancelPendingPromotion();
    void                                setBeforeMoveHook(BeforeMoveHook hook);
    void                                clearBeforeMoveHook();
    void                                clear();

    bool isInside(Vector2D pos) const;
    bool isEmpty(Vector2D pos) const;
    bool isEnemy(Vector2D pos, PieceColor color) const;

private:
    struct PendingPromotionState {
        Case*      from     = nullptr;
        Case*      to       = nullptr;
        Piece*     pawn     = nullptr;
        Piece*     captured = nullptr;
        PieceColor color    = PieceColor::White;
    };

    Case*                                    _selectedCase = nullptr;
    std::array<std::array<Case, SIZE>, SIZE> _cases;
    std::optional<PendingPromotionState>     _pendingPromotion;
    BeforeMoveHook                           _beforeMoveHook;

    void clearHighlights();
};