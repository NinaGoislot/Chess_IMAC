#pragma once

#include <array>
#include <memory>
#include <optional>
#include <random>
#include <utility>
#include <vector>
#include "Model/Chaos/IChaosRule.hpp"
#include "utilities/MoveAttempt.hpp"

// Runtime option
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

// Orchestrates all registered chaos rules
class ChaosMode {
public:
    // Constructors
    ChaosMode();

    // Getters
    bool                               getHasKirbyAt(int x, int y) const;
    std::optional<std::pair<int, int>> getKirbyPosition() const { return _kirbyPosition; }

    // Setters
    void                setEnabled(bool enabled);
    bool                getIsEnabled() const { return _enabled; }
    ChaosOptions&       getOptionsMutable() { return _options; }
    const ChaosOptions& getOptions() const { return _options; }

    // Lifecycle callback
    void onGameSetup(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    void onTurnStart(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    void onTurnEnd(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    // Move callback
    bool beforeMove(MoveAttempt& attempt, Board& board, std::array<Player, 2>& players, std::vector<std::string>& history);
    void afterMove(MoveAttempt& attempt, Board& board, std::array<Player, 2>& players, std::vector<std::string>& history);
    bool consumeSkipTurnRequested();

private:
    // parameters
    bool                                     _enabled           = false;
    bool                                     _skipTurnRequested = false;
    std::mt19937                             _rng;
    ChaosOptions                             _options;
    std::optional<std::pair<int, int>>       _kirbyPosition;
    std::vector<std::unique_ptr<IChaosRule>> _rules;
};
