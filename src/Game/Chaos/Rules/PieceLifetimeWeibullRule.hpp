#pragma once

#include <unordered_map>
#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Gives pieces random finite lifetimes sampled from a Weibull distribution.
class PieceLifetimeWeibullRule : public IChaosRule {
public:
    // Constructors
    explicit PieceLifetimeWeibullRule(const ChaosOptions* options);

    // Callback: initializes lifetime values at game start.
    void onGameSetup(ChaosRuleContext& context) override;
    // Callback: decrements lifetimes and resolves expirations before move.
    bool beforeMove(ChaosMoveContext& context) override;

private:
    // Samples number of turns a piece may survive.
    int  sampleLifetime(std::mt19937& rng) const;
    // Ensures newly seen pieces receive an initial lifetime.
    void initializeMissingPieces(ChaosRuleContext& context);

    // Shared options pointer (not owned).
    const ChaosOptions*                   _options = nullptr;
    // Remaining turns per tracked piece.
    std::unordered_map<const Piece*, int> _remainingTurns;
};
