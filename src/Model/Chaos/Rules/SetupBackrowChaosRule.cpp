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
 * Shuffles (randomizes) a vector using the Fisher-Yates algorithm.
 * This is the STANDARD algorithm for shuffling with these guarantees:
 * - Each possible ordering has EQUAL probability (uniform randomness, no bias)
 * - Efficient: O(n) time, single pass through vector
 * - Mathematically proven to work correctly
 * 
 * Algorithm: Start from END, swap each position with random earlier position
 * Example: Shuffle [A, B, C, D]
 *   i=3: Swap D with random(0-3) -> maybe [A, D, C, B]
 *   i=2: Swap C with random(0-2) -> maybe [A, D, B, C]
 *   i=1: Swap B with random(0-1) -> maybe [D, A, B, C] ✓ Done
 *
 * @param values : the vector to shuffle (modified in-place)
 * @param rng : the random number generator
 */
template<typename T>
void fisherYatesShuffle(std::vector<T>& values, std::mt19937& rng)
{
    if (values.empty())
        return;

    // Iterate from the END of vector to the beginning
    for (int i = static_cast<int>(values.size()) - 1; i > 0; --i)
    {
        // Pick random index between 0 and current position i (inclusive)
        const int j = uniformIndex(rng, 0, i);
        // Swap: move the random element to position i
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
 * Called ONCE at game start to scramble the opening position (Chaos Mode).
 * Transforms a predictable chess opening into random chaos:
 *   Normal: y=0: [R N B Q K B N R]    y=1: [P P P P P P P P]
 *   Chaos:  y=0: [K B P Q N B P R]    y=1: [P R P N P P B P] <-- scrambled!
 * 
 * For each player:
 * 1. Extract all starting pieces from their rows
 * 2. Randomly replace some backrow pieces with pawns (Bernoulli probability)
 * 3. Shuffle both rows randomly (Fisher-Yates)
 * 4. Place new scrambled rows back on board
 * Result: Neither player knows where their pieces are! Chaos guaranteed.
 *
 * @param context : the chaos rule initialization context
 */
void SetupBackrowChaosRule::onGameSetup(ChaosRuleContext& context)
{
    if (_options == nullptr || !_options->enableBernoulliBackrowAndShuffle)
        return;

    // Bernoulli: "Will a pawn spawn in backrow?" Probability configured in options
    LoiBernoulli pawnInBackrow(_options->pawnBackrowProbability);

    // Process both White and Black players
    for (int playerIndex = 0; playerIndex < 2; ++playerIndex)
    {
        // Determine which rows belong to this player
        const int backY  = (playerIndex == 0) ? 0 : 7;  // White: row 0, Black: row 7 (piece row)
        const int frontY = (playerIndex == 0) ? 1 : 6;  // White: row 1, Black: row 6 (pawn row)

        // --- STEP 1: Extract all pieces from their normal starting positions ---
        std::vector<Piece*> backPieces;   // Rooks, Knights, Bishops, Queen, King
        std::vector<Piece*> pawnPieces;   // Pawns
        backPieces.reserve(Board::SIZE);
        pawnPieces.reserve(Board::SIZE);

        for (int x = 0; x < Board::SIZE; ++x)
        {
            // Remove piece from backrow (position x,backY)
            Piece* back = context.board.getCase(x, backY).takePiece();
            // Remove piece from front row (position x,frontY)
            Piece* pawn = context.board.getCase(x, frontY).takePiece();
            
            if (back != nullptr)
                backPieces.push_back(back);
            if (pawn != nullptr)
                pawnPieces.push_back(pawn);
        }

        // --- STEP 2: Build new scrambled backrow by mixing pieces with pawns ---
        std::vector<Piece*> newBack;
        newBack.reserve(Board::SIZE);

        for (int i = 0; i < Board::SIZE; ++i)
        {
            // Randomly decide: should a pawn appear here instead of a normal piece?
            const bool usePawn = pawnInBackrow(context.rng) == 1 && !pawnPieces.empty();
            
            if (usePawn)
            {
                // Yes: Place a pawn in backrow position (unusual, chaotic!)
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
        // Mixed pieces are now in some order, randomize their positions
        fisherYatesShuffle(newBack, context.rng);

        // --- STEP 4: Build scrambled front row from leftover pieces ---
        std::vector<Piece*> frontPieces;
        frontPieces.reserve(Board::SIZE);
        // Collect any unused backrow pieces and pawns
        frontPieces.insert(frontPieces.end(), backPieces.begin(), backPieces.end());
        frontPieces.insert(frontPieces.end(), pawnPieces.begin(), pawnPieces.end());
        // Shuffle this row too (Fisher-Yates)
        fisherYatesShuffle(frontPieces, context.rng);

        // --- STEP 5: Place the scrambled pieces back on the board ---
        for (int x = 0; x < Board::SIZE; ++x)
        {
            context.board.getCase(x, backY).setPiece(newBack[x]);
            context.board.getCase(x, frontY).setPiece(frontPieces[x]);
        }
    }

    context.history.push_back("Chaos: setup Bernoulli + uniforme discrete applique.");
}
