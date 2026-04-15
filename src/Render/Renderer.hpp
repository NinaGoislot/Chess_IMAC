#pragma once

#include <optional>
#include <utility>
#include "Board/Board.hpp"
#include "Game/State/SelectionState.hpp"
#include "Game/settings.hpp"
#include "Scene3D.hpp"
#include "TextureManager.hpp"
#include "utilities/AppConfig.hpp"


struct BoardClick {
    int x = 0;
    int y = 0;
};

// Chooses 2D or 3D board rendering path and returns clicked tile when relevant.
class Renderer {
public:
    // Constructors
    explicit Renderer(TextureManager& textures);

    // Init function: prepares renderer resources from app configuration.
    void                      initialize(const AppConfig& config);
    // Render function: draws current board and returns optional clicked tile.
    std::optional<BoardClick> draw(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds,
                                   std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection);
    // Hover helper: returns the hovered tile from the latest draw, when available.
    std::optional<BoardClick> getHoveredTile() const { return _hoveredTile; }

private:
    // Render function: draws the board through the 3D pipeline.
    std::optional<BoardClick> draw3DBoard(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds,
                                          std::optional<std::pair<int, int>> kirbyPosition, const SelectionState& selection);
    // Render function: draws the board through the 2D ImGui pipeline.
    std::optional<BoardClick> draw2DBoard(const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition,
                                          const SelectionState& selection);
    // Draws and handles one tile in 2D mode.
    bool                      draw2DCase(const Board& board, const settings& gameSettings, int x, int y, std::optional<std::pair<int, int>> kirbyPosition,
                                         const SelectionState& selection);

    // Non-owning texture manager reference used by 2D piece rendering.
    TextureManager& _textures;
    // 3D scene subsystem used when 3D mode is enabled.
    Scene3D         _scene3D;
    // Latest hovered board tile from the last draw pass.
    std::optional<BoardClick> _hoveredTile;
};
