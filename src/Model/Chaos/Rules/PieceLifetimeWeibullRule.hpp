#pragma once

#include <unordered_map>
#include "Model/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Gives pieces random finite lifetimes sampled with a Weibull law
class PieceLifetimeWeibullRule : public IChaosRule {
public:
    // Constructors
    explicit PieceLifetimeWeibullRule(const ChaosOptions* options);

    // Callback
    void onGameSetup(ChaosRuleContext& context) override;
    bool beforeMove(ChaosMoveContext& context) override;

private:
    int  sampleLifetime(std::mt19937& rng) const;
    void initializeMissingPieces(ChaosRuleContext& context);

    // Parameters
    const ChaosOptions*                   _options = nullptr;
    std::unordered_map<const Piece*, int> _remainingTurns;
};
