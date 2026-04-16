#include "Model/Chaos/Rules/PieceLifetimeWeibullRule.hpp"
#include <algorithm>
#include "Model/Chaos/ChaosMode.hpp"
#include "probabimac/WeibullDistribution.hpp"
#include "utilities/Vector2D.hpp"

namespace {
void removePieceAt(Board& board, std::array<Player, 2>& players, int x, int y)
{
    if (!board.isInside(Vector2D(static_cast<float>(x), static_cast<float>(y))))
        return;

    Case& square = board.getCase(x, y);
    if (!square.getHasPiece())
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
}

/**
 * Called before a move is attempted
 * Initializes lifetime for the piece if not already done, then decrements it
 *
 * @param context : the chaos move context
 * @return : always true to allow the move (if no errors)
 */
bool PieceLifetimeWeibullRule::beforeMove(ChaosMoveContext& context)
{
    if (_options == nullptr || !_options->enableWeibullLifetime)
        return true;

    Piece* piece = context.attempt.piece;
    if (piece == nullptr)
        return true;

    // Kings are exempt from chaos lifetime decay
    if (piece->getType() == PieceType::King)
    {
        _remainingTurns.erase(piece);
        return true;
    }

    // Initialize lifetime on first use if not already done
    if (!_remainingTurns.contains(piece))
    {
        _remainingTurns[piece] = sampleLifetime(context.rng);
    }

    // Decrement lifetime counter
    auto it = _remainingTurns.find(piece);
    if (it != _remainingTurns.end())
    {
        --it->second;
    }

    return true;
}

/**
 * Called immediately after a move is executed
 * Removes pieces that have reached end of lifetime
 *
 * @param context : the chaos move context
 */
void PieceLifetimeWeibullRule::afterMove(ChaosMoveContext& context)
{
    // cheeck if law is enableed
    if (_options == nullptr || !_options->enableWeibullLifetime)
        return;

    Piece* piece = context.attempt.piece;
    if (piece == nullptr)
        return;

    // Check if piece has a lifetime counter (it should, but just in case)
    auto it = _remainingTurns.find(piece);
    if (it == _remainingTurns.end())
        return;

    // Check if piece has reached end of life
    if (it->second <= 0)
    {
        removePieceAt(context.board, context.players, context.attempt.toX, context.attempt.toY);
        _remainingTurns.erase(piece);
        context.history.push_back("Chaos: une piece à rendue l'âme apres usage (Weibull). BoooooooM ! ");
    }
}

/**
 * Samples a lifetime for a piece using the Weibull distribution
 *
 * @param rng : the random number generator
 * @return : the sampled lifetime
 */
int PieceLifetimeWeibullRule::sampleLifetime(std::mt19937& rng) const
{
    if (_options == nullptr)
        return 5;

    WeibullDistribution weibull(_options->weibullShape, _options->weibullScale);
    const int           sampled = weibull(rng);
    return std::max(2, sampled);
}
