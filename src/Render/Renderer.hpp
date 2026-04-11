#pragma once

#include <optional>

#include "Board/Board.hpp"
#include "Scene3D.hpp"
#include "Game/settings.hpp"
#include "TextureManager.hpp"

struct BoardClick
{
    int x = 0;
    int y = 0;
};

class Renderer {
public:
    explicit Renderer(TextureManager& textures);

    void initialize(const AppConfig& config);
    std::optional<BoardClick> draw(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds);

private:
    std::optional<BoardClick> draw3DBoard(const Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds);
    std::optional<BoardClick> draw2DBoard(const Board& board, const settings& gameSettings);
    bool                      draw2DCase(const Board& board, const settings& gameSettings, int x, int y);

    TextureManager  _textures;
    Scene3D       _scene3D;
};