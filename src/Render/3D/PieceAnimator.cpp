#include "PieceAnimator.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <vector>

#include "Board/Board.hpp"

namespace {

constexpr float EPSILON = 1e-5f;
constexpr float PI = 3.14159265358979323846f;
constexpr float MIN_ANIMATION_DURATION = 0.01f;
constexpr float DEFAULT_KNIGHT_ARC_HEIGHT = 0.5f;
constexpr float CAPTURE_EXPLOSION_DURATION = 0.45f;

float easeOutCubic(float t)
{
    const float oneMinusT = 1.f - t;
    return 1.f - oneMinusT * oneMinusT * oneMinusT;
}

bool sameGridPosition(const glm::vec2& lhs, const glm::vec2& rhs)
{
    return std::abs(lhs.x - rhs.x) <= EPSILON && std::abs(lhs.y - rhs.y) <= EPSILON;
}

} // namespace

namespace Render3D {

void PieceAnimator::update(const Board& board, const settings& gameSettings, float deltaTimeSeconds)
{
    struct PieceSnapshot
    {
        const Piece* piece = nullptr;
        glm::vec2    grid{0.f, 0.f};
    };

    const std::size_t boardCellCount = static_cast<std::size_t>(Board::SIZE) * static_cast<std::size_t>(Board::SIZE);

    std::vector<PieceSnapshot> pieces;
    pieces.reserve(boardCellCount);

    std::unordered_set<const Piece*> activePieces;
    activePieces.reserve(boardCellCount);

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& tile = board.getCase(x, y);
            if (!tile.getHasPiece())
                continue;

            const Piece* piece = tile.getPiece();
            const glm::vec2 gridPosition{static_cast<float>(x), static_cast<float>(y)};

            pieces.push_back(PieceSnapshot{piece, gridPosition});
            activePieces.insert(piece);
        }
    }

    if (activePieces.size() > _lastActivePieceCount)
    {
        _pieceAnimations.clear();
        _captureAnimations.clear();
    }

    const float frameDelta = std::max(0.f, deltaTimeSeconds);
    const float animationDuration = std::max(gameSettings.pieceMoveDuration, MIN_ANIMATION_DURATION);
    const float captureDuration = std::max(CAPTURE_EXPLOSION_DURATION, MIN_ANIMATION_DURATION);

    for (const PieceSnapshot& snapshot : pieces)
    {
        auto [it, inserted] = _pieceAnimations.try_emplace(snapshot.piece);
        PieceAnimationState& state = it->second;

        if (inserted)
        {
            state.start    = snapshot.grid;
            state.target   = snapshot.grid;
            state.current  = snapshot.grid;
            state.duration = animationDuration;
            continue;
        }

        if (!sameGridPosition(state.target, snapshot.grid))
        {
            if (gameSettings.animatePieces)
            {
                state.start    = state.current;
                state.target   = snapshot.grid;
                state.elapsed  = 0.f;
                state.duration = animationDuration;
                state.moving   = true;
                state.yOffset  = 0.f;
            }
            else
            {
                state.start   = snapshot.grid;
                state.target  = snapshot.grid;
                state.current = snapshot.grid;
                state.elapsed = 0.f;
                state.moving  = false;
                state.yOffset = 0.f;
            }
        }
    }

    for (auto it = _pieceAnimations.begin(); it != _pieceAnimations.end();)
    {
        if (!activePieces.contains(it->first))
        {
            CaptureAnimationState& capture = _captureAnimations[it->first];
            capture.grid     = it->second.current;
            capture.elapsed  = 0.f;
            capture.duration = captureDuration;
            it = _pieceAnimations.erase(it);
        }
        else
            ++it;
    }

    for (auto it = _captureAnimations.begin(); it != _captureAnimations.end();)
    {
        if (activePieces.contains(it->first))
            it = _captureAnimations.erase(it);
        else
            ++it;
    }

    _animatedPositions.clear();
    _animatedPositions.reserve(pieces.size());

    for (const PieceSnapshot& snapshot : pieces)
    {
        auto animationIt = _pieceAnimations.find(snapshot.piece);
        if (animationIt == _pieceAnimations.end())
            continue;

        PieceAnimationState& state = animationIt->second;

        if (!gameSettings.animatePieces)
        {
            state.current = state.target;
            state.elapsed = 0.f;
            state.moving  = false;
            state.yOffset = 0.f;
        }
        else if (state.moving)
        {
            state.elapsed += frameDelta;

            const float t = std::clamp(state.elapsed / std::max(state.duration, MIN_ANIMATION_DURATION), 0.f, 1.f);
            const float easedT = easeOutCubic(t);

            state.current = state.start + (state.target - state.start) * easedT;
            state.yOffset = (snapshot.piece->getType() == PieceType::Knight) ? std::sin(PI * t) * DEFAULT_KNIGHT_ARC_HEIGHT : 0.f;

            if (t >= 1.f - EPSILON)
            {
                state.current = state.target;
                state.moving  = false;
                state.yOffset = 0.f;
            }
        }
        else
        {
            state.current = state.target;
            state.yOffset = 0.f;
        }

        _animatedPositions.emplace(snapshot.piece, glm::vec3{state.current.x, state.current.y, state.yOffset});
    }

    _explodingPieces.clear();
    _explodingPieces.reserve(_captureAnimations.size());

    for (auto it = _captureAnimations.begin(); it != _captureAnimations.end();)
    {
        CaptureAnimationState& state = it->second;
        state.elapsed += frameDelta;

        const float t      = std::clamp(state.elapsed / std::max(state.duration, MIN_ANIMATION_DURATION), 0.f, 1.f);
        const float easedT = easeOutCubic(t);

        _explodingPieces.emplace(it->first, ExplodingPieceState{glm::vec3{state.grid.x, state.grid.y, 0.f}, easedT});

        if (t >= 1.f - EPSILON)
            it = _captureAnimations.erase(it);
        else
            ++it;
    }

    _lastActivePieceCount = activePieces.size();
}

const PieceAnimator::AnimatedPiecePositions& PieceAnimator::getPositions() const
{
    return _animatedPositions;
}

const ExplodingPiecePositions& PieceAnimator::getExplosions() const
{
    return _explodingPieces;
}

void PieceAnimator::reset()
{
    _pieceAnimations.clear();
    _captureAnimations.clear();
    _animatedPositions.clear();
    _explodingPieces.clear();
    _lastActivePieceCount = 0;
}

} // namespace Render3D

