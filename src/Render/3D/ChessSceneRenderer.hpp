#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <optional>
#include <utility>

#include "Board/Board.hpp"
#include "Game/State/SelectionState.hpp"
#include "Game/settings.hpp"
#include "Render/3D/PieceAnimator.hpp"
#include "Render/3D/PieceEffects.hpp"

namespace Render3D {

class GLRenderer;
class ResourceManager;

// High-level board/piece renderer. Stateless; draw calls require a renderer.
class ChessSceneRenderer {
public:
    // Draw functions
    void drawBoard(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager,
                   PieceColor currentTurn, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection) const;
    void drawPieces(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager,
                    PieceColor currentTurn, const SelectionState& selection, const PieceAnimator::AnimatedPiecePositions& animatedPiecePositions,
                    const ExplodingPiecePositions& explodingPiecePositions) const;

private:
    void drawBoardGaps(GLRenderer& glRenderer, const glm::mat4& viewProjection, const settings& gameSettings) const;
    void drawTiles(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Board& board, const settings& gameSettings,
                   std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection) const;
    void drawBoardEdges(GLRenderer& glRenderer, const glm::mat4& viewProjection, const settings& gameSettings, const ResourceManager& resourceManager) const;

    void drawSinglePiece(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Piece* piece, float boardX, float boardY, float yOffset,
                         float originX, float originZ, float topY, const ResourceManager& resourceManager, bool isHovered, const glm::vec3& hoverColor) const;
    void drawSingleExplodingPiece(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Piece* piece, float boardX, float boardY, float yOffset,
                                  float originX, float originZ, float topY, float explosionProgress,
                                  const ResourceManager& resourceManager) const;
};

} // namespace Render3D
