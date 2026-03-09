#include "Game.hpp"
#include <imgui.h>
#include <memory>
#include "Game/Pieces/Pawn.hpp"

// #include "quick_imgui/quick_imgui.hpp"

Game::Game()
    : _board(), _textures(), _boardRenderer(_textures)
{
    _textures.load();

    _board.getCase(0,0).setPiece(std::make_unique<Pawn>(_textures.whitePawn));
    _board.getCase(1,1).setPiece(std::make_unique<Pawn>(_textures.whitePawn));
    _board.getCase(2,2).setPiece(std::make_unique<Pawn>(_textures.whitePawn));
    _board.getCase(3,3).setPiece(std::make_unique<Pawn>(_textures.whitePawn));
}

void Game::placePieces()
{
    _board.getCase(0, 0).setPiece(std::make_unique<Pawn>(_textures.whitePawn));
}

void Game::displayBoard(const settings& gameSettings)
{
    _boardRenderer.draw(_board, gameSettings);
}