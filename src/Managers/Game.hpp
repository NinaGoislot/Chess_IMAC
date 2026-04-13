#pragma once

#include <array>
#include <memory>
#include "Board/Board.hpp"
#include "Game/Pieces/Piece.hpp"
#include "Game/Player.hpp"
#include "Game/Promotion/PromotionFlow.hpp"
#include "Game/settings.hpp"
#include "Managers/TurnManager.hpp"
#include "Render/Renderer.hpp"

class ChaosMode;
struct ChaosOptions;

// Central game singleton that owns board, players, rules, and rendering flow.
class Game {
public:
    enum class Mode {
        Classic,
        Chaos,
    };

    static Game& instance();

    // Constructors
    Game(const Game&)            = delete;
    Game(Game&&)                 = delete;
    Game& operator=(const Game&) = delete;
    Game& operator=(Game&&)      = delete;
    ~Game();

    // Getters
    settings&                       getSettings();
    const settings&                 getSettings() const;
    const std::vector<std::string>& getMoveHistory() const;
    ChaosOptions&                   getChaosOptionsMutable();
    const ChaosOptions&             getChaosOptions() const;
    PromotionFlow&                  getPromotionFlow() { return _promotionFlow; }
    Mode                            getMode() const { return _mode; }
    const TurnManager&              getTurnManager() const { return _turnManager; }
    TurnManager&                    getTurnManager() { return _turnManager; }

    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

    // Init function: loads resources and prepares systems.
    void initialize(const AppConfig& config);
    // Starts a new game with selected mode.
    void newGame(Mode mode = Mode::Classic);
    // Debug/helper function that prints board state.
    void displayBoard();

private:
    Game();

    void placePieces();
    void placePieceForPlayer(int x, int y, PieceType type, Player& owner);
    void placeBackRankPieces(int y, Player& owner);
    void placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner);
    void onValidatedMoveAdvanced();

    // Private game state and runtime dependencies.
    Board                      _board;
    TextureManager             _textures;
    Renderer                   _renderer;
    std::array<Player, 2>      _players;
    TurnManager                _turnManager;
    std::unique_ptr<ChaosMode> _chaosMode;
    PromotionFlow              _promotionFlow;
    Mode                       _mode      = Mode::Classic;
    int                        _turnCount = 1;
    settings                   _settings;
    std::vector<std::string>   _moveHistory;
};

