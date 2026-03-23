#pragma once

#include "Board/Board.hpp"
#include "Board3DRenderer.hpp"
#include "Game/settings.hpp"
#include "TextureManager.hpp"

class BoardRenderer {
public:
    explicit BoardRenderer(TextureManager& textures);

    void draw(Board& board, const settings& gameSettings, PieceColor currentTurn);

private:
    TextureManager  _textures;
    Board3DRenderer _board3DRenderer;
};