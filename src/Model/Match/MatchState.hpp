#pragma once
#include <array>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Model/Board/Board.hpp"
#include "Model/Chaos/ChaosMode.hpp"
#include "Model/Match/Player.hpp"
#include "Model/Match/GameRules.hpp"
#include "Model/Match/History.hpp"
#include "Model/Match/Promotion/PromotionService.hpp"
#include "Managers/TurnManager.hpp"
#include "Render/TextureManager.hpp"
#include "utilities/MoveAttempt.hpp"

// Owns a complete chess match state and orchestrates rules, move execution, and chaos
class MatchState {
public:
    enum class Mode {
        Classic,
        Chaos,
    };

    // Constructors
    explicit MatchState(const TextureManager& textures);

    // Lifecycle
    void newMatch(Mode mode = Mode::Classic);

    // Rules
    bool tryMove(Vector2D from, Vector2D to);
    bool choosePromotion(PieceType type);
    void cancelPendingPromotion();

    // UI helpers
    bool                  canSelect(Vector2D tile) const;
    std::vector<Vector2D> getLegalMovesFrom(Vector2D from) const;

    using PendingPromotionInfo = PromotionService::PendingPromotionInfo; // set an alias

    // Getters
    const Board&                        getBoard() const;
    Board&                              getBoard();
    PieceColor                          getCurrentTurn() const;
    TurnManager&                        getTurnManager();
    const TurnManager&                  getTurnManager() const;
    const std::vector<std::string>&     getMoveHistory() const;
    const std::string&                  getWhitePlayerName() const;
    const std::string&                  getBlackPlayerName() const;
    int                                 getFullTurnCount() const;
    bool                                getHasWinner() const;
    const Player*                       getWinner() const;
    std::optional<std::pair<int, int>>  getKirbyPosition() const;
    bool                                getHasKirbyAt(int x, int y) const;
    bool                                getHasPendingPromotion() const;
    std::optional<PendingPromotionInfo> getPendingPromotion() const;
    Mode                                getMode() const;
    ChaosOptions&                       getChaosOptionsMutable();
    const ChaosOptions&                 getChaosOptions() const;

    // ADD functions
    void addPlayerWhite(const std::string& name);
    void addPlayerBlack(const std::string& name);
    void addMoveToHistory(const std::string& move);

private:
    bool applyChaosPreMove(MoveAttempt& attempt); // Pre-move chaos hook. Can alter or cancel attempt
    void consumeCapturedPiece(Piece* capturedPiece);

    // Initial board setup
    void placePieces();
    void placePiecesForPlayer(int backRankY, int pawnRankY, Player& owner);

    void applyTurnProgression();

    // Parameters : Core match state
    Board                 _board;
    TurnManager           _turnManager;
    std::array<Player, 2> _players;
    MoveHistory           _moveHistory;

    // Parameters : Stateless services
    GameRules        _rules;
    PromotionService _promotion;

    // Parameters : chaos and texture dependency
    std::unique_ptr<ChaosMode> _chaosMode;
    const TextureManager*      _textures = nullptr;

    // Parameters : Current match mode
    Mode   _mode               = Mode::Classic;
    int    _validatedMoveCount = 0;
    Player* _winner            = nullptr;
};