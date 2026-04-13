#include "Game/Chaos/Rules/PieceLifetimeWeibullRule.hpp"
#include <algorithm>
#include "Game/Chaos/ChaosMode.hpp"
#include "probabimac/WeibullDistribution.hpp"
#include "utilities/Vector2D.hpp"


namespace {
void removePieceAt(Board& board, std::array<Player, 2>& players, int x, int y)
{
    if (!board.isInside(Vector2D(static_cast<float>(x), static_cast<float>(y))))
        return;

    Case& square = board.getCase(x, y);
    if (!square.hasPiece())
        return;

    Piece* piece = square.takePiece();
    if (piece == nullptr)
        return;

    if (players[0].owns(*piece))
        players[0].removePiece(*piece);
    else if (players[1].owns(*piece))
        players[1].removePiece(*piece);
}
} // namespace

PieceLifetimeWeibullRule::PieceLifetimeWeibullRule(const ChaosOptions* options)
    : _options(options)
{
}

void PieceLifetimeWeibullRule::onGameSetup(ChaosRuleContext& context)
{
    _remainingTurns.clear();
    initializeMissingPieces(context);
}

bool PieceLifetimeWeibullRule::beforeMove(ChaosMoveContext& context)
{
    if (_options == nullptr || !_options->enableWeibullLifetime)
        return true;

    Piece* piece = context.attempt.piece;
    if (piece == nullptr)
        return true;

    if (!_remainingTurns.contains(piece))
    {
        _remainingTurns[piece] = sampleLifetime(context.rng);
    }

    auto it = _remainingTurns.find(piece);
    if (it == _remainingTurns.end())
        return true;

    --it->second;
    if (it->second > 0)
        return true;

    if (context.skipTurnRequested != nullptr)
    {
        *context.skipTurnRequested = true;
    }

    removePieceAt(context.board, context.players, context.attempt.fromX, context.attempt.fromY);
    context.history.push_back("Chaos: la piece se desintegre apres usage (Weibull).");
    return false;
}

int PieceLifetimeWeibullRule::sampleLifetime(std::mt19937& rng) const
{
    if (_options == nullptr)
        return 5;

    WeibullDistribution weibull(_options->weibullShape, _options->weibullScale);
    const int           sampled = weibull(rng);
    return std::max(2, sampled);
}

void PieceLifetimeWeibullRule::initializeMissingPieces(ChaosRuleContext& context)
{
    for (int x = 0; x < Board::SIZE; ++x)
    {
        for (int y = 0; y < Board::SIZE; ++y)
        {
            const Case& square = context.board.getCase(x, y);
            if (!square.hasPiece())
                continue;

            Piece* piece = square.getPiece();
            if (!_remainingTurns.contains(piece))
            {
                _remainingTurns[piece] = sampleLifetime(context.rng);
            }
        }
    }
}
