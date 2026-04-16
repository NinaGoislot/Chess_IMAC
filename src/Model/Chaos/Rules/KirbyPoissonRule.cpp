#include "Model/Chaos/Rules/KirbyPoissonRule.hpp"
#include <optional>
#include <utility>
#include <vector>
#include "Model/Chaos/ChaosMode.hpp"
#include "probabimac/PoissonDistribution.hpp"
#include "probabimac/UniformDistribution.hpp"
#include "utilities/Vector2D.hpp"

namespace {

/**
 * Generates a random index within the specified range using a uniform distribution
 * @param rng : the random number generator to use
 * @param inclusiveMin : the minimum index (inclusive)
 * @param inclusiveMax : the maximum index (inclusive)
 */
int uniformIndex(std::mt19937& rng, int inclusiveMin, int inclusiveMax)
{
    UniformDistribution dist(static_cast<double>(inclusiveMin), static_cast<double>(inclusiveMax + 1));
    return static_cast<int>(dist(rng));
}

/**
 * Removes a piece from the specified position on the board and updates the player's piece list
 * @param board : the game board
 * @param players : the array of players
 * @param x : the x coordinate of the piece to remove
 * @param y : the y coordinate of the piece to remove
 */
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

/**
 * Checks if a pawn is located at the specified position on the board
 * @param board : the game board
 * @param x : the x coordinate to check
 * @param y : the y coordinate to check
 * @return true if a pawn is at the specified position, false otherwise

 */
bool hasPawnAt(const Board& board, int x, int y)
{
    const Case& square = board.getCase(x, y);
    if (!square.getHasPiece())
        return false;

    const Piece* piece = square.getPiece();
    return piece != nullptr && piece->getType() == PieceType::Pawn;
}
} // namespace

KirbyPoissonRule::KirbyPoissonRule(const ChaosOptions* options)
    : _options(options)
{
}

/**
 * Sets up the game state for the Kirby Poisson rule
 * @param context : the chaos rule context
 */
void KirbyPoissonRule::onGameSetup(ChaosRuleContext& context)
{
    _remainingTurnsVisible = 0;
    if (context.kirbyPosition != nullptr)
        *context.kirbyPosition = std::nullopt;
}

/**
 * Handles Kirby spawning and despawning at the start of each turn.
 *
 * Kirby can spawn multiple times per game, with frequency determined by Poisson distribution.
 * He appears at the center of the board and eats a piece when he spawns.
 * He remains visible for 1 turn only, then disappears until the next spawn event.
 *
 * Flow:
 * 1. Handle visibility timer: if Kirby was visible last turn, hide him now
 * 2. Use Poisson distribution to determine if Kirby spawns this turn (no spawn limit)
 * 3. If yes: pick a spawn location (center area, max 8 attempts), find adjacent pieces as targets, eat one
 *
 * @param context : the chaos rule context (board, rng, players, history)
 */
void KirbyPoissonRule::onTurnStart(ChaosRuleContext& context)
{
    // Step 0: Activate only if rule is enabled
    if (_options == nullptr || !_options->enableKirbyPoissonUniform)
        return;

    // Step 1: Manage Kirby visibility timeout (he shows for 1 turn, then disappears)
    if (_remainingTurnsVisible > 0)
    {
        --_remainingTurnsVisible;
        if (_remainingTurnsVisible == 0)
        {
            // Hide Kirby: set position to null so he's no longer rendered
            if (context.kirbyPosition != nullptr)
                *context.kirbyPosition = std::nullopt;
        }
    }

    // Step 2: Use Poisson distribution to roll for spawn chance (no limit, let chaos happen)
    // If the random value is 0, Kirby doesn't spawn this turn
    PoissonDistribution spawnCount(_options->kirbySpawnLambda);
    if (spawnCount(context.rng) == 0)
        return;

    // Step 4: Try to spawn Kirby (max 8 attempts to find a valid location)
    bool spawned = false;
    for (int attempt = 0; attempt < 8 && !spawned; ++attempt)
    {
        // Pick a random position in the center area (rows 2-5)
        const int x = uniformIndex(context.rng, 0, Board::SIZE - 1);
        const int y = uniformIndex(context.rng, 2, 5);

        // Reject if a pawn occupies this spot (pawns are too important to kill)
        if (hasPawnAt(context.board, x, y))
            continue;

        // Collect all pieces at the spawn location and in adjacent squares (8 neighbors)
        std::vector<std::pair<int, int>> targets;

        // Check the spawn location itself
        if (context.board.getCase(x, y).getHasPiece())
        {
            targets.emplace_back(x, y);
        }

        // Check all 8 adjacent squares (except self, that's why we skip dx==0 && dy==0)
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0)
                    continue; // Skip the spawn square itself (already checked above)

                const int nx = x + dx;
                const int ny = y + dy;

                // Skip if the neighbor is off the board
                if (!context.board.isInside(Vector2D(static_cast<float>(nx), static_cast<float>(ny))))
                    continue;

                // Add to targets if it has a piece
                if (context.board.getCase(nx, ny).getHasPiece())
                    targets.emplace_back(nx, ny);
            }
        }

        // Reject this spawn location if there are no adjacent pieces to eat
        if (targets.empty())
            continue;

        // Pick a random victim from all available targets
        const int  victimIndex = uniformIndex(context.rng, 0, static_cast<int>(targets.size()) - 1);
        const auto victim      = targets[static_cast<std::size_t>(victimIndex)];

        // Finalize: place Kirby and remove the victim piece
        if (context.kirbyPosition != nullptr)
            *context.kirbyPosition = std::pair<int, int>{x, y};
        removePieceAt(context.board, context.players, victim.first, victim.second);

        // Update counters and history
        _remainingTurnsVisible = 1;    // Kirby stays visible for 1 turn
        spawned                = true; // Mark success and exit loop
        context.history.push_back("Chaos: Kirby apparait au centre et mange une piece.");
    }
}

bool KirbyTileMatches(const std::optional<std::pair<int, int>>* kirbyPosition, int x, int y)
{
    return kirbyPosition != nullptr
           && kirbyPosition->has_value()
           && kirbyPosition->value().first == x
           && kirbyPosition->value().second == y;
}

bool KirbyPoissonRule::beforeMove(ChaosMoveContext& context)
{
    if (!_options || !_options->enableKirbyPoissonUniform)
        return true;

    if (KirbyTileMatches(context.kirbyPosition, context.attempt.toX, context.attempt.toY))
    {
        context.history.push_back("Chaos: Kirby bloque cette case.");
        return false;
    }

    return true;
}
