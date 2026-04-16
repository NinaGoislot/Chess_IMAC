#pragma once

#include "Model/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Alters movement with sliding behavior, obedience concept
class SlidingAndObedienceRule : public IChaosRule {
public:
    // Constructors
    explicit SlidingAndObedienceRule(const ChaosOptions* options);

    // Callback
    bool beforeMove(ChaosMoveContext& context) override;

private:
    double obedienceProbabilityFor(const Piece* piece) const;
    bool   isSlidingPiece(const Piece* piece) const;

    // Parameters
    const ChaosOptions* _options = nullptr;

    const double PAWN_OBEDIENCE_PROBA   = 0.95;
    const double KNIGHT_OBEDIENCE_PROBA = 1.0;
    const double KING_OBEDIENCE_PROBA   = 0.95;
    const double QUEEN_OBEDIENCE_PROBA  = 0.65;
    const double ROOK_OBEDIENCE_PROBA   = 0.80;
    const double BISHOP_OBEDIENCE_PROBA = 0.80;
};
