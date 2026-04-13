#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

class SlidingAndObedienceRule : public IChaosRule {
public:
    explicit SlidingAndObedienceRule(const ChaosOptions* options);

    bool beforeMove(ChaosMoveContext& context) override;

private:
    double obedienceProbabilityFor(const Piece* piece) const;
    bool   isSlidingPiece(const Piece* piece) const;

    const ChaosOptions* _options = nullptr;
};
