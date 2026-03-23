#pragma once

#include "Board/Board.hpp"
#include "Game/settings.hpp"
#include "TextureManager.hpp"

class BoardRenderer
{
public:
    explicit BoardRenderer(TextureManager& textures);

    void draw(Board& board, const settings& gameSettings) const;

private:
    TextureManager _textures;
};