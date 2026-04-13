#include "Game/Chaos/Rules/KirbyPoissonRule.hpp"
#include <optional>
#include <utility>
#include <vector>
#include "Game/Chaos/ChaosMode.hpp"
#include "probabimac/PoissonDistribution.hpp"
#include "probabimac/UniformDistribution.hpp"
#include "utilities/Vector2D.hpp"


namespace {
int uniformIndex(std::mt19937& rng, int inclusiveMin, int inclusiveMax)
{
    UniformDistribution dist(static_cast<double>(inclusiveMin), static_cast<double>(inclusiveMax + 1));
    return static_cast<int>(dist(rng));
}

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

void KirbyPoissonRule::onGameSetup(ChaosRuleContext& context)
{
    _spawnCount            = 0;
    _remainingTurnsVisible = 0;
    if (context.kirbyPosition != nullptr)
        *context.kirbyPosition = std::nullopt;
}

void KirbyPoissonRule::onTurnStart(ChaosRuleContext& context)
{
    if (_options == nullptr || !_options->enableKirbyPoissonUniform)
        return;

    if (_remainingTurnsVisible > 0)
    {
        --_remainingTurnsVisible;
        if (_remainingTurnsVisible == 0)
        {
            if (context.kirbyPosition != nullptr)
                *context.kirbyPosition = std::nullopt;
        }
    }

    if (_spawnCount >= 3)
        return;

    PoissonDistribution spawnCount(_options->kirbySpawnLambda);
    if (spawnCount(context.rng) == 0)
        return;

    bool spawned = false;
    for (int attempt = 0; attempt < 8 && !spawned; ++attempt)
    {
        const int x = uniformIndex(context.rng, 0, Board::SIZE - 1);
        const int y = uniformIndex(context.rng, 2, 5);

        if (hasPawnAt(context.board, x, y))
            continue;

        std::vector<std::pair<int, int>> targets;
        if (context.board.getCase(x, y).getHasPiece())
        {
            targets.emplace_back(x, y);
        }

        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx == 0 && dy == 0)
                    continue;

                const int nx = x + dx;
                const int ny = y + dy;
                if (!context.board.isInside(Vector2D(static_cast<float>(nx), static_cast<float>(ny))))
                    continue;

                if (context.board.getCase(nx, ny).getHasPiece())
                    targets.emplace_back(nx, ny);
            }
        }

        if (targets.empty())
            continue;

        const int  victimIndex = uniformIndex(context.rng, 0, static_cast<int>(targets.size()) - 1);
        const auto victim      = targets[static_cast<std::size_t>(victimIndex)];

        if (context.kirbyPosition != nullptr)
            *context.kirbyPosition = std::pair<int, int>{x, y};
        removePieceAt(context.board, context.players, victim.first, victim.second);

        _remainingTurnsVisible = 1;
        ++_spawnCount;
        spawned = true;
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

