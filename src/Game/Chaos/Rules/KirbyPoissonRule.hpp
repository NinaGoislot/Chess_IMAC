#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

class KirbyPoissonRule : public IChaosRule {
public:
    explicit KirbyPoissonRule(const ChaosOptions* options);

    void onGameSetup(ChaosRuleContext& context) override;
    void onTurnStart(ChaosRuleContext& context) override;
    bool beforeMove(ChaosMoveContext& context) override;

private:
    const ChaosOptions* _options               = nullptr;
    int                 _spawnCount            = 0;
    int                 _remainingTurnsVisible = 0;
};
