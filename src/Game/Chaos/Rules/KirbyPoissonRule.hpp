#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Spawns and controls Kirby events using a Poisson-style process.
class KirbyPoissonRule : public IChaosRule {
public:
    // Constructors
    explicit KirbyPoissonRule(const ChaosOptions* options);

    // Callback: resets internal Kirby state on game setup.
    void onGameSetup(ChaosRuleContext& context) override;
    // Callback: updates Kirby spawn/visibility each turn start.
    void onTurnStart(ChaosRuleContext& context) override;
    // Callback: lets Kirby interact with piece movement.
    bool beforeMove(ChaosMoveContext& context) override;

private:
    // Shared options pointer (not owned).
    const ChaosOptions* _options               = nullptr;
    // Number of Kirby spawns since game start.
    int                 _spawnCount            = 0;
    // Remaining turns Kirby stays visible.
    int                 _remainingTurnsVisible = 0;
};
