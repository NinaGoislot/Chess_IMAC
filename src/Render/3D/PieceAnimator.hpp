#pragma once

#include <cstddef>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Game/Pieces/Piece.hpp"
#include "Game/settings.hpp"
#include "Render/3D/PieceEffects.hpp"

class Board;

namespace Render3D {

// Computes smooth piece movement/capture animation states for 3D rendering.
class PieceAnimator
{
public:
    using AnimatedPiecePositions = std::unordered_map<const Piece*, glm::vec3>;

    // Updates animation states from board snapshots and frame delta.
    void update(const Board& board, const settings& gameSettings, float deltaTimeSeconds);
    const AnimatedPiecePositions& getPositions() const;
    const ExplodingPiecePositions& getExplosions() const;
    // Resets all cached animation states.
    void reset();

private:
    // Per-piece interpolation state for movement animation.
    struct PieceAnimationState
    {
        glm::vec2 start{0.f, 0.f};
        glm::vec2 target{0.f, 0.f};
        glm::vec2 current{0.f, 0.f};
        float     elapsed  = 0.f;
        float     duration = 0.f;
        float     yOffset  = 0.f;
        bool      moving   = false;
    };

    // Per-piece timing state for capture/explosion animation.
    struct CaptureAnimationState
    {
        glm::vec2 grid{0.f, 0.f};
        float     elapsed  = 0.f;
        float     duration = 0.f;
    };

    // Internal animation caches and outputs.
    std::unordered_map<const Piece*, PieceAnimationState> _pieceAnimations;
    std::unordered_map<const Piece*, CaptureAnimationState> _captureAnimations;
    AnimatedPiecePositions                                  _animatedPositions;
    ExplodingPiecePositions                                 _explodingPieces;
    std::size_t                                             _lastActivePieceCount = 0;
};

} // namespace Render3D
