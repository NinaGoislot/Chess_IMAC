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

class PieceAnimator
{
public:
    using AnimatedPiecePositions = std::unordered_map<const Piece*, glm::vec3>;

    void update(const Board& board, const settings& gameSettings, float deltaTimeSeconds);
    const AnimatedPiecePositions& positions() const;
    const ExplodingPiecePositions& explosions() const;
    void reset();

private:
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

    struct CaptureAnimationState
    {
        glm::vec2 grid{0.f, 0.f};
        float     elapsed  = 0.f;
        float     duration = 0.f;
    };

    std::unordered_map<const Piece*, PieceAnimationState> _pieceAnimations;
    std::unordered_map<const Piece*, CaptureAnimationState> _captureAnimations;
    AnimatedPiecePositions                                  _animatedPositions;
    ExplodingPiecePositions                                 _explodingPieces;
    std::size_t                                             _lastActivePieceCount = 0;
};

} // namespace Render3D
