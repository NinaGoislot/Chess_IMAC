#pragma once

#include <array>
#include "Board/Board.hpp"
#include "Game/Pieces/Piece.hpp"
#include "Game/Player.hpp"
#include "Game/settings.hpp"
#include "Render/BoardRenderer.hpp"

class Game {
public:
    static Game& instance();

    // Constructors
    Game(const Game&)            = delete;
    Game(Game&&)                 = delete;
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&)      = delete;
    ~Game()                      = default;

     // Getters
    settings&                       getSettings();
    const settings&                 getSettings() const;
    const std::vector<std::string>& getMoveHistory() const;

    // Setters
    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

    // Functions
    void newGame();
    void displayBoard();

private:
    Game();

    void placePieces();
    void placePieceForPlayer(int x, int y, PieceType type, Player& owner);
    void placeBackRankPieces(int y, Player& owner);
    void displayBoard(const settings& gameSettings);
    void placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner);

    // Parameters
    Board                    _board;
    TextureManager           _textures;
    BoardRenderer            _boardRenderer;
    std::array<Player, 2>    _players;
    PieceColor               _currentTurn = PieceColor::White;
    settings                 _settings;
    std::vector<std::string> _moveHistory;
};