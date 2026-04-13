#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

// Randomizes initial back-row setup according to chaos options.
class SetupBackrowChaosRule : public IChaosRule {
public:
    // Constructors
    explicit SetupBackrowChaosRule(const ChaosOptions* options);

    // Callback: mutates board setup during game initialization.
    void onGameSetup(ChaosRuleContext& context) override;

private:
    // Shared options pointer (not owned).
    const ChaosOptions* _options = nullptr;
};
