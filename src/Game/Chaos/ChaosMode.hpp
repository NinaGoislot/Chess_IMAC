#pragma once

#include <array>
#include <memory>
#include <optional>
#include <random>
#include <utility>
#include <vector>
#include "Game/Chaos/IChaosRule.hpp"
#include "utilities/MoveAttempt.hpp"

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

class ChaosMode {
public:
    ChaosMode();

    void                setEnabled(bool enabled);
    bool                isEnabled() const { return _enabled; }
    ChaosOptions&       optionsMutable() { return _options; }
    const ChaosOptions& options() const { return _options; }

    void                               onGameSetup(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    void                               onTurnStart(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    void                               onTurnEnd(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn);
    bool                               beforeMove(MoveAttempt& attempt, Board& board, std::array<Player, 2>& players, std::vector<std::string>& history);
    bool                               consumeSkipTurnRequested();
    bool                               hasKirbyAt(int x, int y) const;
    std::optional<std::pair<int, int>> kirbyPosition() const { return _kirbyPosition; }

private:
    bool                                     _enabled           = false;
    bool                                     _skipTurnRequested = false;
    std::mt19937                             _rng;
    ChaosOptions                             _options;
    std::optional<std::pair<int, int>>       _kirbyPosition;
    std::vector<std::unique_ptr<IChaosRule>> _rules;
};
