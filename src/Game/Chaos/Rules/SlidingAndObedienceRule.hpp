#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Alters movement with sliding behavior and probabilistic obedience checks.
class SlidingAndObedienceRule : public IChaosRule {
public:
    // Constructors
    explicit SlidingAndObedienceRule(const ChaosOptions* options);

    // Callback: adjusts or rejects move attempts based on rule logic.
    bool beforeMove(ChaosMoveContext& context) override;

private:
    // Computes obedience probability for a specific piece.
    double obedienceProbabilityFor(const Piece* piece) const;
    // Returns whether the piece type uses sliding movement.
    bool   isSlidingPiece(const Piece* piece) const;

    // Shared options pointer (not owned).
    const ChaosOptions* _options = nullptr;
};
