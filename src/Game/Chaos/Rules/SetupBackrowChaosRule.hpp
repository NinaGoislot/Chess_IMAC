#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Randomizes initial back-row setup
class SetupBackrowChaosRule : public IChaosRule {
public:
    // Constructors
    explicit SetupBackrowChaosRule(const ChaosOptions* options);

    // Callback
    void onGameSetup(ChaosRuleContext& context) override;

private:
    // Parameters
    const ChaosOptions* _options = nullptr;
};
