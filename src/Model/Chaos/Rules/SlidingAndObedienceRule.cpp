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

/**
 * Chaos rule: Makes pieces potentially disobey orders and slide to random distances
 *
 * Two chaos mechanics applied BEFORE a move is executed:
 * 1. OBEDIENCE: Piece might refuse to move (Bernoulli roll by piece type)
 * 2. SLIDING: Sliding pieces (Bishop, Rook, Queen) might stop early (Geometric distribution)
 *
 * @param context : the move attempt context
 * @return : true if move allowed, false if blocked (disobedience)
 */
bool SlidingAndObedienceRule::beforeMove(ChaosMoveContext& context)
{
    // Only apply if rule is enabled
    if (_options == nullptr || !_options->enableGeometricSlidingAndObedience)
        return true;

    BernoulliDistribution obeyDistribution(obedienceProbabilityFor(context.attempt.piece));
    if (obeyDistribution(context.rng) == 0)
    // --- MECHANIC 1: OBEDIENCE CHECK ---
    // Roll Bernoulli: "Will this piece obey the player's command?"
    LoiBernoulli obeyDistribution(obedienceProbabilityFor(context.attempt.piece)); // Each piece type has different obedience probability (configured in options)
    if (obeyDistribution(context.rng) == 0)                                        // Roll failed (piece refuses)
    {
        // OMG, Piece disobeys! Cancel the move and skip turn :(
        context.history.push_back("Chaos: la piece refuse d'obeir. Elle semble vexée.");
        if (context.skipTurnRequested != nullptr)
        {
            *context.skipTurnRequested = true;
        }
        return false;
    }

    // --- MECHANIC 2: SLIDING CHECK ---
    // Only sliding pieces (Bishop, Rook, Queen) can be interrupted mid-move
    if (!isSlidingPiece(context.attempt.piece))
        return true; // Non-sliding pieces move normally

    // Calculate movement direction and distance
    const int dx       = context.attempt.toX - context.attempt.fromX;
    const int dy       = context.attempt.toY - context.attempt.fromY;
    const int distance = std::max(std::abs(dx), std::abs(dy)); // Chebyshev distance

    // If moving only 1 square (adjacent), no sliding possible
    if (distance <= 1)
        return true;

    // Calculate unit direction (step by step)
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

    // Roll Geometric distribution: "How far does the piece slide before stopping?"
    // Geometric produces: 1, 2, 3, 4, 5... with decreasing probability
    // Higher probability = stops earlier
    GeometricDistribution stopDistanceDistribution(_options->slidingEarlyStopProbability);
    const int             sampled = stopDistanceDistribution(context.rng);
    // Cap the sliding distance to not exceed intended destination
    const int travel = std::min(distance, sampled);

    // If the piece stops before reaching destination, modify the move target
    if (travel < distance)
    {
        // Recalculate destination based on actual sliding distance
        context.attempt.toX = context.attempt.fromX + stepX * travel;
        context.attempt.toY = context.attempt.fromY + stepY * travel;
        context.history.push_back("Chaos: la piece semble fatiguée... Elle s'est arrêtée avant la destination.");
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
        return PAWN_OBEDIENCE_PROBA;
    case PieceType::Knight:
        return KNIGHT_OBEDIENCE_PROBA;
    case PieceType::King:
        return KING_OBEDIENCE_PROBA;
    case PieceType::Queen:
        return QUEEN_OBEDIENCE_PROBA;
    case PieceType::Bishop:
        return BISHOP_OBEDIENCE_PROBA;
    case PieceType::Rook:
        return ROOK_OBEDIENCE_PROBA;
    }

    return 1.0;
}

bool SlidingAndObedienceRule::isSlidingPiece(const Piece* piece) const
{
    if (piece == nullptr)
        return false;

    return piece->getType() == PieceType::Bishop || piece->getType() == PieceType::Rook || piece->getType() == PieceType::Queen;
}
