#include "Game/Chaos/Rules/SetupBackrowChaosRule.hpp"
#include <utility>
#include <vector>
#include "Game/Chaos/ChaosMode.hpp"
#include "probabimac/BernoulliDistribution.hpp"
#include "probabimac/UniformDistribution.hpp"


namespace {
int uniformIndex(std::mt19937& rng, int inclusiveMin, int inclusiveMax)
{
    UniformDistribution dist(static_cast<double>(inclusiveMin), static_cast<double>(inclusiveMax + 1));
    return static_cast<int>(dist(rng));
}

template<typename T>
void fisherYatesShuffle(std::vector<T>& values, std::mt19937& rng)
{
    if (values.empty())
        return;

    for (int i = static_cast<int>(values.size()) - 1; i > 0; --i)
    {
        const int j = uniformIndex(rng, 0, i);
        std::swap(values[i], values[j]);
    }
}
} // namespace

SetupBackrowChaosRule::SetupBackrowChaosRule(const ChaosOptions* options)
    : _options(options)
{
}

void SetupBackrowChaosRule::onGameSetup(ChaosRuleContext& context)
{
    if (_options == nullptr || !_options->enableBernoulliBackrowAndShuffle)
        return;

    LoiBernoulli pawnInBackrow(_options->pawnBackrowProbability);

    for (int playerIndex = 0; playerIndex < 2; ++playerIndex)
    {
        const int backY  = (playerIndex == 0) ? 0 : 7;
        const int frontY = (playerIndex == 0) ? 1 : 6;

        std::vector<Piece*> backPieces;
        std::vector<Piece*> pawnPieces;
        backPieces.reserve(Board::SIZE);
        pawnPieces.reserve(Board::SIZE);

        for (int x = 0; x < Board::SIZE; ++x)
        {
            Piece* back = context.board.getCase(x, backY).takePiece();
            Piece* pawn = context.board.getCase(x, frontY).takePiece();
            if (back != nullptr)
                backPieces.push_back(back);
            if (pawn != nullptr)
                pawnPieces.push_back(pawn);
        }

        std::vector<Piece*> newBack;
        newBack.reserve(Board::SIZE);

        for (int i = 0; i < Board::SIZE; ++i)
        {
            const bool usePawn = pawnInBackrow(context.rng) == 1 && !pawnPieces.empty();
            if (usePawn)
            {
                newBack.push_back(pawnPieces.back());
                pawnPieces.pop_back();
            }
            else if (!backPieces.empty())
            {
                newBack.push_back(backPieces.back());
                backPieces.pop_back();
            }
            else if (!pawnPieces.empty())
            {
                newBack.push_back(pawnPieces.back());
                pawnPieces.pop_back();
            }
        }

        fisherYatesShuffle(newBack, context.rng);

        std::vector<Piece*> frontPieces;
        frontPieces.reserve(Board::SIZE);
        frontPieces.insert(frontPieces.end(), backPieces.begin(), backPieces.end());
        frontPieces.insert(frontPieces.end(), pawnPieces.begin(), pawnPieces.end());
        fisherYatesShuffle(frontPieces, context.rng);

        for (int x = 0; x < Board::SIZE; ++x)
        {
            context.board.getCase(x, backY).setPiece(newBack[x]);
            context.board.getCase(x, frontY).setPiece(frontPieces[x]);
        }
    }

    context.history.push_back("Chaos: setup Bernoulli + uniforme discrete applique.");
}
