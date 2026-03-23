#include "Managers/Game.hpp"
#include <imgui.h>
#include <array>
#include <memory>
#include "Game/PieceFactory.hpp"
#include "Game/settings.hpp"

// #include "quick_imgui/quick_imgui.hpp"

void Game::init(){}

Game::Game()
    : _board()
    , _textures()
    , _boardRenderer(_textures)
    , _players{Player(PieceColor::White, "White"), Player(PieceColor::Black, "Black")}
{
    _textures.load();
    placePieces();
}

void Game::placePieces()
{
    _players[0].resetPieces();
    _players[1].resetPieces();

    Player& whitePlayer = _players[0];
    Player& blackPlayer = _players[1];

    placeBackRankPieces(0, whitePlayer);
    placeBackRankPieces(7, blackPlayer);
    for (int x = 0; x < Board::SIZE; ++x)
    {
        placePieceForPlayer(x, 1, PieceType::Pawn, whitePlayer);
        placePieceForPlayer(x, 6, PieceType::Pawn, blackPlayer);
    }
}

void Game::displayBoard(const settings& gameSettings)
{
    _boardRenderer.draw(_board, gameSettings, _currentTurn);
}

void Game::placePieceForPlayer(int x, int y, PieceType type, Player& owner)
{
    const PieceColor color   = owner.color();
    ImTextureID texture = _textures.getPieceTexture(color, type);

    std::unique_ptr<Piece> piece = PieceFactory::create(type, color, texture);
    Piece*                 raw   = piece.get();

    _board.getCase(x, y).setPiece(std::move(piece));
    owner.addPiece(*raw);
}

void Game::placeBackRankPieces(int y, Player& owner)
{
    const std::array<PieceType, 8> order = {
        PieceType::Rook,
        PieceType::Knight,
        PieceType::Bishop,
        PieceType::Queen,
        PieceType::King,
        PieceType::Bishop,
        PieceType::Knight,
        PieceType::Rook,
    };

    for (int x = 0; x < Board::SIZE; ++x)
    {
        placePieceForPlayer(x, y, order[x], owner);
    }
}