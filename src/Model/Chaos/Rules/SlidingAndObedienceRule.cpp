#include "Model/Chaos/Rules/SlidingAndObedienceRule.hpp"
#include <algorithm>
#include <cmath>
#include "Model/Chaos/ChaosMode.hpp"
#include "probabimac/BernoulliDistribution.hpp"
#include "probabimac/GeometricDistribution.hpp"


SlidingAndObedienceRule::SlidingAndObedienceRule(const ChaosOptions* options)
    : _options(options)
{
}

bool SlidingAndObedienceRule::beforeMove(ChaosMoveContext& context)
{
    if (_options == nullptr || !_options->enableGeometricSlidingAndObedience)
        return true;

    LoiBernoulli obeyDistribution(obedienceProbabilityFor(context.attempt.piece));
    if (obeyDistribution(context.rng) == 0)
    {
        context.history.push_back("Chaos: la piece refuse d'obeir.");

        if (context.skipTurnRequested != nullptr)
        {
            *context.skipTurnRequested = true;
        }

        return false;
    }

    if (!isSlidingPiece(context.attempt.piece))
        return true;

    const int dx       = context.attempt.toX - context.attempt.fromX;
    const int dy       = context.attempt.toY - context.attempt.fromY;
    const int distance = std::max(std::abs(dx), std::abs(dy));

    if (distance <= 1)
        return true;

    int stepX = 0;
    if (dx > 0)
        stepX = 1;
    else if (dx < 0)
        stepX = -1;

    int stepY = 0;
    if (dy > 0)
        stepY = 1;
    else if (dy < 0)
        stepY = -1;

    GeometricDistribution stopDistanceDistribution(_options->slidingEarlyStopProbability);
    const int             sampled = stopDistanceDistribution(context.rng);
    const int             travel  = std::min(distance, sampled);

    if (travel < distance)
    {
        context.attempt.toX = context.attempt.fromX + stepX * travel;
        context.attempt.toY = context.attempt.fromY + stepY * travel;
        context.history.push_back("Chaos: la piece glissante s'arrete avant la destination.");
    }

    return true;
}

double SlidingAndObedienceRule::obedienceProbabilityFor(const Piece* piece) const
{
    if (piece == nullptr)
        return 1.0;

    switch (piece->getType())
    {
    case PieceType::Pawn:
    case PieceType::King:
        return 0.95;
    case PieceType::Knight:
        return 1.0;
    case PieceType::Queen:
        return 0.65;
    case PieceType::Bishop:
    case PieceType::Rook:
        return 0.80;
    }

    return 1.0;
}

bool SlidingAndObedienceRule::isSlidingPiece(const Piece* piece) const
{
    if (piece == nullptr)
        return false;

    return piece->getType() == PieceType::Bishop || piece->getType() == PieceType::Rook || piece->getType() == PieceType::Queen;
}

