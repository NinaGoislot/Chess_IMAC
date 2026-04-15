#pragma once

#include "Game/Chaos/IChaosRule.hpp"

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
};
