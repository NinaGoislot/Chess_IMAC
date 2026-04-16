#include "ChessSceneRenderer.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <glm/gtc/matrix_transform.hpp>
#include "Render/3D/GLRenderer.hpp"
#include "Render/3D/Material.hpp"
#include "Render/3D/ResourceManager.hpp"


namespace {

constexpr std::size_t PIECE_TYPE_COUNT = 6u;

constexpr float BOARD_TILE_SIZE  = 0.94f;
constexpr float BOARD_CENTER_Y   = -0.05f;
constexpr float PIECE_LIFT_Y     = 0.01f;
constexpr float PIECE_BASE_WIDTH = 0.45f;
constexpr float KIRBY_HEIGHT     = 0.7f;

constexpr std::array<float, PIECE_TYPE_COUNT> PIECES_HEIGHT = {
    0.6f,
    0.95f,
    0.8f,
    0.9f,
    1.1f,
    1.2f,
};

std::size_t pieceTypeIndex(PieceType type)
{
    return static_cast<std::size_t>(type);
}

glm::vec3 colorForPiece(const Piece* piece)
{
    if (piece == nullptr)
        return glm::vec3{0.f, 0.f, 0.f};

    if (piece->getColor() == PieceColor::White)
        return glm::vec3{0.9f, 0.9f, 0.9f};

    return glm::vec3{0.15f, 0.15f, 0.2f};
}

bool hasTile(const std::vector<Vector2D>& tiles, int x, int y)
{
    return std::any_of(
        tiles.begin(),
        tiles.end(),
        [&](const Vector2D& tile) {
            return static_cast<int>(tile.getX()) == x && static_cast<int>(tile.getY()) == y;
        }
    );
}

} // namespace

namespace Render3D {

void ChessSceneRenderer::drawBoard(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager, PieceColor currentTurn, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection) const
{
    (void)currentTurn;
    drawBoardGaps(glRenderer, viewProjection, gameSettings);
    drawTiles(glRenderer, viewProjection, board, gameSettings, kirbyPosition, selection);
    drawKirby(glRenderer, viewProjection, gameSettings, kirbyPosition, resourceManager);
    drawBoardEdges(glRenderer, viewProjection, gameSettings, resourceManager);
}

void ChessSceneRenderer::drawBoardGaps(GLRenderer& glRenderer, const glm::mat4& viewProjection, const settings& gameSettings) const
{
    const float boardHalfSize   = (static_cast<float>(Board::SIZE) - 1.f) * 0.5f + BOARD_TILE_SIZE * 0.5f;
    const float fullBoardSize   = boardHalfSize * 2.f;
    const float gapLayerHeight  = gameSettings.boardThickness * 0.4f;
    const float gapLayerTopY    = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f - 0.004f;
    const float gapLayerCenterY = gapLayerTopY - gapLayerHeight * 0.5f;

    Material gapMaterial;
    gapMaterial.color = gameSettings.getBoardGapColorVec3();

    const glm::mat4 gapLayerModel = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, gapLayerCenterY, 0.f})
                                    * glm::scale(glm::mat4{1.f}, glm::vec3{fullBoardSize, gapLayerHeight, fullBoardSize});

    glRenderer.drawCube(viewProjection, gapLayerModel, gapMaterial);
}

void ChessSceneRenderer::drawTiles(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection) const
{
    (void)kirbyPosition;
    (void)kirbyPosition;

    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;

    const glm::vec3 whiteTileColor = gameSettings.getWhiteVec3();
    const glm::vec3 blackTileColor = gameSettings.getBlackVec3();
    const glm::vec3 selectedOwnPieceColor{0.20f, 0.45f, 1.f};
    const glm::vec3 availableMoveColor{0.20f, 0.75f, 0.25f};
    const glm::vec3 captureMoveColor{1.f, 0.55f, 0.f};
    const ImVec4    hoverColorRaw = gameSettings.getHighlight();
    const glm::vec3 hoverSelectableColor{hoverColorRaw.x, hoverColorRaw.y, hoverColorRaw.z};

    const glm::mat4 tileScale = glm::scale(glm::mat4{1.f}, glm::vec3{BOARD_TILE_SIZE, gameSettings.boardThickness, BOARD_TILE_SIZE});

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& tileCase    = board.getCase(x, y);
            const bool  isWhiteTile = ((x + y) % 2) == 0;
            glm::vec3   tileColor   = isWhiteTile ? whiteTileColor : blackTileColor;

            const bool isHighlighted = hasTile(selection.highlighted, x, y);
            const bool isSelected    = selection.selected.has_value()
                                    && static_cast<int>(selection.selected->getX()) == x
                                    && static_cast<int>(selection.selected->getY()) == y;
            const bool isHovered = selection.hoveredSelectable.has_value()
                                   && static_cast<int>(selection.hoveredSelectable->getX()) == x
                                   && static_cast<int>(selection.hoveredSelectable->getY()) == y;

            if (isHighlighted || isSelected)
            {
                if (isSelected)
                {
                    tileColor = selectedOwnPieceColor;
                }
                else if (!tileCase.getHasPiece())
                {
                    tileColor = availableMoveColor;
                }
                else
                {
                    tileColor = captureMoveColor;
                }
            }
            else if (isHovered)
            {
                tileColor = hoverSelectableColor;
            }

            Material tileMaterial;
            tileMaterial.color = tileColor;

            const float worldX = boardOriginX + static_cast<float>(x);
            const float worldZ = boardOriginZ + static_cast<float>(y);

            const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, BOARD_CENTER_Y, worldZ}) * tileScale;

            glRenderer.drawCube(viewProjection, model, tileMaterial);
        }
    }
}

void ChessSceneRenderer::drawKirby(GLRenderer& glRenderer, const glm::mat4& viewProjection, const settings& gameSettings,
                                   std::optional<std::pair<int, int>> kirbyPosition, const ResourceManager& resourceManager) const
{
    if (!kirbyPosition.has_value())
        return;

    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardTopY    = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f;

    const float worldX = boardOriginX + static_cast<float>(kirbyPosition->first);
    const float worldZ = boardOriginZ + static_cast<float>(kirbyPosition->second);

    const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, boardTopY + PIECE_LIFT_Y, worldZ})
                            * glm::scale(glm::mat4{1.f}, glm::vec3{KIRBY_HEIGHT, KIRBY_HEIGHT, KIRBY_HEIGHT});

    const ResourceManager::PieceMeshGlData* kirbyMesh = resourceManager.getKirbyMesh();
    if (kirbyMesh != nullptr && kirbyMesh->isValid())
    {
        Material kirbyMaterial;
        kirbyMaterial.color = glm::vec3{1.f, 1.f, 1.f};
        kirbyMaterial.useMeshUv = true;

        if (!kirbyMesh->submeshes.empty())
        {
            for (const auto& submesh : kirbyMesh->submeshes)
            {
                Material submeshMaterial = kirbyMaterial;
                const unsigned int textureId = (submesh.textureId != 0) ? submesh.textureId : kirbyMesh->textureId;
                submeshMaterial.textureId = textureId;
                submeshMaterial.useTexture = (textureId != 0);
                submeshMaterial.useMeshUv = true;
                submeshMaterial.color = submesh.baseColorFactor;

                glRenderer.drawIndexedMesh(viewProjection,
                                           model,
                                           submeshMaterial,
                                           kirbyMesh->vao,
                                           static_cast<int>(submesh.indexCount),
                                           static_cast<std::size_t>(submesh.indexOffset));
            }
        }
        else
        {
            glRenderer.drawIndexedMesh(viewProjection, model, kirbyMaterial, kirbyMesh->vao, kirbyMesh->indexCount);
        }

        return;
    }

    Material fallbackMaterial;
    fallbackMaterial.color = glm::vec3{0.96f, 0.48f, 0.82f};
    glRenderer.drawCube(viewProjection, model, fallbackMaterial);
}

void ChessSceneRenderer::drawBoardEdges(GLRenderer& glRenderer, const glm::mat4& viewProjection, const settings& gameSettings, const ResourceManager& resourceManager) const
{
    const float boardHalfSize = (static_cast<float>(Board::SIZE) - 1.f) * 0.5f + BOARD_TILE_SIZE * 0.5f;

    const float sideThickness = gameSettings.boardSideThickness;
    const float sideHeight    = gameSettings.boardThickness + gameSettings.boardSideDrop;
    const float sideCenterY   = BOARD_CENTER_Y - gameSettings.boardSideDrop * 0.5f;
    const float sideLength    = (boardHalfSize + sideThickness) * 2.f;

    const glm::mat4 horizontalSideScale = glm::scale(glm::mat4{1.f}, glm::vec3{sideLength, sideHeight, sideThickness});
    const glm::mat4 verticalSideScale   = glm::scale(glm::mat4{1.f}, glm::vec3{sideThickness, sideHeight, sideLength});

    const glm::mat4 northSide = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, sideCenterY, boardHalfSize + sideThickness * 0.5f})
                                * horizontalSideScale;
    const glm::mat4 southSide = glm::translate(glm::mat4{1.f}, glm::vec3{0.f, sideCenterY, -(boardHalfSize + sideThickness * 0.5f)})
                                * horizontalSideScale;
    const glm::mat4 eastSide = glm::translate(glm::mat4{1.f}, glm::vec3{boardHalfSize + sideThickness * 0.5f, sideCenterY, 0.f})
                               * verticalSideScale;
    const glm::mat4 westSide = glm::translate(glm::mat4{1.f}, glm::vec3{-(boardHalfSize + sideThickness * 0.5f), sideCenterY, 0.f})
                               * verticalSideScale;

    const unsigned int boardEdgeTexture = resourceManager.getTexture2D(std::string(ResourceManager::BoardEdgeTextureId));

    Material sideMaterial;
    sideMaterial.color        = gameSettings.getBoardSideColorVec3();
    sideMaterial.textureId    = boardEdgeTexture;
    sideMaterial.textureScale = 1.75f;
    sideMaterial.useTexture   = (boardEdgeTexture != 0);

    glRenderer.drawCube(viewProjection, northSide, sideMaterial);
    glRenderer.drawCube(viewProjection, southSide, sideMaterial);
    glRenderer.drawCube(viewProjection, eastSide, sideMaterial);
    glRenderer.drawCube(viewProjection, westSide, sideMaterial);
}

void ChessSceneRenderer::drawSinglePiece(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ, float topY, const ResourceManager& resourceManager, bool isHovered, const glm::vec3& hoverColor) const
{
    if (piece == nullptr)
        return;

    const std::size_t pieceIndex = pieceTypeIndex(piece->getType());
    if (pieceIndex >= PIECES_HEIGHT.size())
        return;

    Material  pieceMaterial;
    glm::vec3 pieceColor = colorForPiece(piece);
    if (isHovered)
    {
        constexpr float hoverBlend = 0.55f;
        pieceColor                 = pieceColor * (1.f - hoverBlend) + hoverColor * hoverBlend;
    }
    pieceMaterial.color = pieceColor;

    const float pieceHeight = PIECES_HEIGHT[pieceIndex] * 1.35f;
    const float worldX      = originX + boardX;
    const float worldZ      = originZ + boardY;

    const ResourceManager::PieceMeshGlData* modelMesh = resourceManager.getPieceMeshFor(piece->getType());

    if (modelMesh != nullptr && modelMesh->isValid())
    {
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + yOffset, worldZ})
                                * glm::scale(glm::mat4{1.f}, glm::vec3{pieceHeight, pieceHeight, pieceHeight});

        if (!modelMesh->submeshes.empty())
        {
            for (const auto& submesh : modelMesh->submeshes)
            {
                Material submeshMaterial = pieceMaterial;
                const unsigned int textureId = (submesh.textureId != 0) ? submesh.textureId : modelMesh->textureId;
                submeshMaterial.textureId = textureId;
                submeshMaterial.useTexture = (textureId != 0);
                submeshMaterial.useMeshUv = true;
                submeshMaterial.color = pieceColor * submesh.baseColorFactor;

                glRenderer.drawIndexedMesh(viewProjection,
                                           model,
                                           submeshMaterial,
                                           modelMesh->vao,
                                           static_cast<int>(submesh.indexCount),
                                           static_cast<std::size_t>(submesh.indexOffset));
            }
        }
        else
        {
            glRenderer.drawIndexedMesh(viewProjection, model, pieceMaterial, modelMesh->vao, modelMesh->indexCount);
        }
    }
    else
    {
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + pieceHeight * 0.5f + yOffset, worldZ})
                                * glm::scale(glm::mat4{1.f}, glm::vec3{PIECE_BASE_WIDTH, pieceHeight, PIECE_BASE_WIDTH});

        glRenderer.drawCube(viewProjection, model, pieceMaterial);
    }
}

void ChessSceneRenderer::drawSingleExplodingPiece(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Piece* piece, float boardX, float boardY, float yOffset, float originX, float originZ, float topY, float explosionProgress, const ResourceManager& resourceManager) const
{
    if (piece == nullptr)
        return;

    const std::size_t pieceIndex = pieceTypeIndex(piece->getType());
    if (pieceIndex >= PIECES_HEIGHT.size())
        return;

    const float pieceHeight = PIECES_HEIGHT[pieceIndex] * 1.35f;
    const float worldX      = originX + boardX;
    const float worldZ      = originZ + boardY;

    Material pieceMaterial;
    pieceMaterial.color = colorForPiece(piece);

    const ResourceManager::PieceMeshGlData* modelMesh = resourceManager.getPieceMeshFor(piece->getType());
    if (modelMesh != nullptr && modelMesh->isValid())
    {
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + yOffset, worldZ})
                                * glm::scale(glm::mat4{1.f}, glm::vec3{pieceHeight, pieceHeight, pieceHeight});

        if (!modelMesh->submeshes.empty())
        {
            for (const auto& submesh : modelMesh->submeshes)
            {
                glRenderer.drawExplosionIndexedMesh(viewProjection,
                                                    model,
                                                    pieceMaterial,
                                                    modelMesh->vao,
                                                    static_cast<int>(submesh.indexCount),
                                                    static_cast<std::size_t>(submesh.indexOffset),
                                                    explosionProgress);
            }
        }
        else
        {
            glRenderer.drawExplosionIndexedMesh(viewProjection, model, pieceMaterial, modelMesh->vao, modelMesh->indexCount, explosionProgress);
        }
    }
    else
    {
        const glm::mat4 model = glm::translate(glm::mat4{1.f}, glm::vec3{worldX, topY + PIECE_LIFT_Y + pieceHeight * 0.5f + yOffset, worldZ})
                                * glm::scale(glm::mat4{1.f}, glm::vec3{PIECE_BASE_WIDTH, pieceHeight, PIECE_BASE_WIDTH});

        glRenderer.drawExplosionCube(viewProjection, model, pieceMaterial, explosionProgress);
    }
}

void ChessSceneRenderer::drawPieces(GLRenderer& glRenderer, const glm::mat4& viewProjection, const Board& board, const settings& gameSettings, const ResourceManager& resourceManager, PieceColor currentTurn, const SelectionState& selection, const PieceAnimator::AnimatedPiecePositions& animatedPiecePositions, const ExplodingPiecePositions& explodingPiecePositions) const
{
    (void)currentTurn;

    const float boardOriginX = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardOriginZ = -(static_cast<float>(Board::SIZE) - 1.f) * 0.5f;
    const float boardTopY    = BOARD_CENTER_Y + gameSettings.boardThickness * 0.5f;

    const ImVec4    hoverColorRaw = gameSettings.getHighlight();
    const glm::vec3 hoverColor{hoverColorRaw.x, hoverColorRaw.y, hoverColorRaw.z};

    for (int y = 0; y < Board::SIZE; ++y)
    {
        for (int x = 0; x < Board::SIZE; ++x)
        {
            const Case& currentCase = board.getCase(x, y);
            if (currentCase.getHasPiece())
            {
                const Piece* piece = currentCase.getPiece();

                const bool isHovered = selection.hoveredSelectable.has_value()
                                       && static_cast<int>(selection.hoveredSelectable->getX()) == x
                                       && static_cast<int>(selection.hoveredSelectable->getY()) == y;

                float boardX  = static_cast<float>(x);
                float boardY  = static_cast<float>(y);
                float yOffset = 0.f;

                const auto animatedPosition = animatedPiecePositions.find(piece);
                if (animatedPosition != animatedPiecePositions.end())
                {
                    boardX  = animatedPosition->second.x;
                    boardY  = animatedPosition->second.y;
                    yOffset = animatedPosition->second.z;
                }

                drawSinglePiece(glRenderer, viewProjection, piece, boardX, boardY, yOffset, boardOriginX, boardOriginZ, boardTopY, resourceManager, isHovered, hoverColor);
            }
        }
    }

    if (explodingPiecePositions.empty())
        return;

    ExplosionPassSettings explosionSettings;
    explosionSettings.lightDirection  = glm::vec3{-0.35f, 1.f, 0.25f};
    explosionSettings.ambientStrength = 0.25f;

    if (!glRenderer.beginExplosionPass(explosionSettings))
        return;

    for (const auto& [piece, explosion] : explodingPiecePositions)
    {
        drawSingleExplodingPiece(glRenderer, viewProjection, piece, explosion.position.x, explosion.position.y, explosion.position.z, boardOriginX, boardOriginZ, boardTopY, explosion.progress, resourceManager);
    }

    glRenderer.endExplosionPass();
}

} // namespace Render3D
