#pragma once

#include <optional>
#include <utility>
#include "Board/Board.hpp"
#include "Game/settings.hpp"
#include "Scene3D.hpp"
#include "TextureManager.hpp"


struct BoardClick {
    int x = 0;
    int y = 0;
};

class Renderer {
public:
    explicit Renderer(TextureManager& textures);

    void                      initialize(const AppConfig& config);
    std::optional<BoardClick> draw(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition);

private:
    std::optional<BoardClick> draw3DBoard(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds, std::optional<std::pair<int, int>> kirbyPosition);
    std::optional<BoardClick> draw2DBoard(const Board& board, const settings& gameSettings, std::optional<std::pair<int, int>> kirbyPosition);
    bool                      draw2DCase(const Board& board, const settings& gameSettings, int x, int y, std::optional<std::pair<int, int>> kirbyPosition);

    TextureManager _textures;
    Scene3D        _scene3D;
};