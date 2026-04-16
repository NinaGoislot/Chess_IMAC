#pragma once

#include <array>
#include <optional>
#include <random>
#include <utility>
#include <vector>
#include "Model/Board/Board.hpp"
#include "Model/Match/Player.hpp"
#include "utilities/MoveAttempt.hpp"

// Shared context passed to chaos rules during game/turn lifecycle callbacks
struct ChaosRuleContext {
    Board&                              board;
    std::array<Player, 2>&              players;
    std::vector<std::string>&           history;
    std::mt19937&                       rng;
    PieceColor                          currentTurn;
    std::optional<std::pair<int, int>>* kirbyPosition = nullptr;
};

// Shared context passed to chaos rules during move validation callbacks
struct ChaosMoveContext {
    MoveAttempt&                        attempt;
    Board&                              board;
    std::array<Player, 2>&              players;
    std::vector<std::string>&           history;
    std::mt19937&                       rng;
    bool*                               skipTurnRequested = nullptr;
    std::optional<std::pair<int, int>>* kirbyPosition     = nullptr;
};

// Interface implemented by every chaos gameplay rule
class IChaosRule {
public:
    virtual ~IChaosRule() = default;

    // Callback
    virtual void onGameSetup(ChaosRuleContext& context) {}
    virtual void onTurnStart(ChaosRuleContext& context) {}
    virtual void onTurnEnd(ChaosRuleContext& context) {}

    // Functions
    virtual bool beforeMove(ChaosMoveContext& context)
    {
        (void)context;
        return true;
    }
};
