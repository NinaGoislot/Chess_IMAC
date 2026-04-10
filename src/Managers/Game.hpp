#pragma once

#include <array>
#include "Board/Board.hpp"
#include "Game/Pieces/Piece.hpp"
#include "Game/Player.hpp"
#include "Game/settings.hpp"
#include "Render/Renderer.hpp"

class Game
{
public:
    Game();

    void placePieces();
    void displayBoard(const settings& gameSettings, float deltaTimeSeconds);
    void init();

private:
    void placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner);

    Board _board;
    TextureManager _textures;
    Renderer _renderer;
    std::array<Player, 2> _players;
    PieceColor _currentTurn = PieceColor::White;
};