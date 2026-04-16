#pragma once

#include "Model/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Spawns and controls Kirby events using a Poisson-law
class KirbyPoissonRule : public IChaosRule {
public:
    // Constructors
    explicit KirbyPoissonRule(const ChaosOptions* options);

    // Callback
    void onGameSetup(ChaosRuleContext& context) override;
    void onTurnStart(ChaosRuleContext& context) override;
    bool beforeMove(ChaosMoveContext& context) override;

private:
    // Parameters
    const ChaosOptions* _options               = nullptr;
    int                 _spawnCount            = 0;
    int                 _remainingTurnsVisible = 0;
};
