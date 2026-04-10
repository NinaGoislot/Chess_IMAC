#pragma once

#include "Board/Board.hpp"
#include "Board3DRenderer.hpp"
#include "Game/settings.hpp"
#include "TextureManager.hpp"

class Renderer {
public:
    explicit Renderer(TextureManager& textures);

    void initialize();
    void draw(Board& board, const settings& gameSettings, PieceColor currentTurn, float deltaTimeSeconds);

private:
    TextureManager  _textures;
    Board3DRenderer _board3DRenderer;
};