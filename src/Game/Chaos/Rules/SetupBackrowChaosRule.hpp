#pragma once

#include "Game/Chaos/IChaosRule.hpp"

struct ChaosOptions;

class SetupBackrowChaosRule : public IChaosRule {
public:
    explicit SetupBackrowChaosRule(const ChaosOptions* options);

    void onGameSetup(ChaosRuleContext& context) override;

private:
    const ChaosOptions* _options = nullptr;
};
