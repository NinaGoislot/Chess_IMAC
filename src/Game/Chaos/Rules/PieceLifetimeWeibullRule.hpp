#pragma once

#include <unordered_map>
#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

class PieceLifetimeWeibullRule : public IChaosRule {
public:
    explicit PieceLifetimeWeibullRule(const ChaosOptions* options);

    void onGameSetup(ChaosRuleContext& context) override;
    bool beforeMove(ChaosMoveContext& context) override;

private:
    int  sampleLifetime(std::mt19937& rng) const;
    void initializeMissingPieces(ChaosRuleContext& context);

    const ChaosOptions*                   _options = nullptr;
    std::unordered_map<const Piece*, int> _remainingTurns;
};
