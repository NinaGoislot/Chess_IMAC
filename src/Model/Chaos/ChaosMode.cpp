#include "Model/Chaos/ChaosMode.hpp"
#include "Model/Chaos/Rules/KirbyPoissonRule.hpp"
#include "Model/Chaos/Rules/PieceLifetimeWeibullRule.hpp"
#include "Model/Chaos/Rules/SetupBackrowChaosRule.hpp"
#include "Model/Chaos/Rules/SlidingAndObedienceRule.hpp"

ChaosMode::ChaosMode()
    : _rng(std::random_device{}())
{
    _rules.push_back(std::make_unique<SetupBackrowChaosRule>(&_options));
    _rules.push_back(std::make_unique<SlidingAndObedienceRule>(&_options));
    _rules.push_back(std::make_unique<PieceLifetimeWeibullRule>(&_options));
    _rules.push_back(std::make_unique<KirbyPoissonRule>(&_options));
}

void ChaosMode::setEnabled(bool enabled)
{
    _enabled = enabled;
    if (!_enabled)
    {
        _kirbyPosition = std::nullopt;
    }
}

void ChaosMode::onGameSetup(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn)
{
    if (!_enabled)
        return;

    ChaosRuleContext context{board, players, history, _rng, currentTurn, &_kirbyPosition};
    for (auto& rule : _rules)
        rule->onGameSetup(context);
}

void ChaosMode::onTurnStart(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn)
{
    if (!_enabled)
        return;

    ChaosRuleContext context{board, players, history, _rng, currentTurn, &_kirbyPosition};
    for (auto& rule : _rules)
        rule->onTurnStart(context);
}

void ChaosMode::onTurnEnd(Board& board, std::array<Player, 2>& players, std::vector<std::string>& history, PieceColor currentTurn)
{
    if (!_enabled)
        return;

    ChaosRuleContext context{board, players, history, _rng, currentTurn, &_kirbyPosition};
    for (auto& rule : _rules)
        rule->onTurnEnd(context);
}

bool ChaosMode::beforeMove(MoveAttempt& attempt, Board& board, std::array<Player, 2>& players, std::vector<std::string>& history)
{
    if (!_enabled)
        return true;

    _skipTurnRequested = false;

    ChaosMoveContext context{attempt, board, players, history, _rng, &_skipTurnRequested, &_kirbyPosition};
    for (auto& rule : _rules)
    {
        if (!rule->beforeMove(context))
            return false;
    }

    return true;
}

void ChaosMode::afterMove(MoveAttempt& attempt, Board& board, std::array<Player, 2>& players, std::vector<std::string>& history)
{
    if (!_enabled)
        return;

    ChaosMoveContext context{attempt, board, players, history, _rng, nullptr, &_kirbyPosition};
    for (auto& rule : _rules)
        rule->afterMove(context);
}

bool ChaosMode::consumeSkipTurnRequested()
{
    const bool requested = _skipTurnRequested;
    _skipTurnRequested   = false;
    return requested;
}

bool ChaosMode::getHasKirbyAt(int x, int y) const
{
    return _kirbyPosition.has_value() && _kirbyPosition->first == x && _kirbyPosition->second == y;
}
