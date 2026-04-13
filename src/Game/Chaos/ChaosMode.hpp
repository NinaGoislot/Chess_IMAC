#pragma once

#include <array>
#include <memory>
#include <optional>
#include <random>
#include <utility>
#include <vector>
#include "Game/Chaos/IChaosRule.hpp"
#include "utilities/MoveAttempt.hpp"

// Runtime options controlling which chaos rules are active and their parameters.
struct ChaosOptions {
    bool enableWeibullLifetime              = true;
    bool enableBernoulliBackrowAndShuffle   = true;
    bool enableKirbyPoissonUniform          = true;
    bool enableGeometricSlidingAndObedience = true;

    double weibullShape                = 1.45;
    double weibullScale                = 9.0;
    double pawnBackrowProbability      = 0.20;
    double kirbySpawnLambda            = 0.08;
    double slidingEarlyStopProbability = 0.20;
};

// Orchestrates all registered chaos rules during game and move lifecycle.
class ChaosMode {
public:
    // Constructors
    ChaosMode();

    // Setters
    void                setEnabled(bool enabled);
    bool                getIsEnabled() const { return _enabled; }
    ChaosOptions&       getOptionsMutable() { return _options; }
    const ChaosOptions& getOptions() const { return _options; }

    // Lifecycle callback: forwards game setup event to active rules.
    void                               onGameSetup(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    // Lifecycle callback: forwards turn-start event to active rules.
    void                               onTurnStart(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    // Lifecycle callback: forwards turn-end event to active rules.
    void                               onTurnEnd(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    // Move callback: lets active rules alter or cancel a move attempt.
    bool                               beforeMove(MoveAttempt& attempt, Board& board, std::array<Player, 2>& players, std::vector<std::string>& history);
    bool                               consumeSkipTurnRequested();
    bool                               getHasKirbyAt(int x, int y) const;
    std::optional<std::pair<int, int>> getKirbyPosition() const { return _kirbyPosition; }

private:
    // Global chaos activation flag.
    bool                                     _enabled           = false;
    // One-turn flag asking turn manager to skip next move.
    bool                                     _skipTurnRequested = false;
    // RNG used by probabilistic chaos rules.
    std::mt19937                             _rng;
    // Active configuration values for chaos behavior.
    ChaosOptions                             _options;
    // Current Kirby board position if spawned.
    std::optional<std::pair<int, int>>       _kirbyPosition;
    // Installed chaos rules executed each lifecycle step.
    std::vector<std::unique_ptr<IChaosRule>> _rules;
};
