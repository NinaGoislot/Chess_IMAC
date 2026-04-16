#include "Model/Chaos/Rules/SetupBackrowChaosRule.hpp"
#include <utility>
#include <vector>
#include "Model/Chaos/ChaosMode.hpp"
#include "probabimac/BernoulliDistribution.hpp"
#include "probabimac/UniformDistribution.hpp"

// Shuffles pieces in the backrow and randomly puts some pawns in the backrow as well, based on a Bernoulli law
namespace {
/**
 * Generates a random index within the specified range using a uniform distribution
 *
 * @param rng : the random number generator
 * @param inclusiveMin : the minimum value (inclusive)
 * @param inclusiveMax : the maximum value (inclusive)
 * @return : the generated index
 */
int uniformIndex(std::mt19937& rng, int inclusiveMin, int inclusiveMax)
{
    UniformDistribution dist(static_cast<double>(inclusiveMin), static_cast<double>(inclusiveMax + 1));
    return static_cast<int>(dist(rng));
}

/**
 * Shuffles (randomizes) a vector using the Fisher-Yates algorithm (EQUAL probability, O(n) time)
 *
 * Algorithm: Start from END, swap each position with random earlier position
 *
 * @param values : the vector to shuffle
 * @param rng : the random number generator
 */
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

// Constructor
SetupBackrowChaosRule::SetupBackrowChaosRule(const ChaosOptions* options)
    : _options(options)
{
}

/**
 * Called ONCE at game start
 *
 * For each player:
 * 1. Extract all starting pieces from their rows
 * 2. Randomly replace some backrow pieces with pawns (Bernoulli probability)
 * 3. Shuffle both rows randomly (Fisher-Yates)
 * 4. Place new setPieces on board
 *
 * @param context : the chaos rule initialization context
 */
void SetupBackrowChaosRule::onGameSetup(ChaosRuleContext& context)
{
    if (_options == nullptr || !_options->enableBernoulliBackrowAndShuffle)
        return;

    // Bernoulli: "Will a pawn spawn in backrow?" Probability configured in options
    LoiBernoulli pawnInBackrow(_options->pawnBackrowProbability);

    // Process
    for (int playerIndex = 0; playerIndex < 2; ++playerIndex)
    {
        // Determine which rows belong to this player
        const int backY  = (playerIndex == 0) ? 0 : 7; // White: row 0, Black: row 7 (piece row)
        const int frontY = (playerIndex == 0) ? 1 : 6; // White: row 1, Black: row 6 (pawn row)

        // --- STEP 1: Extract all pieces from their normal positions ---
        std::vector<Piece*> backPieces;
        std::vector<Piece*> pawnPieces;
        backPieces.reserve(Board::SIZE);
        pawnPieces.reserve(Board::SIZE);

        // Remove pieces
        for (int x = 0; x < Board::SIZE; ++x)
        {
            Piece* back = context.board.getCase(x, backY).takePiece();
            Piece* pawn = context.board.getCase(x, frontY).takePiece();

            if (back != nullptr)
                backPieces.push_back(back);
            if (pawn != nullptr)
                pawnPieces.push_back(pawn);
        }

        // --- STEP 2: Createe new bakckrow ---
        std::vector<Piece*> newBack;
        newBack.reserve(Board::SIZE); // optimisation de performance pour éviter les reallocations pendant les push back

        for (int i = 0; i < Board::SIZE; ++i)
        {
            // pawn or normal
            const bool usePawn = pawnInBackrow(context.rng) == 1 && !pawnPieces.empty();

            if (usePawn)
            {
                // Yes: Place a pawn in backrow position (unusual, chaotic! AHAH)
                newBack.push_back(pawnPieces.back());
                pawnPieces.pop_back();
            }
            else if (!backPieces.empty())
            {
                // No: Place a normal backrow piece here
                newBack.push_back(backPieces.back());
                backPieces.pop_back();
            }
            else if (!pawnPieces.empty())
            {
                // Fallback: ran out of backpieces, use remaining pawns
                newBack.push_back(pawnPieces.back());
                pawnPieces.pop_back();
            }
        }

        // --- STEP 3: Shuffle the new backrow (Fisher-Yates) ---
        fisherYatesShuffle(newBack, context.rng);

        // --- STEP 4: Create new front row ---
        std::vector<Piece*> frontPieces;
        frontPieces.reserve(Board::SIZE); // optimization perf encore

        // Collect any unused backrow pieces and pawns, then shuffle
        frontPieces.insert(frontPieces.end(), backPieces.begin(), backPieces.end());
        frontPieces.insert(frontPieces.end(), pawnPieces.begin(), pawnPieces.end());
        fisherYatesShuffle(frontPieces, context.rng);

        // --- STEP 5: Display on board ---
        for (int x = 0; x < Board::SIZE; ++x)
        {
            context.board.getCase(x, backY).setPiece(newBack[x]);
            context.board.getCase(x, frontY).setPiece(frontPieces[x]);
        }
    }

    context.history.push_back("Chaos: setup Bernoulli + uniforme discrete applique. C'est le bordel dans les rangs arrieres !");
}
