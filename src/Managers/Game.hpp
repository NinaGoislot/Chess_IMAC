#pragma once

#include <imgui.h>
#include "Board/Board.hpp"
#include "Render/BoardRenderer.hpp"
#include "Game/settings.hpp"


class Game
{
public:
    Game();

    void placePieces();
    void displayBoard(const settings& gameSettings);

private:
    Board _board;
    TextureManager _textures;
    BoardRenderer _boardRenderer;
};